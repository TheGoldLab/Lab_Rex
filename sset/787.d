/* 787.d
** 2014/01/28 td created 787.d (adaptation to motion-direction chagne) from 786.d (motion detection)
**
** 2014/02/06 yl: can do offset angle that is other than 180
*/

/* 786.d
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
*/

/* 785yltd.d
**  
** 2013/06/24 td created from 785yl.d 
**  dotsj2 is used instead of dotsj (speed test and size test have been added)
**
*/

/*	785yl.d
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
static _PRrecord 	gl_rec = NULL; /* KA-HU-NA */

/* stim_toggle_ON/stim_bit_ON is for MT stim
** stim_toggle_ON2/stim_bit_ON2 is for LIP stim
*/
int stim_toggle_ON = 0;		/* if DIO stim_toggle_bit has been toggled */
int stim_bit_ON  = 0;			/* if DIO stim_on_bit has been toggled */
int stim_toggle_ON2 = 0;		/* if DIO stim_toggle_bit2 has been toggled */
int stim_bit_ON2 = 0;			/* etc. */

// long change_val = 0; /* second coherence on the t4 detection task */
int curr_angle;			/* current direction used in t4 adaptation task */
int delta_flag;			/* whether to use PREF or PREF+DELTA */

/* int dot_counter = 0;*/			/* # times dot pos has been updated in this trial */

	/* for now, allocate these here... */
MENU 	 	 umenus[100];		/* note: remember to expand if menu # gets bigger!) */
RTVAR		 rtvars[15];
USER_FUNC ufuncs[15];


#define TIMV(n)	pl_list_get_v(gl_rec->trialP->task->task_menus->lists[0],(n))

#define VSTIM_DUR 1000			/* duration of dots stim */
#define VSTIM_MAP_DUR 1500		/* duration of dots stim (MT mapping) */

#define WIND0 0		/* window to compare with eye signal */
#define WIND1 1		/* window to compare with eye signal */
#define WIND2 2		/* window to compare with eye signal */
#define WIND3 3		/* dummy window for convenience */

#define EYEH_SIG 	0
#define EYEV_SIG 	1

#define FPCOLOR_INI		5		/* color before fixation attained */
#define FPCOLOR_AFT		5		/* color after fixation attained */

#define FPDIAM_INI		5		/* diam after fixation attained, prev 3 */
#define FPDIAM_AFT		3		/* diam after fixation attained, prev 4*/


#define FPWINSIZE			50		/* fp window size */
#define FPWINSIZE_FIX		45		/* fp window size - post fixation */
#define TGWINSIZE			50		/* fp target size */

/** these are no longer used **/
#define MAX_COUNT			4		/* max # of times to update dot position */
#define FPCOLOR_ASL	   5		/* not used ... */
#define FPCOLOR_FT	   5 		/* color after fixation attained */
#define TARG1_COLOR		2		/* not used ... */
#define TARG2_COLOR		2		/* not used ... */
/** **/

/* ROUTINES */

/*
** INITIALIZATION routines
*/
/* ROUTINE: autoinit
**
**	Initialize gl_rec. This will automatically
**		set up the menus, etc.
ignal_num*
**  TASK0:		ASL calibration
**  TASK1:		passive fixation with dots (MT mapping)
**  TASK2:		fixation with target (MGS, LIP mapping)
**  TASK3:		fixed duration dot task (MT and LIP-stim)
**  TASK4:		motion-detection task (MT & LIP)
**  TASK5:		fixation w/ target (VGS)
*/
void autoinit(void)
{

printf("autoinit start\n");

	gl_rec = pr_initV(0, 0, 
		umenus, NULL,
		rtvars, pl_list_initV("rtvars", 0, 1,
				"angle", 0, 1.0, NULL),
		ufuncs, 
		"asl",		 1,		/* calibration */
		"dotsj2",	 1,   	/* MT mapping: passive fix dots */
		"ft",		 1,		/* LIP mapping: MGS */
		"dotsy",	 1,		/* MT- and LIP-stim: dots tasks */
		"dots_adpt", 1,	/* detection task */
		"ft",		 1,		/* VGS */
		NULL);

printf("autoinit end\n");
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
		wd_src_pos(WIND0, WD_DIRPOS, 0, WD_DIRPOS, 0);
		wd_src_check(WIND0, WD_SIGNAL, EYEH_SIG, WD_SIGNAL, EYEV_SIG);

		wd_src_pos(WIND1, WD_DIRPOS, 0, WD_DIRPOS, 0);
		wd_src_check(WIND1, WD_SIGNAL, EYEH_SIG, WD_SIGNAL, EYEV_SIG);

		wd_src_pos(WIND2, WD_DIRPOS, 0, WD_DIRPOS, 0);
		wd_src_check(WIND2, WD_SIGNAL, EYEH_SIG, WD_SIGNAL, EYEV_SIG);
		
		wd_src_pos(WIND3, WD_DIRPOS, 0, WD_DIRPOS, 0);
		wd_src_check(WIND3, WD_SIGNAL, EYEH_SIG, WD_SIGNAL, EYEV_SIG);

		/* init the screen */
		pr_setup();
	}
}

/* ROUTINE: start_trial
*/
int start_trial(void)
{

	int task_index = pr_get_task_index();
	int gdi[] = {0}, sflag;

	printf("start_trial start\n");
	/* No dynamic stimuli, so make draw_flag=3 the default.
	**	This draws each command ONCE
	*/
	dx_set_flags(DXF_D1);

	/* set dummy window */
	wd_siz(WIND1, 0, 0);
	wd_siz(WIND2, 0, 0);
	wd_siz(WIND3, 0, 0);
	
	/** print trial progress **/
	printf("trial # %d of %d\n", 
	       gl_rec->trialP->task->trialPs_index+1, 
	       gl_rec->trialP->task->trialPs_length);

	printf("start_trial end\n");
	return(0);
}


/* ROUTINE: finish_trial */
/* stuff that needs to be finished up */
/* added YL 2011/03/23 */
int finish_trial(void)
{
   int tid = pr_get_task_index();
	
	/* hide the dummy window */
	hide_dummy_wind();
	
	/* do other finish trial business */
	pr_finish_trial_yl();
	
	/* for tid 1,3,4: print the actual dot dir */
	if(tid == 1 || tid == 3 || tid == 4)
	printf("Last trial's dot_dir = %d\n",
	   PL_L2PW(gl_rec->trialP->task->task_menus->lists[2],3));
	
	return(0);
}

/* ROUTINE: clear_stim_flags */
/* turns off all the stim flags and bits - if any are ON
** 2011/05/18 yl */
int clear_stim_flags(void)
{
	return(0);
}


/* ROUTINE: do_toggle_stim_on_bit */
/* manipulates DIO "Stim_on_bit" - conditional on stim_toggle_ON
** toggle_flag: 0 = turn OFF send ELESTM ecode
** 				  1 = turn ON send ELEOFF ecode
**
** 2011/05/24 yl always sends estim code - even if already in that state
*/
int do_toggle_stim_on_bit(int toggle_flag)
{
	return(0);
}

/* ROUTINE: try_toggle_MT_stim */
/* mainpulates DIO "Stim_on_bit" - but only if this is an MT stim trial
** toggle_flag: same as for do_toggle_stim_on_bit
*/
int try_toggle_MT_stim(int toggle_flag)
{
	return(0);
}

/* ROUTINE: toggle_off_plexon */
/* turns off the plexon bit -- should be only called after PAUSE */
int toggle_off_plexon(void)
{
	PR_DIO_OFF("Plexon_toggle_bit");
}

/* ROUTINE: check_rew */
/* checks how reward should be given 
** correct ... 1 if choice was correct, 0 if choice was wrong
** returns: 1 if should reward, 0 else
*/
int check_rew(int correct)
{
	int tid = pr_get_task_index();
	int rew_p, result;
	
	if(tid == 3) {
		rew_p = TIMV("Rew_prob");
		if (rew_p == NULLI) { return(correct);}
		
		result = TOY_RCMP(rew_p);
/*		printf("rew_p = %d, REW = %d\n",rew_p,result);*/
		return(result);
	}
	else {
		return(correct);
	}
}

int check_rew0(void) {
/*	printf("check0\n");*/
	return(check_rew(0));
}

int check_rew1(void) {
/*	printf("check1\n");*/
	return(check_rew(1));
}

/* ROUTINE: show_error */
int show_error(long error_type)
{

	if(error_type == 0)
		printf("Bad task index (%d)\n", pr_get_task_index());

	return(0);
}


/* ROUTINE: do_calibration
**
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


/* ROUTINE: show_dummy_wind
** 
** plots in the window display where the subject's eyes are supposed to be
*/
int show_dummy_wind(int obj_i)
{
	dx_position_window(5, 5, obj_i, 0, WIND3);
	return(0);
}

int hide_dummy_wind(void)
{ 	wd_siz(WIND3, 0, 0);
}


int start_dotsj(void)
{
 ec_send_code_tagged(I_TESTIDCD, 7000 + TIMV("DR0/RF1/SZ2/SP3"));
 
 return(0);
}

/* ROUTINE: do_showT
**
** shows target .. according to Show_one/show_both menu
** 2013/11/23 yl
*/
int do_showT(void)
{
	int show_flag;
	show_flag = TIMV("Show_one(0)/both(1)");
	
	if(show_flag==1)
	{
		dx_toggle2(TARGONCD, 1, 1, 1000, 2, 1000);
	} else {
		dx_toggle2(TARGONCD, 1, 1, 1000, 2, 0);
	}
	
	return(0);
}


/* ROUTINE: do_set_change
** 
** sets change according to motion-detect trial condition & menu
** 2013/11/22 yl
int do_set_change(void)
{
	/* for now, just assume it's coherence change 
	dx_set_ecode(COHCHG);
	dx_set_by_name(DX_DOTS, DX_COH, 0, change_val);
	
	return(0);
} */

/* ROUTINE: do_switch_dir
** 
** switches motion direction between pref and null
** 2014/1/28
*/
int do_switch_dir(long ecode)
{
	long Angle_o = TIMV("Angle_o");
	long Angle_delta = TIMV("Angle_delta");
	
	// toggle the delta flag
	if(delta_flag == 1) { 
		delta_flag = 0; 
	} else { 
		delta_flag = 1; 
	}
	
	curr_angle = Angle_o+delta_flag*Angle_delta;
	
	printf("dir=%d\n", curr_angle);
	dx_set_ecode(ecode); // ecode for timing
	dx_set_by_name(DX_DOTS, DX_DIR, 0, curr_angle);
	return(0);
}



/* ROUTINE: do_t4init
** init process for detection task
** sets change_val according to motion-detect trial condition & menu
** 2013/12/10 td
*/

/* adaptation task */
/* check current trials' pref/null index and set motion direction
** prefnull is read out from the sign of coherence
** set direction based on a menu variable and trial-list coherence sign
** also, change of rate is determined here by reading out trials list 
** t4showd's preset time is changed accordingly.
** set curr_angle to pref or null depending on sign coh
*/
int do_t4init(void)
{
	long coh_angle = PL_L2PV(gl_rec->trialP->list,1);  /* signed coherence */
	long Angle_o   = TIMV("Angle_o"); // pref angle
	long Angle_delta = TIMV("Angle_delta"); // offset
	long rate_ind = get_index_rate(); // rate index
	
	if((coh_angle>=0 & rate_ind==0) | (coh_angle<0 & rate_ind>0)){
		delta_flag = 1;
	}else {
		delta_flag = 0;
	}
	
	curr_angle = Angle_o + delta_flag*Angle_delta;
	
	
	// the ecode for index_rate is sent in the task c file.
	
	printf("coh_angle=%d, Angle_o=%d, first dir=%d\n", coh_angle, Angle_o,curr_angle);
	dx_set_by_name(DX_DOTS, DX_DIR, 0, curr_angle);
	return(0);
}

/* get index for the rate of motion-dir change: 0,slow; 1,mid; 2,fast  */
int get_index_rate(void){
   int index_rate = PL_L2PV(gl_rec->trialP->list,0);
/*	printf("change-rate index = %d\n", index_rate); */
	return(index_rate);  /* get the index for direction-change rate from the trial list */
}


/* ROUTINE: do_show_fp0
**
** fp to cue fixation initially
**
** for tid==4, use menu
** otherwise, use FPCLUT_INI
**
** 2013/11/25 yl
*/
int do_show_fp0(void)
{
	int clut, tid;
	tid = pr_get_task_index();
	
	if(tid==4) {
		clut = TIMV("FP_CLUT0");
	} else {
		clut = FPCOLOR_INI;
	}
	
	dx_show_fp(FPONCD,0,FPDIAM_INI,FPDIAM_INI,clut,clut);
	return(0);
}

/*
** ROUTINE: do_show_fp1
**
** colors are from menu
** 
** 2013/11/23 yl
*/
int do_show_fp1(void)
{
	int clut;   /*% color from menu*/
	clut = TIMV("FP_CLUT0");
	
/*	printf("do_show_fp0: clut=%d\n", clut); */
	dx_show_fp(FIX1CD,0,FPDIAM_AFT,FPDIAM_AFT,clut,clut);
	return(0);
}  
int do_show_fp2(void)
{
	int clut;   /*% color from menu*/
	clut = TIMV("FP_CLUT1");
	
	printf("do_show_fp1: clut=%d\n", clut);
	dx_show_fp(FIX1CD,0,FPDIAM_AFT,FPDIAM_AFT,clut,clut);
	return(0);
}  
 

/* THE STATE SET 
*/
%%
id 787
restart rinitf
main_set {
status ON
begin	first:
		to prewait

   /*
   ** First wait time, which can be used to add
   ** a delay after reset states (e.g., fixation break)
   */
	prewait:
		do timer_set1(0,100,600,200,0,0)	/* this adds no time */
		to loop on +MET % timer_check1
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
		do timer_set1(0, 0, 0, 0, 2000, 0)		/* wait 2 sec */
		/* timer_set1(a, b, c, d, e,    f)
		** a = probability
		** b = min_time
		** c = max_time
		** d = mean_time
		** e = override_time
		** f = override_random
		*/
		to pause on +PSTOP & softswitch
		to go on +MET % timer_check1			/* PAUSE when the 'Paradigm Stopped' button on REX is pressed */
	pause:
		do clear_stim_flags()
		to toggle_off_plexon
	toggle_off_plexon:						/* 2011/07/12 added w/ pr_finish_trial_yl */
		do toggle_off_plexon()
		to go on -PSTOP & softswitch
	go:
		do pr_toggle_file(1)
		to trstart_trial
	trstart_trial:
		to trstart on MET % pr_start_trial
		to loop
	trstart:
		do start_trial()
		to fpshowdummy
	
	fpshowdummy:
		do show_dummy_wind(0)
		to fpshow
		
	fpshow:
	do do_show_fp0()
/*	do dx_show_fp(FPONCD,0,FPDIAM_INI,FPDIAM_INI,FPCOLOR_INI,FPCOLOR_INI)*/ 	/* was: 0, 5, 8, 1, 8 */
		to caljmp  on DX_MSG % dx_check
		
	caljmp:
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
			to correctASL
	

	/* Position window and wait for fixation */
	fpwinpos:
		time 20  /* takes time to settle window */
/*		do dx_position_window(25, 25, 0, 0, 0) */
 		do dx_position_window(FPWINSIZE, FPWINSIZE, 0, 0, 0)
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
		do dx_position_window(FPWINSIZE_FIX, FPWINSIZE_FIX, 0, 0, 0)	/* does not center window on eye pos */
/*		do dx_position_window(30, 30, 0, 0, 0) */
		to taskjmp

	/* Jump to task-specific statelist*/
	taskjmp:
		to t0fp on 0 % pr_get_task_index
		to t1fp on 1 % pr_get_task_index
		to t2fp on 2 % pr_get_task_index	
		to t3fp on 3 % pr_get_task_index	
		to t4fp on 4 % pr_get_task_index
		to t5fp on 5 % pr_get_task_index
		to badtask
	badtask:
		do show_error(0)
		to finish


	/* TASK 0: ASL eye tracker caliberation  */
	t0fp:		/* enters here only if cal0/val1 == 1 */
		do dx_show_fp(FIX1CD,0,FPDIAM_AFT,FPDIAM_AFT,FPCOLOR_AFT,FPCOLOR_AFT)
/*		do dx_toggle2(FPCHG, 1, 0, 1000, NULLI, NULLI) */
/*		do dx_show_fp(FPCHG, 0, 5, 5, FPCOLOR_ASL, FPCOLOR_ASL); */
		to t0wait1 on DX_MSG % dx_check
	t0wait1:
		do timer_set1(0, 100, 600, 200, 500, 0)	/* must stay in window! */
		to fixbreak on +WD0_XY & eyeflag		/* added 2011/07/12 */
 		to t0winpos on MET % timer_check1
	t0winpos:
		time 20
		do dx_position_window(20, 20,-1,0,0)
 		to correctASL
	
	
			
/* TASK 1: passive fixation with dots  */
	
	/* First wait period before showing the targets */
	t1fp:
		do dx_show_fp(FIX1CD,0,FPDIAM_AFT,FPDIAM_AFT,FPCOLOR_AFT,FPCOLOR_AFT)
		to t1start on DX_MSG % dx_check
   t1start:
      do start_dotsj()
      to t1wait1
	t1wait1:
		do timer_set1(1000, 100, 300, 200, 500, 0) /* exp t, 100-300, mean 200 */
		to fixbreak on +WD0_XY & eyeflag
 		to t1showd on MET % timer_check1
	/* Show the dots & turn them off*/
	t1showd:
		do dx_toggle2(GORANDCD, 1, 0, 0, 3, 1000)
		to fixbreak on +WD0_XY & eyeflag
		to t1waitd on DX_MSG % dx_check
	t1waitd:
		do timer_set1(0,0,0,0,VSTIM_MAP_DUR,0)
		to fixbreak on +WD0_XY & eyeflag
		to t1stopd on MET % timer_check1
	t1stopd:
		do dx_toggle2(ENDCD, 0, 0, 0, 3, 1000)
      to fixbreak on +WD0_XY & eyeflag
		to t1wait2 on DX_MSG % dx_check
	t1wait2:
		do timer_set1(0,0,0,0,500,0)
		to t1fpoff on MET % timer_check1
	t1fpoff:
		do dx_toggle2(FPOFFCD, 0, 0, 1000, 0, 0)
		to correct on DX_MSG % dx_check
	
	
   /* TASK 2: Memory Guided saccades */
	t2fp:
		do dx_show_fp(FIX1CD,0,FPDIAM_AFT,FPDIAM_AFT,FPCOLOR_AFT,FPCOLOR_AFT)
		to t2wait1 on DX_MSG % dx_check

	/* First wait period before showing the targets */
	t2wait1:
		do timer_set1(1000, 100, 300, 200, 500, 0)  /* old */
		to fixbreak on +WD0_XY & eyeflag
 		to t2show1 on MET % timer_check1
	t2show1:
		do dx_toggle2(TARGONCD, 1, 1, 1000, NULLI, NULLI)
		to t2winpos1 on DX_MSG % dx_check
	
	t2winpos1:
		do dx_position_window(TGWINSIZE, TGWINSIZE, 1, 0, 1) /*rex needs 20 ms to settle the window */
		time 20
	    to t2wait2
	
	/* Second wait period before turning off targets */
	t2wait2:
		do timer_set1(0,0,0,0,300,0)
	/*	do timer_set1(1000, 300, 800, 500, 500, 0)*/
		to fixbreak on +WD0_XY & eyeflag
 		to t2show2 on MET % timer_check1
	t2show2:
		do dx_toggle2(TARGOFFCD, 0, 1, 1000, NULLI, NULLI)
		to t2wait3 on DX_MSG % dx_check
	
	/* Third wait period before turning off fp */
	t2wait3:
		do timer_set1(1000, 500, 1000, 800, 500, 0)	/* 2011/07/08 */
	/*	do timer_set1(0, 300, 800, 500, 1000, 0) */
	/*	do timer_set1(1000, 300, 800, 500, 500, 0) */
		to fixbreak on +WD0_XY & eyeflag
 		to t2show3 on MET % timer_check1
	t2show3:
		do dx_toggle2(FPOFFCD, 0, 0, 1000, NULLI, NULLI)
		to grace on DX_MSG % dx_check

	
			
/* TASK 3: FD dots  */
	
	/* First wait period before showing the targets */
	t3fp:
		do dx_show_fp(FIX1CD,0,FPDIAM_AFT,FPDIAM_AFT,FPCOLOR_AFT,FPCOLOR_AFT)
	/*	do dx_toggle2(FIX1CD, 1, 0, 1000, NULLI, NULLI)  */
	/*	do dx_show_fp(FIX1CD, 0, 4, 0, FPCOLOR_FT,0) */
		to t3wait1 on DX_MSG % dx_check
	t3wait1:
		do timer_set1(1000, 100, 300, 200, 500, 0)
		to t3show1
	t3show1:
		do dx_toggle2(TARGONCD, 1, 1, 1000, 2, 1000)
		to t3winpos1 on DX_MSG % dx_check
	t3winpos1:
		do dx_position_window(TGWINSIZE, TGWINSIZE, 1, 0, 1)
	/*	do dx_position_window(50, 50, 1, 0, 1) */ /*rex needs 20 ms to settle the window */
		time 100
	    to t3winpos2
	t3winpos2:	
		do dx_position_window(TGWINSIZE, TGWINSIZE, 2, 0, 2)
/*		do dx_position_window(50, 50, 2, 0, 2) */
		time 100
	    to t3wait2
	
	/* Second wait period before showing the dots */
	t3wait2:
		do timer_set1(1000, 300, 800, 500, 500, 0)
		to fixbreak on +WD0_XY & eyeflag
 		to t3showd on MET % timer_check1
	
	/* Show the dots & turn them off*/
	t3showd:
		do dx_toggle2(GORANDCD, 1, 0, 0, 3, 1000)
		to t3trystim_on on DX_MSG % dx_check
	t3trystim_on:
		do try_toggle_MT_stim(1)
		to t3waitd
	t3waitd:
		do timer_set1(0,0,0,0,VSTIM_DUR,0)
		to fixbreak on +WD0_XY & eyeflag
		to t3stopd on MET % timer_check1
	t3stopd:
		do dx_toggle2(ENDCD, 0, 0, 0, 3, 1000)
		to fixbreak on +WD0_XY & eyeflag
		to t3trystim_off on DX_MSG % dx_check
	t3trystim_off:
		do try_toggle_MT_stim(0)
		to t3wait3

		
	/* Turn off FP*/
	t3wait3:
		do timer_set1(0,0,0,0,500,500)
		to fixbreak on +WD0_XY & eyeflag
		to t3fpoff on MET % timer_check1

	t3fpoff:
		do dx_toggle2(FPOFFCD, 0, 0, 1000, 0, 0)
		to grace on DX_MSG % dx_check


	/* TASK 4: motion-change adaptation task */
	t4fp:
		do do_show_fp1()
/*		do dx_show_fp(FIX1CD,0,FPDIAM_AFT,FPDIAM_AFT,FPCOLOR_AFT,FPCOLOR_AFT)*/
		to t4init on DX_MSG % dx_check
	t4init:
		do do_t4init()
		to t4wait1 on DX_MSG % dx_check
	t4wait1:
	   do timer_set1(1000, 300, 800, 407, 500, 0) /* 300-800, 407 --> sample mean: 500 */
		to fixbreak on +WD0_XY & eyeflag
 		to t4jmp on MET % timer_check1
	t4jmp:
		to t4showd00 on 0 % get_index_rate
		to t4showd10 on 1 % get_index_rate
		to t4showd20 on 2 % get_index_rate
	/* 1 state in the first half of Dots */ 
	t4showd00:
		do dx_toggle2(GORANDCD, 1, 0, 0, 3, 1000)
		to t4showd00wait on DX_MSG % dx_check
	t4showd00wait:
		time 1000
      to fixbreak on +WD0_XY & eyeflag
		to t4showdsec1

	/* 2 states in the first half of Dots */ 
	t4showd10:
		do dx_toggle2(GORANDCD, 1, 0, 0, 3, 1000)
		to t4showd10wait on DX_MSG % dx_check
	t4showd10wait:
		time 500 
		to fixbreak on +WD0_XY & eyeflag
		to t4showd11
	t4showd11:
		do do_switch_dir(DIRCHG0)
		to t4showd11wait on DX_MSG % dx_check
	t4showd11wait:
		time 500 
		to fixbreak on +WD0_XY & eyeflag
		to t4showdsec1
	
	/* 4 states in the first half of Dots */ 
	t4showd20:
		do dx_toggle2(GORANDCD, 1, 0, 0, 3, 1000)
		to t4showd20wait on DX_MSG % dx_check
	t4showd20wait:
		time 250 
		to fixbreak on +WD0_XY & eyeflag
		to t4showd21
	t4showd21:
		do do_switch_dir(DIRCHG0)
		to t4showd21wait on DX_MSG % dx_check
	t4showd21wait:
		time 250 
		to fixbreak on +WD0_XY & eyeflag
		to t4showd22
	t4showd22:
		do do_switch_dir(DIRCHG1)
		to t4showd22wait on DX_MSG % dx_check
	t4showd22wait:
		time 250 
		to fixbreak on +WD0_XY & eyeflag
		to t4showd23
	t4showd23:
		do do_switch_dir(DIRCHG2)
		to t4showd23wait on DX_MSG % dx_check
	t4showd23wait:
		time 250 
		to fixbreak on +WD0_XY & eyeflag
		to t4showdsec1
	
	/* the second half of Dots */
	t4showdsec1:
		do do_switch_dir(DIRCHG3)
		to t4showdsec1wait on DX_MSG % dx_check
	t4showdsec1wait:
		time 1000
		to fixbreak on +WD0_XY & eyeflag
	   to t4stopd
	t4stopd:
		do dx_toggle2(ENDCD, 0, 0, 0, 3, 1000)
		to t4wait2 on DX_MSG % dx_check
	t4wait2:
		do timer_set1(0,0,0,0,500,0)
		to fixbreak on +WD0_XY & eyeflag
		to t4fpoff on MET % timer_check1
	t4fpoff:
		do dx_toggle2(FPOFFCD, 0, 0, 1000, 0, 0)
		to correct on DX_MSG % dx_check
	
/* TASK 5: Visually Guided saccades */
	t5fp:
		do dx_show_fp(FIX1CD,0,FPDIAM_AFT,FPDIAM_AFT,FPCOLOR_AFT,FPCOLOR_AFT)
		to t5wait1  on DX_MSG % dx_check

	/* First wait period before showing the target */
	t5wait1:
		do timer_set1(1000, 500, 1000, 800, 0, 0) /* exp(800) */
		to fixbreak on +WD0_XY & eyeflag
 		to t5show1 on MET % timer_check1
	t5show1:
		do dx_toggle2(TARGONCD, 1, 1, 1000, NULLI, NULLI)
		to t5winpos1 on DX_MSG % dx_check
	t5winpos1:
		do dx_position_window(TGWINSIZE, TGWINSIZE, 1, 0, 1) /*rex needs 20 ms to settle the window */
		time 20
	    to t5show3
	
	/* Turn off FP - no wait */
	t5show3:
		do dx_toggle2(FPOFFCD, 0, 0, 1000, NULLI, NULLI)
		to grace on DX_MSG % dx_check		
	
	/* after go signal for non-RT tasks */
	grace:
		do show_dummy_wind(1)				/* should saccade to targ1 */
		to gracedummy
	gracedummy:							/* should leave fp in 300 ms */
		time 1000		/* was 300 */ 
		to sacmad on +WD0_XY & eyeflag
		to ncerr
	sacmad:									/* should reach T1 in 100 ms*/
		do ec_send_code(SACMAD)
		time 100
		to thold on -WD1_XY & eyeflag
		to nwinpos
   nwinpos:
      time 100
      to nhold on -WD2_XY & eyeflag
      to ncerr

   /* directly come to thold/nhold in RT tasks  */
	thold:									/* should stay in T1 400 ms*/
		do ec_send_code(TRGACQUIRECD)
		time 400
		to ncerr on +WD1_XY & eyeflag
     	to correct
	nhold:
		do ec_send_code(TRGACQUIRECD)	/* must stay in T2 400 ms for Error*/
		time 400
		to ncerr on +WD2_XY & eyeflag	/* otherwise, a no choice trial */
     	to error
	
	/* OUTCOME STATES
	** FIXBREAK
	** NCERR (No-Choice Error)
	**	ERROR
	** CORRECT
	*/

	/* fixation break */
	fixbreak:
		do try_toggle_MT_stim(0)			/* try to turn OFF MT stim */
		to fixbreak_cont
	fixbreak_detect:
		do dx_toggle2(SACMAD, 0, 0, 1000, 3, 1000) /* TD 2013-12-10, not sure if args are correct... */
		to fixbreak_cont on DX_MSG % dx_check
	fixbreak_cont:
		time 2500
		do pr_score_trial(kBrFix, 0, 1)
		to finish
	
	/* no choice */
	ncerr:		
/*		do dx_toggle2(TARG2OFF, 0, 1,0, 2, 1000)	*/
		do dx_toggle2(TARGOFFCD, 0, 1, 1000, 2, 1000) 
		to ncerr_score on DX_MSG % dx_check
	
	ncerr_score:
		do pr_score_trial(kNC, 0, 1) 	/* last parameter is "blank_flag" - blanks screen */
		to ncerr_wait
	
	ncerr_wait:
		time 1000
		to finish
	
	/* error */
	error:			
		do dx_toggle2(TARGOFFCD,0,1,1000,2,1000)	/* turn off BOTH targets */
	/*	do dx_toggle2(TARG2OFF, 0, 1,0, 2, 1000)*/ 	/* turn OFF T2 first as feedback - 2011/07/12 */	
		to error_score on DX_MSG % dx_check
/*		to correct_score on DX_MSG % dx_check */
	
	
	error_score:
		do pr_score_trial(kError, 0, 1)
		to correct_wait on 1 % check_rew0   /* reward on this trial? */
		to error_wait 

	error_wait:
		time 1000
/*		do print_error_wait()*/
		to finish

	/* correct -- reward! */
	correct:	
		do dx_toggle2(TARGOFFCD, 0, 1, 1000, 2, 1000) /* turn off BOTH targets*/
	/*	do dx_toggle2(TARG2OFF, 0, 1,0, 2, 1000)*/		/* turn OFF T2 first */
		to correct_score on DX_MSG % dx_check
	
	
	correct_score:
	   do pr_score_trial(kCorrect, 0, 1)
	   to correct_wait on 1 % check_rew1   /* reward on this trial? */
	   to error_wait
	   
	

	correct_wait:
		time 200
		to reward
		
	correctASL:
		do pr_score_trial(kCorrect, 0, 1)
		to rewardASL

	reward:
		do pr_set_reward(1, 200, 50, -1, 0, 0)		/* 2011-07-11 yl: change duration 50->200 */
/*	   do pr_set_reward(-3, 200, 300, -1, 200, 300) */ /* old */
	   to finish on 0 % pr_beep_reward
	   
	rewardASL:
	  do pr_set_reward(1, 100, 50, -1, 50, 50)
	  to finish on 0 % pr_beep_reward   

	finish:
		do finish_trial()
		to loop
}

#/** PhEDIT attribute block
#-11:16777215
#0:18794:default:-3:-3:0
#18794:18798:monospace10:-3:-3:0
#18798:19739:default:-3:-3:0
#19739:19743:monospace10:0:-1:0
#19743:26533:default:-3:-3:0
#**  PhEDIT attribute block ends (-0000237)**/
