/* 788.d
**
** Created 09/15/2015 by jig from 787b.d
**
*/

/* HISTORY:
** LD 2017/01/23 changed paradigm to have fixed switch times, high coh for epoch 1 and randomized dir for epoch 2
**			to do that, also added a fourth task to have two sets of coherences for epoch 2.
** LD 2016/04/18 removed feedback for fixation break. 
** LD 2016/04/14 added feedback for fixation break and error trials.
**	LD 2016/05/11 added options to reward dotsrev and non-dotsrevi tasks differently
**
**-------------
** 787b.d
** 2014/02/17 td created 787b.d. Two offset values can be chosen (e.g. null dir and near-pref dir).  
**
**-------------
** 787.d
** 2014/01/28 td created 787.d (adaptation to motion-direction chagne) from 786.d (motion detection)
**
** 2014/02/06 yl: can do offset angle that is other than 180
**
**-------------
** 786.d
** 
** 2013/11/** yl created this spot file for a new detection task  
** 2013/12/03 td inserted fixation break for t4waitpre. 
** 2013/12/09 yl: - t4waitpost1 and 2 replaces t4waitpost ... can't saccade before post1
**                - drop ecodes for COHCHG, second I_COHCD and SACMAD for false alarms
** 2013/12/10 td: - use ec_send_code_tagged() instead of pl_ecodes_by_nameV() to drop I_COHCD
**                - reading second coh value and drop I_COHCD at the beginning of the trial to avoid possible delay 
**                  moved part of do_set_coh() to do_t4init()
**                - Turn off Dots by dx_toggle2 in fixbreak_detrct
**                - Turn off Dots by dx_toggle2 after valid con-change detection 
**                - Moved dummy window to t4waitpost1.
**                - Made thold and nhold more symmetric
**                - Use 400ms for the error target acquisition (be careful about root file setting)
**
**
**-------------
** 785yltd.d
**  
** 2013/06/24 td created from 785yl.d 
**  dotsj2 is used instead of dotsj (speed test and size test have been added)
**
**
**-------------
**	785yl.d
**
** Paradigm for the fixed duration 2afc w/ MT/LIP stim   
**	
**	2011/03/17 yl mod from 781.d
**	2011/05    yl incorporated estim
**	2011/05/17 yl uses improved task_dotsj wrt joy stick control
** 	2011/05/18 yl during LIP stim blocks: stim is the 'default' mode.
**	2011/07/08 yl added visually guide saccade as task5
** 	2011/07/08 yl MT stim now ends @ end of dots
** 	2011/07/11 yl after fixation, window centers on eye pos
**	2011/07/12 yl: (1) FP/target window size = 50
**	               (2) FP window does not center on eye pos
**	               (3) FPDIAM: 5->3 (orig 3->4)
**	               (4) must wait 500 ms to get reward for automated ASL
**	               (5) always turn off T2 first as feedback
**	               (6) use pr_finish_trial_yl -- does not toggle OFF plexon
**	               (7) toggle OFF plexon if PAUSED
** 2011/07/14 yl: always drop FIX1CD
** 2011/10/25 yl: add delay after dots off for MT mapping
** 2011/12/05 yl: add stim_toggle_ON2/stim_bit_ON2 for LIP stim v MT stim
**						LIP and MT stim should now be two separate systems
** 2012/02/07 yl:	1. no visual feedback
**					2. rewards per check_rew & rew_prob 
*/

#include "rexHdr.h"
#include "paradigm_rec.h"
#include "toys.h"
#include "lcode.h"

/* PRIVATE data structures */

/* GLOBAL VARIABLES */
static _PRrecord	gl_rec=NULL;      /* KA-HU-NA */
static int        gl_dots_epoch=10; /* 10=epoch 1, 20=epoch 2 */
static int        gl_dots_set=1;    /* set 1 or 2 of coh/dir values */
static int			gl_xtime=0;			/* whether to add extra timeout */
static int			gl_repeat=0;		/* if repeat trial */
static int 			last_dots_set = 1; /* last dots dir */		

int errorT = 1;			/* LD 2016-04-14, added to keep track of the wrong target for feedback */

/* for now, allocate these here... */
MENU        umenus[100];		/* note: remember to expand if menu # gets bigger!) */
RTVAR       rtvars[15];
USER_FUNC   ufuncs[15];

/* CONSTANTS/MACROS */
#define TIMV(n)	pl_list_get_v(gl_rec->trialP->task->task_menus->lists[0],(n))
#define TIMW(n)	pl_list_get_w(gl_rec->trialP->task->task_menus->lists[0],(n))

#define WIND0 0		/* window to compare with eye signal (FP) */
#define WIND1 1		/* window to compare with eye signal (T1) */
#define WIND2 2		/* window to compare with eye signal (T2) */

#define EYEH_SIG 	0
#define EYEV_SIG 	1

#define R0 0
#define R1 1
#define R2 2

/* ROUTINES */

/*
** INITIALIZATION routines
*/
/* ROUTINE: autoinit
**
**	Initialize gl_rec. This will automatically
**		set up the menus, etc.
**  tasks:
**  TASK0:		ASL calibration
**  TASK1:		fixation w/ target (VGS)
**  TASK2:		passive fixation with dots (MT mapping)
**  TASK3:		dots-reversal task #1
**  TASK4:		dots-reversal task #2
**  TASK5:		dots-reversal task #3
**  TASK6:     dots-reversal task #4
*/
void autoinit(void)
{

	gl_rec = pr_initV(0, 0, umenus, NULL, rtvars,
		pl_list_initV("rtvars", 0, 1, "angle", 0, 1.0, NULL),
		ufuncs, 
		"asl",         1,  /* calibration */
		"ft",          1,  /* VGS */
		"dotsj2",      1,  /* Dots during passive fixation for MT mapping */
		"dots_rev",		4,  /* dots-reversal task */
		NULL);
}

/* ROUTINE: rinitf
**
** initialize at first pass or at r s from keyboard 
*/
void rinitf(void)
{
	static int first_time = 1;

	/* This stuff needs to be done only once, but also
	**		needs to be done after the clock has started
	**		(so do NOT put it up in autoinit).
	*/
	if(first_time) {

		/* do this once */
		first_time = 0;

		/* initialize interface (window) parameters */
		/* WIND0: FIXATION WINDOW */
		wd_src_pos(WIND0, WD_DIRPOS, 0, WD_DIRPOS, 0);
		wd_src_check(WIND0, WD_SIGNAL, EYEH_SIG, WD_SIGNAL, EYEV_SIG);

		/* WIND1: T1 WINDOW */
		wd_src_pos(WIND1, WD_DIRPOS, 0, WD_DIRPOS, 0);
		wd_src_check(WIND1, WD_SIGNAL, EYEH_SIG, WD_SIGNAL, EYEV_SIG);

		/* WIND2: T2 WINDOW */
		wd_src_pos(WIND2, WD_DIRPOS, 0, WD_DIRPOS, 0);
		wd_src_check(WIND2, WD_SIGNAL, EYEH_SIG, WD_SIGNAL, EYEV_SIG);
		
      /* init the screen */
		pr_setup();
	}
}

/* ROUTINE: start_trial
*/
int start_trial(void)
{
	/* set draw flag for dynamic dots, if necessary */
	if(pr_get_task_index() > 1)
		dx_set_flags(DXF_D1);
	else
		dx_set_flags(DXF_D3);

	/* Start eye check in WIND0 */
	wd_cntrl(WIND0, WD_ON);
	
	return(0);
}

/* ROUTINE: finish_trial
**
** stuff that needs to be finished up 
**
*/
int finish_trial(long bad_trials)
{
	static int bad_trial_counter=0;

	/* don't check eye positions between trials */
	wd_cntrl(WIND0, WD_OFF);
	wd_cntrl(WIND1, WD_OFF);
	wd_cntrl(WIND2, WD_OFF);

	/* Bad_trials is a scalar that indicates how many bad 
	** 	(i.e., non-rewarded) trials in a row need to happen
	**		before giving an extra time-out in "xtime" state
	*/
	gl_xtime = 0;
	if(bad_trials>0 && gl_rec->pmf->last_reward==0)
		if(++bad_trial_counter==bad_trials)
			gl_xtime=1;
	else
		bad_trial_counter=0;

	return(0);
}

/* ROUTINE: check_xtime
**
*/
int check_xtime(void)
{
	return(gl_xtime);
}

/* ROUTINE: do_calibration

**	Returns 1 if doing calibration (ASL task)
*/
int do_calibration(void)
{
	if( pr_get_task_index() == 0 && 
		pr_get_task_menu_value("setup", "Cal0/Val1", 0) == 0) {
			printf("do_calibration == 1\n");
		return(1);
	} else {
		return(0);
	}
}

/* ROUTINE: check_fixflag
**
*/
int check_fixflag(void)
{
	// Fixation task
	if(TIMV("Fixation_task"))
		return(1);

	// Saccade task -- turn on eye check in WIND1
	wd_cntrl(WIND1, WD_ON);
	return(0);
}

/* ROUTINE: setup_dots
**
**	position windows around T1 and T2, reset globals
*/
int setup_dots(long set_probability)
{
/*	static int last_dots_set = 1; */ 
/* commented out LD 1-25-2017 */
/* should be a global variable. */

	/* reset global keeping track of current epoch */
	gl_dots_epoch = 10;

	/* keep track of which "set" we are using (1 or 2, corresponding
	**  to the coh/dir pair for the current epoch), using 
	**  set_probability (0=always 1; 500=50/50; 1000=always 2)
	**  Note that gl_dots_set is always flipped at the beginning
	**		of switch_dots, so setting the complement here
	**
	** 3/14/16: jig changed so that the same dots set is used
	**		after errors, based on "Trial repeats" in the paradigm menu
	*/
	if(pl_list_get_w(gl_rec->paradigm_menu, "Trial_repeats")==1)
		gl_dots_set = last_dots_set;
	else
		last_dots_set = gl_dots_set = TOY_RCMP(set_probability) ? 1 : 2;

	/* send appropriate draw commands */
	switch_dots(0);

	/* hide windows */
	dx_position_window(0, 0, 1, 0, WIND1);
	dx_position_window(0, 0, 2, 0, WIND2);

	/* outta */
	return(0);
}

/* ROUTINE: set_dots_timers
**
**	Set timer from Task Info Menu
**	-> timer1 is for epoch
**	-> timer2 is for switch
*/
int set_dots_timers(void)
{
	static int epoch_times[2], switch_times[100], counter=0; /* for repeat trials */
	int repeat = pl_list_get_w(gl_rec->paradigm_menu, "Trial_repeats");

	/* setup timer for beginning of each epoch */
	if(gl_dots_epoch==10) {    /* beginning of first epoch */

		if (repeat==1)
			timer_set1(0,0,0,0,epoch_times[0],0);
		else if (TIMV("Epoch1_DurMin") == TIMV("Epoch1_DurMax"))
			epoch_times[0] = timer_set1(0,0,0,0,TIMV("Epoch1_DurMin"),0);
		else
			epoch_times[0] = timer_set1(1000,TIMV("Epoch1_DurMin"),TIMV("Epoch1_DurMax"),
				TIMV("Epoch1_DurMean"),0,0);

		/* flag to indicate we are in the middle of the first epoch, plus reset counter */
		gl_dots_epoch	= 11;
		counter 			= 0;

	} else if(gl_dots_epoch==20) { /* beginning of second epoch */

		if (repeat==1)
			timer_set1(0,0,0,0,epoch_times[1],0);
		else if (TIMV("Epoch2_DurMin") == TIMV("Epoch2_DurMax"))
			epoch_times[1] = timer_set1(0,0,0,0,TIMV("Epoch2_DurMin"),0);
		else
			epoch_times[1] = timer_set1(1000,TIMV("Epoch2_DurMin"),TIMV("Epoch2_DurMax"),
				TIMV("Epoch2_DurMean"),0,0);

		/* flag to indicate we are in the middle of the second epoch */
		gl_dots_epoch	= 21;
	}

	/* setup timer for beginning of each within-epoch switch */
	if (repeat==1) {
		timer_set2(0,0,0,0,switch_times[counter++],0);
	} else {
		if(gl_dots_epoch==11) {
			if (TIMV("Epoch1_RateFlag") == 1) // Fixed time between switches
				switch_times[counter++] = timer_set2(0,0,0,0,TIMV("Epoch1_Rate"),0);
			else
				switch_times[counter++] = timer_set2(1000,TIMV("Min_Switch_Dur"),
					TIMV("Max_Switch_Dur"),TIMV("Epoch1_Rate"),0,0);

		} else if(gl_dots_epoch==21) {
			if (TIMV("Epoch2_RateFlag") == 1) // Fixed time between switches
				switch_times[counter++] = timer_set2(0,0,0,0,TIMV("Epoch2_Rate"),0);
			else
				switch_times[counter++] = timer_set2(1000,TIMV("Min_Switch_Dur"),
					TIMV("Max_Switch_Dur"),TIMV("Epoch2_Rate"),0,0);
		}
	}

	return(0);
}

/* ROUTINE: check_hide_dots
**
**	Is timer 1 (epoch) done at end of SECOND epoch?
*/
int check_hide_dots(void)
{
	// Is second epoch over?
	if(timer_check1() && gl_dots_epoch==21)
		return(1);
	
	return(0);
}

/* ROUTINE: check_switch_dots
**
**	Is timer 1 (epoch) done at end of FIRST epoch
**			or if timer 2 (switch) done
*/
int check_switch_dots(void)
{
	// Is second epoch over?
	if(timer_check1() && gl_dots_epoch==11) {
		gl_dots_epoch=20;
		return(1);
	} else if(timer_check2()) {
		return(1);
	}

	return(0);
}

/* ROUTINE: switch_dots
**
**	Change dir/coh
*/
int switch_dots(long ecode)
{
	int     	gdi[]	= {0};
	valtype 	dir, coh;
	int 		epoch2_dir = 0;

	// update gl_dots_set
//	gl_dots_set = gl_dots_set==1 ? 2 : 1; 
/* commented out by LD 1-23-2017, do not force direction switch between epochs */

	// Get current values of direction, coherence
	if(gl_dots_epoch<20) {
   // update gl_dots_set
   gl_dots_set = gl_dots_set==1 ? 2 : 1;
		if(gl_dots_set==1) {
			dir = TIMW("Epoch1_Dir1");
			coh = TIMW("Epoch1_Coh1");
		} else {
			dir = TIMW("Epoch1_Dir2");
			coh = TIMW("Epoch1_Coh2");
		}
	} else {
		/* added by LD 1-23-2017 randomize epoch2_dir based on trial id  */
		gl_dots_set = gl_rec->trialP->id%2==0 ? 1 : 2;
		if(gl_dots_set==1) {
			dir = TIMW("Epoch2_Dir1");
			coh = TIMW("Epoch2_Coh1");
		} else {
			dir = TIMW("Epoch2_Dir2");
			coh = TIMW("Epoch2_Coh2");
		}
	}

printf("current coh = %d, dir = %d \n", coh, dir);

	// Set direction, coherence
	dx_set_by_nameIV(DXF_D1, DX_DOTS, 1, gdi,
				DX_COH, coh,
				DX_DIR, dir,
				NULL);

	// Drop coh, dir ecodes
	pl_ecodes_by_nameV(dx_get_object(DX_DOTS), 7000,
				DX_COH, 0, 	I_COHCD,
				DX_DIR, 0, 	I_DOTDIRCD,
				NULL);

	// Conditionally set draw ecode
	if(ecode)
		dx_set_ecode(ecode);

    return(0);
}

/* ROUTINE: end_dots
**
** Hide dots, position target windows, score
*/
int end_dots(long ecode, long width, long height)
{
	_PLlist	targets 	= dx_get_object("dXtarget"),
				dots 		= dx_get_object("dXdots");
	valtype 	t1x     	= pl_list_get_vi(targets, DX_X, 1),
				dir		= pl_list_get_vi(dots, DX_DIR, 0);

	// Turn off the dots
	dx_toggle1(ecode, 0, 3, 1000, NULLI, NULLI);

	// Turn on target windows
	wd_cntrl(WIND1, WD_ON);
	wd_cntrl(WIND2, WD_ON);

	// Determine current pref target & position window
	if( ((dir<=90 || dir>=270) && (t1x>=0)) ||
			((dir> 90 && dir< 270) && (t1x<0))) {
		printf("END: T1 is correct... t1x is %d, dir is %d\n", t1x, dir);
		dx_position_window(width, height, 1, 0, WIND1);
		dx_position_window(width, height, 2, 0, WIND2);
		errorT = 2;
	} else {
		printf("END: T2 is correct... t1x is %d, dir is %d\n", t1x, dir);
		dx_position_window(width, height, 2, 0, WIND1);
		dx_position_window(width, height, 1, 0, WIND2);
		errorT = 1;
	}
	
	/* Also update trial type accordingly... remember we always
	** 	start with trial pointer #1, which we will define as "set 1"...
	** 	so only switch if we ended on set 2 
	*/
	if(gl_dots_set==2) {
		_PRtask task 			= gl_rec->tasks[gl_rec->task_index];
		task->trialPs_index 	= 1;
		gl_rec->trialP 		= task->trialPs[task->trialPs_index];
	}
	
	return(0);
}


/* ROUTINE: err_feedback
** 
** Turn off the error target, as feedback
*/
int err_feedback(long ecode)
{
      /* turn off the error target */
   dx_toggle1(ecode, 0, errorT, 1000, NULLI, NULLI);
	return(0);
}



/* THE STATE SET 
*/
%%
id 788
restart rinitf
main_set {
status ON
begin	first:
		to loop

	/*
   ** Start the loop!
   ** Note that the real workhorse here is pr_next_trial,
   **    called in go. It calls the task-specific
   **    "get trial" and "set trial" methods,
   **    and drops STARTCD and LISTDONECD, as
   **    appropriate.
   **
   */
	loop:
		to pause on +PSTOP & softswitch /* pause for "Paradigm Stopped" button */
		to go
	pause:
		do pr_toggle_file(0)
		to go on -PSTOP & softswitch
	go:
		do pr_toggle_file(1)
		to trstart on MET % pr_start_trial
		to loop
	trstart:
		do start_trial()
		to showfp
	showfp:
		do dx_show_fp(FPONCD,0,0,0,0,0)*/ 	/* was: 0, 5, 8, 1, 8 */
		to caljmp on DX_MSG % dx_check
	caljmp: /* check for eye tracker calibration loop */
		to calstart on 1 % do_calibration
		to fpwinpos
	
	/* CALIBRATION TASK
	** Check for joystick button press indicating a correct fixation
	** missed targets are scored as NC in order to be shown again later
	*/	
    calstart:
		time 5000
		to calacc on 0 % dio_check_joybut
		to ncerr
    calacc:
		do ec_send_code(ACCEPTCAL)
		to correct

	/* 
	**
	** Position window and wait for fixation, then jump to task-specific
	**  statelist
	**
	*/
	fpwinpos:
		time 20  /* takes time to settle window */
 		do dx_position_window(50, 50, 0, 0, WIND0)
 		to fpwait
	fpwait:
 		time 5000
		to fpset on -WD0_XY & eyeflag
		to fpchange
	fpchange:
		to fpwait on 1 % dx_change_fp	
		to fpnofix
	fpnofix:    /* failed to attain fixation */
		time 2500
		do pr_score_trial(kNoFix,0,1)
		to finish
	fpset:
		time 50 /* give gaze time to settle into place (fixation) */
		do ec_send_code(EYINWD) 
		to fpwait on +WD0_XY & eyeflag
		to fpwin2
	fpwin2:
		time 20 /* again time to settle window */
		do dx_position_window(50, 50, -1, 1, 0)
		to taskjmp
	taskjmp:
		to t0start on 0 % pr_get_task_index /* ASL validation       */
		to t1start on 1 % pr_get_task_index /* VGS/MGS              */
		to t2start on 2 % pr_get_task_index	/* fixation with dots   */
		to t3start on 3 % pr_get_task_index	/* dots reversal # 1    */
		to t4start on 4 % pr_get_task_index	/* dots reversal # 2    */	
		to t5start on 5 % pr_get_task_index	/* dots reversal # 3    */
      to t6start on 6 % pr_get_task_index /* dots reversal # 4    */
		to finish

	/** 
    **
    ** TASK 0: ASL eye tracker validation  
    **  enters here only if cal0/val1 == 1
    **
    **/
	t0start:
		do timer_set1(0, 100, 600, 200, 500, 0)
 		to correct on MET % timer_check1
		to fixbreak on +WD0_XY & eyeflag
	 
	/** 
	**
	** TASK 1: visual- or memory- guided saccades
	**
	**/
	/* First wait period before showing the target */
	t1start:
		do timer_set1(1000, 100, 300, 200, 500, 0)
		to t1showt on MET % timer_check1
		to fixbreak on +WD0_XY & eyeflag
	t1showt:
		do dx_toggle1(TARGONCD, 1, 1, 1000, NULLI, NULLI)
		to t1wait2 on DX_MSG % dx_check

	/* Second wait period before possibly turning off target */
	t1wait2:
		do timer_set1(0,0,0,0,300,0)
		to t1hidet on MET % timer_check1
		to fixbreak on +WD0_XY & eyeflag
	t1hidet:
		do dx_toggle1(TARGOFFCD, 0, 1, 1000, NULLI, NULLI)
		to t1wait3 on DX_MSG % dx_check

	/* Third wait period before turning off fp */
	t1wait3:
		do timer_set1(1000, 500, 1000, 800, 500, 0)
		to t1hidefp on MET % timer_check1
		to fixbreak on +WD0_XY & eyeflag
	t1hidefp:
		do dx_hide_fp(FPOFFCD)
		to t1endcheck on DX_MSG % dx_check

	/* Figure out if fixation or saccade is required, then finish */
	t1endcheck:
		to correct on MET % check_fixflag
		to t1win
	/* position window #1 around target 1, for saccade */
	t1win:
		do dx_position_window(50, 50, 1, 0, WIND1) /* rex needs 20 ms to settle the window */
		time 20
		to grace
	
    /* 
    **
    **  TASK 2: passive fixation with dots
    **      wait
    **      show dots
    **      wait
    **      hide dots
    **      wait
    **
    */
	/* First wait period before showing the dots */
	t2start:
    	do timer_set1(1000, 100, 300, 200, 500, 0)
 		to t2showd on MET % timer_check1
		to fixbreak on +WD0_XY & eyeflag
	t2showd:
		do dx_toggle1(GORANDCD, 1, 3, 1000, NULLI, NULLI)
		to t2waitd on DX_MSG % dx_check
	/* Second wait while showing the dots */
	t2waitd:
		do timer_set1(0,0,0,0,1000,0)
		to t2hided on MET % timer_check1
		to fixbreak on +WD0_XY & eyeflag
	t2hided:
		do dx_toggle1(ENDCD, 0, 3, 1000, NULLI, NULLI)
		to t2wait2 on DX_MSG % dx_check
	/* final wait before fixation offset */
	t2wait2:
		do timer_set1(0,0,0,0,500,0)
		to t2hidefp on MET % timer_check1
		to fixbreak on +WD0_XY & eyeflag
	t2hidefp:
		do dx_hide_fp(FPOFFCD)
		to correct on DX_MSG % dx_check
	
	/* 
	**
	**  TASK 3: dots reversal 
	**
	*/
	t3start:
		do setup_dots(500)
		to t3wait1
	/* wait before turning on targets */
	t3wait1:
		do timer_set1(1000, 100, 300, 200, 0, 0)
		to t3showt on MET % timer_check1
		to fixbreak on +WD0_XY & eyeflag
	/* show targets */
	t3showt:
		do dx_toggle2(TARGONCD,1,1,1000,2,1000)
		to t3wait2 on DX_MSG % dx_check
	/* wait again before turning on dots */
	t3wait2:
		do timer_set1(1000, 100, 300, 200, 0, 0)
		to t3showd on MET % timer_check1
		to fixbreak on +WD0_XY & eyeflag
	/* show the dots */
	t3showd:
		do dx_toggle1(GORANDCD, 1, 3, 1000, NULLI, NULLI)
		to t3checkd on DX_MSG % dx_check
	/* check timers for switching/ending behavior */
	t3checkd:
		do set_dots_timers()
		to t3stopd   on MET % check_hide_dots
		to t3switchd on MET % check_switch_dots
		to fixbreak_dots on +WD0_XY & eyeflag
	t3switchd:
		do switch_dots(GORANDCD)
		to t3checkd on DX_MSG % dx_check
	/* stop the dots */
	t3stopd:
		do end_dots(ENDCD,50,50)
		to t3wait3 on DX_MSG % dx_check
	/* final wait before fixation offset */
	t3wait3:
		do timer_set1(0,0,0,0,500,0)
		to t3hidefp on MET % timer_check1
		to fixbreak_dots on +WD0_XY & eyeflag
	t3hidefp:
		do dx_hide_fp(FPOFFCD)
		to grace on DX_MSG % dx_check
	
    /* 
    **
    **  TASK 4: dots reversal 
    **
    */
	t4start:
		do setup_dots(500)
		to t4wait1
	/* wait before turning on targets */
	t4wait1:
		do timer_set1(1000, 100, 300, 200, 0, 0)
		to t4showt on MET % timer_check1
		to fixbreak on +WD0_XY & eyeflag
	/* show targets */
	t4showt:
		do dx_toggle2(TARGONCD,1,1,1000,2,1000)
		to t4wait2 on DX_MSG % dx_check
	/* wait again before turning on dots */
	t4wait2:
		do timer_set1(1000, 100, 300, 200, 0, 0)
		to t4showd on MET % timer_check1
		to fixbreak on +WD0_XY & eyeflag
	/* show the dots */
	t4showd:
		do dx_toggle1(GORANDCD, 1, 3, 1000, NULLI, NULLI)
		to t4checkd on DX_MSG % dx_check
	/* check timers for switching/ending behavior */
	t4checkd:
		do set_dots_timers()
		to t4stopd   on MET % check_hide_dots
		to t4switchd on MET % check_switch_dots
		to fixbreak_dots on +WD0_XY & eyeflag
	t4switchd:
		do switch_dots(GORANDCD)
		to t4checkd on DX_MSG % dx_check
	/* stop the dots */
	t4stopd:
		do end_dots(ENDCD,50,50)
		to t4wait3 on DX_MSG % dx_check
	/* final wait before fixation offset */
	t4wait3:
		do timer_set1(0,0,0,0,500,0)
		to t4hidefp on MET % timer_check1
		to fixbreak_dots on +WD0_XY & eyeflag
	t4hidefp:
		do dx_hide_fp(FPOFFCD)
		to grace on DX_MSG % dx_check

    /*
    **
    **  TASK 5: dots reversal
    **
    */
	t5start:
		do setup_dots(500)
		to t5wait1
	/* wait before turning on targets */
	t5wait1:
		do timer_set1(1000, 100, 300, 200, 0, 0)
		to t5showt on MET % timer_check1
		to fixbreak on +WD0_XY & eyeflag
	/* show targets */
	t5showt:
		do dx_toggle2(TARGONCD,1,1,1000,2,1000)
		to t5wait2 on DX_MSG % dx_check
	/* wait again before turning on dots */
	t5wait2:
		do timer_set1(1000, 100, 300, 200, 0, 0)
		to t5showd on MET % timer_check1
		to fixbreak on +WD0_XY & eyeflag
	/* show the dots */
	t5showd:
		do dx_toggle1(GORANDCD, 1, 3, 1000, NULLI, NULLI)
		to t5checkd on DX_MSG % dx_check
	/* check timers for switching/ending behavior */
	t5checkd:
		do set_dots_timers()
		to t5stopd   on MET % check_hide_dots
		to t5switchd on MET % check_switch_dots
		to fixbreak on +WD0_XY & eyeflag
	t5switchd:
		do switch_dots(GORANDCD)
		to t5checkd on DX_MSG % dx_check
	/* stop the dots */
	t5stopd:
		do end_dots(ENDCD,50,50)
		to t5wait3 on DX_MSG % dx_check
	/* final wait before fixation offset */
	t5wait3:
		do timer_set1(0,0,0,0,500,0)
		to t5hidefp on MET % timer_check1
		to fixbreak on +WD0_XY & eyeflag
	t5hidefp:
		do dx_hide_fp(FPOFFCD)
		to grace on DX_MSG % dx_check

    /*
    **
    **  TASK 6: dots reversal
    **
    */
   t6start:
      do setup_dots(500)
      to t6wait1
   /* wait before turning on targets */
   t6wait1:
      do timer_set1(1000, 100, 300, 200, 0, 0)
      to t6showt on MET % timer_check1
      to fixbreak on +WD0_XY & eyeflag
   /* show targets */
   t6showt:
      do dx_toggle2(TARGONCD,1,1,1000,2,1000)
      to t6wait2 on DX_MSG % dx_check
   /* wait again before turning on dots */
   t6wait2:
      do timer_set1(1000, 100, 300, 200, 0, 0)
      to t6showd on MET % timer_check1
      to fixbreak on +WD0_XY & eyeflag
   /* show the dots */
   t6showd:
      do dx_toggle1(GORANDCD, 1, 3, 1000, NULLI, NULLI)
      to t6checkd on DX_MSG % dx_check
   /* check timers for switching/ending behavior */
   t6checkd:
      do set_dots_timers()
      to t6stopd   on MET % check_hide_dots
      to t6switchd on MET % check_switch_dots
      to fixbreak on +WD0_XY & eyeflag
   t6switchd:
      do switch_dots(GORANDCD)
      to t6checkd on DX_MSG % dx_check
   /* stop the dots */
   t6stopd:
      do end_dots(ENDCD,50,50)
      to t6wait3 on DX_MSG % dx_check
   /* final wait before fixation offset */
   t6wait3:
      do timer_set1(0,0,0,0,500,0)
      to t6hidefp on MET % timer_check1
      to fixbreak on +WD0_XY & eyeflag
   t6hidefp:
      do dx_hide_fp(FPOFFCD)
      to grace on DX_MSG % dx_check


    /* 
    **
    ** Check for saccade, then for hold times
    **
    */
	grace:
		time 500    /* allow for eye movement */
		to sacmad on +WD0_XY & eyeflag
		to ncerr
	sacmad:
		do ec_send_code(SACMAD)
		time 100
		to pref on -WD1_XY & eyeflag
		to null on -WD2_XY & eyeflag
		to ncerr
    pref:
		time 400
		do ec_send_code(TRGACQUIRECD)
		to ncerr on +WD1_XY & eyeflag
		to correct
    null:
		time 100
		do ec_send_code(TRGACQUIRECD)
		to ncerr on +WD2_XY & eyeflag
		to error_dots

	/* OUTCOMES:
	**  Fixation break
	**  No-Choice Error
	**  Error
	**  Correct
	*/

	/* fixation break */

	/* flash fixation point for fix break on dots task */
	fixbreak_dots:
		to fixbreak
/*
      do dx_toggle1(FDBKONCD, 0, 0, 1000, NULLI, NULLI)
		to fixbreak_timeout on DX_MSG % dx_check
*/
	
	fixbreak_timeout:
      do timer_set1(0, 0, 0, 0, 1000, 0)
		to fixbreak_flashon
	fixbreak_flashon:
      do dx_toggle1(0, 1, 0, 1000, NULLI, NULLI)
		to fixbreak_flashon_wait on DX_MSG % dx_check
	fixbreak_flashon_wait:
		time 50
		to fixbreak on MET % timer_check1
		to fixbreak_flashoff
	fixbreak_flashoff:
      do dx_toggle1(0, 0, 0, 1000, NULLI, NULLI)
      to fixbreak_flashoff_wait on DX_MSG % dx_check
	fixbreak_flashoff_wait:
		time 50
      to fixbreak on MET % timer_check1
      to fixbreak_flashon

	fixbreak:
		do pr_score_trial(kBrFix, 0, 1)
		to fixbreak_wait
	fixbreak_wait:
		time 100
		to finish

	/* no choice */
	ncerr:
		do pr_score_trial(kNC, 0, 1) 	/* last parameter is "blank_flag" - blanks screen */
		to ncerr_wait
	ncerr_wait:
		time 100
		to finish

	/* error */

	/* error feedback for dots task */
	error_dots:	
		do err_feedback(FDBKONCD) 
		to error_dots_wait on DX_MSG % dx_check
	error_dots_wait:
		time 400
		to error

	error:
		do pr_score_trial(kError, 0, 1)
		to error_wait
	error_wait:
		time 1000
		to finish

	/* correct -- reward! */
	correct:
		time 10     /* possible wait time before reward onset */
		do pr_score_trial(kCorrect, 0, 1)
/*      to reward on 2 % pr_get_task_index */ /* fixation with dots   */
      to reward on 3 % pr_get_task_index /* dots reversal # 1    */
      to reward on 4 % pr_get_task_index /* dots reversal # 2    */
      to reward on 5 % pr_get_task_index /* dots reversal # 3    */
      to reward on 6 % pr_get_task_index /* dots reversal # 4    */		
      to rewardASL 
    reward:
		do pr_set_reward(2, 200, 50, -1, 0, 0) 
		to finish on 0 % pr_beep_reward

    rewardASL:
      do pr_set_reward(1, 200, 50, -1, 0, 0)
      to finish on 0 % pr_beep_reward

	/* check for extra time, then done */
	finish:
		do finish_trial(5)
		to xtime on 1 % check_xtime
		to loop
	xtime:
		time 5000
		to loop
}
