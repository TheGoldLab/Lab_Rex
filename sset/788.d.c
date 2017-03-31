#include <stdio.h>
#include <sys/types.h>
#include "../hdr/rexHdr.h"




extern int softswitch;
extern int pr_toggle_file();
extern int softswitch;
extern int pr_toggle_file();
extern int pr_start_trial();
extern int start_trial();
extern int dx_show_fp();
extern int dx_check();
extern int do_calibration();
extern int dio_check_joybut();
extern int ec_send_code();
extern int dx_position_window();
extern int eyeflag;
extern int dx_change_fp();
extern int pr_score_trial();
extern int ec_send_code();
extern int eyeflag;
extern int dx_position_window();
extern int pr_get_task_index();
extern int pr_get_task_index();
extern int pr_get_task_index();
extern int pr_get_task_index();
extern int pr_get_task_index();
extern int pr_get_task_index();
extern int timer_set1();
extern int timer_check1();
extern int eyeflag;
extern int timer_set1();
extern int timer_check1();
extern int eyeflag;
extern int dx_toggle1();
extern int dx_check();
extern int timer_set1();
extern int timer_check1();
extern int eyeflag;
extern int dx_toggle1();
extern int dx_check();
extern int timer_set1();
extern int timer_check1();
extern int eyeflag;
extern int dx_hide_fp();
extern int dx_check();
extern int check_fixflag();
extern int dx_position_window();
extern int timer_set1();
extern int timer_check1();
extern int eyeflag;
extern int dx_toggle1();
extern int dx_check();
extern int timer_set1();
extern int timer_check1();
extern int eyeflag;
extern int dx_toggle1();
extern int dx_check();
extern int timer_set1();
extern int timer_check1();
extern int eyeflag;
extern int dx_hide_fp();
extern int dx_check();
extern int setup_dots();
extern int timer_set1();
extern int timer_check1();
extern int eyeflag;
extern int dx_toggle2();
extern int dx_check();
extern int timer_set1();
extern int timer_check1();
extern int eyeflag;
extern int dx_toggle1();
extern int dx_check();
extern int set_dots_timers();
extern int check_hide_dots();
extern int check_switch_dots();
extern int eyeflag;
extern int switch_dots();
extern int dx_check();
extern int end_dots();
extern int dx_check();
extern int timer_set1();
extern int timer_check1();
extern int eyeflag;
extern int dx_hide_fp();
extern int dx_check();
extern int setup_dots();
extern int timer_set1();
extern int timer_check1();
extern int eyeflag;
extern int dx_toggle2();
extern int dx_check();
extern int timer_set1();
extern int timer_check1();
extern int eyeflag;
extern int dx_toggle1();
extern int dx_check();
extern int set_dots_timers();
extern int check_hide_dots();
extern int check_switch_dots();
extern int eyeflag;
extern int switch_dots();
extern int dx_check();
extern int end_dots();
extern int dx_check();
extern int timer_set1();
extern int timer_check1();
extern int eyeflag;
extern int dx_hide_fp();
extern int dx_check();
extern int setup_dots();
extern int timer_set1();
extern int timer_check1();
extern int eyeflag;
extern int dx_toggle2();
extern int dx_check();
extern int timer_set1();
extern int timer_check1();
extern int eyeflag;
extern int dx_toggle1();
extern int dx_check();
extern int set_dots_timers();
extern int check_hide_dots();
extern int check_switch_dots();
extern int eyeflag;
extern int switch_dots();
extern int dx_check();
extern int end_dots();
extern int dx_check();
extern int timer_set1();
extern int timer_check1();
extern int eyeflag;
extern int dx_hide_fp();
extern int dx_check();
extern int eyeflag;
extern int ec_send_code();
extern int eyeflag;
extern int eyeflag;
extern int ec_send_code();
extern int eyeflag;
extern int ec_send_code();
extern int eyeflag;
extern int timer_set1();
extern int dx_toggle1();
extern int dx_check();
extern int timer_check1();
extern int dx_toggle1();
extern int dx_check();
extern int timer_check1();
extern int pr_score_trial();
extern int pr_score_trial();
extern int err_feedback();
extern int dx_check();
extern int pr_score_trial();
extern int pr_score_trial();
extern int pr_get_task_index();
extern int pr_get_task_index();
extern int pr_get_task_index();
extern int pr_set_reward();
extern int pr_beep_reward();
extern int pr_set_reward();
extern int pr_beep_reward();
extern int finish_trial();
extern int check_xtime();



STATE sfirst;
STATE sloop;
STATE spause;
STATE sgo;
STATE strstart;
STATE sshowfp;
STATE scaljmp;
STATE scalstart;
STATE scalacc;
STATE sfpwinpos;
STATE sfpwait;
STATE sfpchange;
STATE sfpnofix;
STATE sfpset;
STATE sfpwin2;
STATE staskjmp;
STATE st0start;
STATE st1start;
STATE st1showt;
STATE st1wait2;
STATE st1hidet;
STATE st1wait3;
STATE st1hidefp;
STATE st1endcheck;
STATE st1win;
STATE st2start;
STATE st2showd;
STATE st2waitd;
STATE st2hided;
STATE st2wait2;
STATE st2hidefp;
STATE st3start;
STATE st3wait1;
STATE st3showt;
STATE st3wait2;
STATE st3showd;
STATE st3checkd;
STATE st3switchd;
STATE st3stopd;
STATE st3wait3;
STATE st3hidefp;
STATE st4start;
STATE st4wait1;
STATE st4showt;
STATE st4wait2;
STATE st4showd;
STATE st4checkd;
STATE st4switchd;
STATE st4stopd;
STATE st4wait3;
STATE st4hidefp;
STATE st5start;
STATE st5wait1;
STATE st5showt;
STATE st5wait2;
STATE st5showd;
STATE st5checkd;
STATE st5switchd;
STATE st5stopd;
STATE st5wait3;
STATE st5hidefp;
STATE sgrace;
STATE ssacmad;
STATE spref;
STATE snull;
STATE sfixbreak_dots;
STATE sfixbreak_timeout;
STATE sfixbreak_flashon;
STATE sfixbreak_flashon_wait;
STATE sfixbreak_flashoff;
STATE sfixbreak_flashoff_wait;
STATE sfixbreak;
STATE sfixbreak_wait;
STATE sncerr;
STATE sncerr_wait;
STATE serror_dots;
STATE serror_dots_wait;
STATE serror;
STATE serror_wait;
STATE scorrect;
STATE sreward;
STATE srewardASL;
STATE sfinish;
STATE sxtime;



STATE *snames[] = {
&sfirst,
&sloop,
&spause,
&sgo,
&strstart,
&sshowfp,
&scaljmp,
&scalstart,
&scalacc,
&sfpwinpos,
&sfpwait,
&sfpchange,
&sfpnofix,
&sfpset,
&sfpwin2,
&staskjmp,
&st0start,
&st1start,
&st1showt,
&st1wait2,
&st1hidet,
&st1wait3,
&st1hidefp,
&st1endcheck,
&st1win,
&st2start,
&st2showd,
&st2waitd,
&st2hided,
&st2wait2,
&st2hidefp,
&st3start,
&st3wait1,
&st3showt,
&st3wait2,
&st3showd,
&st3checkd,
&st3switchd,
&st3stopd,
&st3wait3,
&st3hidefp,
&st4start,
&st4wait1,
&st4showt,
&st4wait2,
&st4showd,
&st4checkd,
&st4switchd,
&st4stopd,
&st4wait3,
&st4hidefp,
&st5start,
&st5wait1,
&st5showt,
&st5wait2,
&st5showd,
&st5checkd,
&st5switchd,
&st5stopd,
&st5wait3,
&st5hidefp,
&sgrace,
&ssacmad,
&spref,
&snull,
&sfixbreak_dots,
&sfixbreak_timeout,
&sfixbreak_flashon,
&sfixbreak_flashon_wait,
&sfixbreak_flashoff,
&sfixbreak_flashoff_wait,
&sfixbreak,
&sfixbreak_wait,
&sncerr,
&sncerr_wait,
&serror_dots,
&serror_dots_wait,
&serror,
&serror_wait,
&scorrect,
&sreward,
&srewardASL,
&sfinish,
&sxtime,
0};

/* 788.d
**
** Created 09/15/2015 by jig from 787b.d
**
*/

/* HISTORY:
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
*/
void autoinit(void)
{

	gl_rec = pr_initV(0, 0, umenus, NULL, rtvars,
		pl_list_initV("rtvars", 0, 1, "angle", 0, 1.0, NULL),
		ufuncs, 
		"asl",         1,  /* calibration */
		"ft",          1,  /* VGS */
		"dotsj2",      1,  /* Dots during passive fixation for MT mapping */
		"dots_rev",		3,  /* dots-reversal task */
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
	static int last_dots_set = 1;

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

	// update gl_dots_set
	gl_dots_set = gl_dots_set==1 ? 2 : 1;

	// Get current values of direction, coherence
	if(gl_dots_epoch<20) {
		if(gl_dots_set==1) {
			dir = TIMW("Epoch1_Dir1");
			coh = TIMW("Epoch1_Coh1");
		} else {
			dir = TIMW("Epoch1_Dir2");
			coh = TIMW("Epoch1_Coh2");
		}
	} else {
		if(gl_dots_set==1) {
			dir = TIMW("Epoch2_Dir1");
			coh = TIMW("Epoch2_Coh1");
		} else {
			dir = TIMW("Epoch2_Dir2");
			coh = TIMW("Epoch2_Coh2");
		}
	}

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
STATE sfirst = {
	{"first"},0,0
	,0,0
	,{0,0,0,0,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&sloop,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE sloop = {
	{"loop"},0,0
	,0,0
	,{0,0,0,0,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&spause,BITON,&softswitch,PSTOP },
	{0,0,&sgo,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE spause = {
	{"pause"},0,0
	,0,0
	,{pr_toggle_file,0,0,0,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&sgo,BITOFF,&softswitch,PSTOP },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE sgo = {
	{"go"},0,0
	,0,0
	,{pr_toggle_file,1,0,0,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&strstart,FUNC,&pr_start_trial,MET },
	{0,0,&sloop,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE strstart = {
	{"trstart"},0,0
	,0,0
	,{start_trial,0,0,0,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&sshowfp,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE sshowfp = {
	{"showfp"},0,0
	,0,0
	,{dx_show_fp,FPONCD,0,0,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&scaljmp,FUNC,&dx_check,DX_MSG },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE scaljmp = {
	{"caljmp"},0,0
	,0,0
	,{0,0,0,0,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&scalstart,FUNC,&do_calibration,1 },
	{0,0,&sfpwinpos,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE scalstart = {
	{"calstart"},0,0
	,0,0
	,{0,0,0,0,0,0,0,0,0,0,0}
	,5000,0
	,{
	{0,0,&scalacc,FUNC,&dio_check_joybut,0 },
	{0,0,&sncerr,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE scalacc = {
	{"calacc"},0,0
	,0,0
	,{ec_send_code,ACCEPTCAL,0,0,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&scorrect,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE sfpwinpos = {
	{"fpwinpos"},0,0
	,0,0
	,{dx_position_window,50, 50, 0, 0, WIND0,0,0,0,0,0}
	,20  ,0
	,{
	{0,0,&sfpwait,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE sfpwait = {
	{"fpwait"},0,0
	,0,0
	,{0,0,0,0,0,0,0,0,0,0,0}
	,5000,0
	,{
	{0,0,&sfpset,BITOFF,&eyeflag,WD0_XY },
	{0,0,&sfpchange,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE sfpchange = {
	{"fpchange"},0,0
	,0,0
	,{0,0,0,0,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&sfpwait,FUNC,&dx_change_fp,1 },
	{0,0,&sfpnofix,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE sfpnofix = {
	{"fpnofix"},0,0
	,0,0
	,{pr_score_trial,kNoFix,0,1,0,0,0,0,0,0,0}
	,2500,0
	,{
	{0,0,&sfinish,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE sfpset = {
	{"fpset"},0,0
	,0,0
	,{ec_send_code,EYINWD,0,0,0,0,0,0,0,0,0}
	,50 ,0
	,{
	{0,0,&sfpwait,BITON,&eyeflag,WD0_XY },
	{0,0,&sfpwin2,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE sfpwin2 = {
	{"fpwin2"},0,0
	,0,0
	,{dx_position_window,50, 50, -1, 1, 0,0,0,0,0,0}
	,20 ,0
	,{
	{0,0,&staskjmp,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE staskjmp = {
	{"taskjmp"},0,0
	,0,0
	,{0,0,0,0,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&st0start,FUNC,&pr_get_task_index,0 },
	{0,0,&st1start,FUNC,&pr_get_task_index,1 },
	{0,0,&st2start,FUNC,&pr_get_task_index,2 },
	{0,0,&st3start,FUNC,&pr_get_task_index,3 },
	{0,0,&st4start,FUNC,&pr_get_task_index,4 },
	{0,0,&st5start,FUNC,&pr_get_task_index,5 },
	{0,0,&sfinish,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st0start = {
	{"t0start"},0,0
	,0,0
	,{timer_set1,0, 100, 600, 200, 500, 0,0,0,0,0}
	,0,0
	,{
	{0,0,&scorrect,FUNC,&timer_check1,MET },
	{0,0,&sfixbreak,BITON,&eyeflag,WD0_XY },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st1start = {
	{"t1start"},0,0
	,0,0
	,{timer_set1,1000, 100, 300, 200, 500, 0,0,0,0,0}
	,0,0
	,{
	{0,0,&st1showt,FUNC,&timer_check1,MET },
	{0,0,&sfixbreak,BITON,&eyeflag,WD0_XY },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st1showt = {
	{"t1showt"},0,0
	,0,0
	,{dx_toggle1,TARGONCD, 1, 1, 1000, NULLI, NULLI,0,0,0,0}
	,0,0
	,{
	{0,0,&st1wait2,FUNC,&dx_check,DX_MSG },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st1wait2 = {
	{"t1wait2"},0,0
	,0,0
	,{timer_set1,0,0,0,0,300,0,0,0,0,0}
	,0,0
	,{
	{0,0,&st1hidet,FUNC,&timer_check1,MET },
	{0,0,&sfixbreak,BITON,&eyeflag,WD0_XY },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st1hidet = {
	{"t1hidet"},0,0
	,0,0
	,{dx_toggle1,TARGOFFCD, 0, 1, 1000, NULLI, NULLI,0,0,0,0}
	,0,0
	,{
	{0,0,&st1wait3,FUNC,&dx_check,DX_MSG },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st1wait3 = {
	{"t1wait3"},0,0
	,0,0
	,{timer_set1,1000, 500, 1000, 800, 500, 0,0,0,0,0}
	,0,0
	,{
	{0,0,&st1hidefp,FUNC,&timer_check1,MET },
	{0,0,&sfixbreak,BITON,&eyeflag,WD0_XY },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st1hidefp = {
	{"t1hidefp"},0,0
	,0,0
	,{dx_hide_fp,FPOFFCD,0,0,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&st1endcheck,FUNC,&dx_check,DX_MSG },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st1endcheck = {
	{"t1endcheck"},0,0
	,0,0
	,{0,0,0,0,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&scorrect,FUNC,&check_fixflag,MET },
	{0,0,&st1win,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st1win = {
	{"t1win"},0,0
	,0,0
	,{dx_position_window,50, 50, 1, 0, WIND1,0,0,0,0,0}
	,20,0
	,{
	{0,0,&sgrace,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st2start = {
	{"t2start"},0,0
	,0,0
	,{timer_set1,1000, 100, 300, 200, 500, 0,0,0,0,0}
	,0,0
	,{
	{0,0,&st2showd,FUNC,&timer_check1,MET },
	{0,0,&sfixbreak,BITON,&eyeflag,WD0_XY },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st2showd = {
	{"t2showd"},0,0
	,0,0
	,{dx_toggle1,GORANDCD, 1, 3, 1000, NULLI, NULLI,0,0,0,0}
	,0,0
	,{
	{0,0,&st2waitd,FUNC,&dx_check,DX_MSG },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st2waitd = {
	{"t2waitd"},0,0
	,0,0
	,{timer_set1,0,0,0,0,1000,0,0,0,0,0}
	,0,0
	,{
	{0,0,&st2hided,FUNC,&timer_check1,MET },
	{0,0,&sfixbreak,BITON,&eyeflag,WD0_XY },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st2hided = {
	{"t2hided"},0,0
	,0,0
	,{dx_toggle1,ENDCD, 0, 3, 1000, NULLI, NULLI,0,0,0,0}
	,0,0
	,{
	{0,0,&st2wait2,FUNC,&dx_check,DX_MSG },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st2wait2 = {
	{"t2wait2"},0,0
	,0,0
	,{timer_set1,0,0,0,0,500,0,0,0,0,0}
	,0,0
	,{
	{0,0,&st2hidefp,FUNC,&timer_check1,MET },
	{0,0,&sfixbreak,BITON,&eyeflag,WD0_XY },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st2hidefp = {
	{"t2hidefp"},0,0
	,0,0
	,{dx_hide_fp,FPOFFCD,0,0,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&scorrect,FUNC,&dx_check,DX_MSG },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st3start = {
	{"t3start"},0,0
	,0,0
	,{setup_dots,500,0,0,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&st3wait1,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st3wait1 = {
	{"t3wait1"},0,0
	,0,0
	,{timer_set1,1000, 100, 300, 200, 0, 0,0,0,0,0}
	,0,0
	,{
	{0,0,&st3showt,FUNC,&timer_check1,MET },
	{0,0,&sfixbreak,BITON,&eyeflag,WD0_XY },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st3showt = {
	{"t3showt"},0,0
	,0,0
	,{dx_toggle2,TARGONCD,1,1,1000,2,1000,0,0,0,0}
	,0,0
	,{
	{0,0,&st3wait2,FUNC,&dx_check,DX_MSG },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st3wait2 = {
	{"t3wait2"},0,0
	,0,0
	,{timer_set1,1000, 100, 300, 200, 0, 0,0,0,0,0}
	,0,0
	,{
	{0,0,&st3showd,FUNC,&timer_check1,MET },
	{0,0,&sfixbreak,BITON,&eyeflag,WD0_XY },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st3showd = {
	{"t3showd"},0,0
	,0,0
	,{dx_toggle1,GORANDCD, 1, 3, 1000, NULLI, NULLI,0,0,0,0}
	,0,0
	,{
	{0,0,&st3checkd,FUNC,&dx_check,DX_MSG },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st3checkd = {
	{"t3checkd"},0,0
	,0,0
	,{set_dots_timers,0,0,0,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&st3stopd,FUNC,&check_hide_dots,MET },
	{0,0,&st3switchd,FUNC,&check_switch_dots,MET },
	{0,0,&sfixbreak_dots,BITON,&eyeflag,WD0_XY },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st3switchd = {
	{"t3switchd"},0,0
	,0,0
	,{switch_dots,GORANDCD,0,0,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&st3checkd,FUNC,&dx_check,DX_MSG },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st3stopd = {
	{"t3stopd"},0,0
	,0,0
	,{end_dots,ENDCD,50,50,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&st3wait3,FUNC,&dx_check,DX_MSG },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st3wait3 = {
	{"t3wait3"},0,0
	,0,0
	,{timer_set1,0,0,0,0,500,0,0,0,0,0}
	,0,0
	,{
	{0,0,&st3hidefp,FUNC,&timer_check1,MET },
	{0,0,&sfixbreak_dots,BITON,&eyeflag,WD0_XY },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st3hidefp = {
	{"t3hidefp"},0,0
	,0,0
	,{dx_hide_fp,FPOFFCD,0,0,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&sgrace,FUNC,&dx_check,DX_MSG },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st4start = {
	{"t4start"},0,0
	,0,0
	,{setup_dots,500,0,0,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&st4wait1,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st4wait1 = {
	{"t4wait1"},0,0
	,0,0
	,{timer_set1,1000, 100, 300, 200, 0, 0,0,0,0,0}
	,0,0
	,{
	{0,0,&st4showt,FUNC,&timer_check1,MET },
	{0,0,&sfixbreak,BITON,&eyeflag,WD0_XY },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st4showt = {
	{"t4showt"},0,0
	,0,0
	,{dx_toggle2,TARGONCD,1,1,1000,2,1000,0,0,0,0}
	,0,0
	,{
	{0,0,&st4wait2,FUNC,&dx_check,DX_MSG },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st4wait2 = {
	{"t4wait2"},0,0
	,0,0
	,{timer_set1,1000, 100, 300, 200, 0, 0,0,0,0,0}
	,0,0
	,{
	{0,0,&st4showd,FUNC,&timer_check1,MET },
	{0,0,&sfixbreak,BITON,&eyeflag,WD0_XY },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st4showd = {
	{"t4showd"},0,0
	,0,0
	,{dx_toggle1,GORANDCD, 1, 3, 1000, NULLI, NULLI,0,0,0,0}
	,0,0
	,{
	{0,0,&st4checkd,FUNC,&dx_check,DX_MSG },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st4checkd = {
	{"t4checkd"},0,0
	,0,0
	,{set_dots_timers,0,0,0,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&st4stopd,FUNC,&check_hide_dots,MET },
	{0,0,&st4switchd,FUNC,&check_switch_dots,MET },
	{0,0,&sfixbreak_dots,BITON,&eyeflag,WD0_XY },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st4switchd = {
	{"t4switchd"},0,0
	,0,0
	,{switch_dots,GORANDCD,0,0,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&st4checkd,FUNC,&dx_check,DX_MSG },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st4stopd = {
	{"t4stopd"},0,0
	,0,0
	,{end_dots,ENDCD,50,50,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&st4wait3,FUNC,&dx_check,DX_MSG },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st4wait3 = {
	{"t4wait3"},0,0
	,0,0
	,{timer_set1,0,0,0,0,500,0,0,0,0,0}
	,0,0
	,{
	{0,0,&st4hidefp,FUNC,&timer_check1,MET },
	{0,0,&sfixbreak_dots,BITON,&eyeflag,WD0_XY },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st4hidefp = {
	{"t4hidefp"},0,0
	,0,0
	,{dx_hide_fp,FPOFFCD,0,0,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&sgrace,FUNC,&dx_check,DX_MSG },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st5start = {
	{"t5start"},0,0
	,0,0
	,{setup_dots,500,0,0,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&st5wait1,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st5wait1 = {
	{"t5wait1"},0,0
	,0,0
	,{timer_set1,1000, 100, 300, 200, 0, 0,0,0,0,0}
	,0,0
	,{
	{0,0,&st5showt,FUNC,&timer_check1,MET },
	{0,0,&sfixbreak,BITON,&eyeflag,WD0_XY },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st5showt = {
	{"t5showt"},0,0
	,0,0
	,{dx_toggle2,TARGONCD,1,1,1000,2,1000,0,0,0,0}
	,0,0
	,{
	{0,0,&st5wait2,FUNC,&dx_check,DX_MSG },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st5wait2 = {
	{"t5wait2"},0,0
	,0,0
	,{timer_set1,1000, 100, 300, 200, 0, 0,0,0,0,0}
	,0,0
	,{
	{0,0,&st5showd,FUNC,&timer_check1,MET },
	{0,0,&sfixbreak,BITON,&eyeflag,WD0_XY },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st5showd = {
	{"t5showd"},0,0
	,0,0
	,{dx_toggle1,GORANDCD, 1, 3, 1000, NULLI, NULLI,0,0,0,0}
	,0,0
	,{
	{0,0,&st5checkd,FUNC,&dx_check,DX_MSG },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st5checkd = {
	{"t5checkd"},0,0
	,0,0
	,{set_dots_timers,0,0,0,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&st5stopd,FUNC,&check_hide_dots,MET },
	{0,0,&st5switchd,FUNC,&check_switch_dots,MET },
	{0,0,&sfixbreak,BITON,&eyeflag,WD0_XY },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st5switchd = {
	{"t5switchd"},0,0
	,0,0
	,{switch_dots,GORANDCD,0,0,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&st5checkd,FUNC,&dx_check,DX_MSG },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st5stopd = {
	{"t5stopd"},0,0
	,0,0
	,{end_dots,ENDCD,50,50,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&st5wait3,FUNC,&dx_check,DX_MSG },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st5wait3 = {
	{"t5wait3"},0,0
	,0,0
	,{timer_set1,0,0,0,0,500,0,0,0,0,0}
	,0,0
	,{
	{0,0,&st5hidefp,FUNC,&timer_check1,MET },
	{0,0,&sfixbreak,BITON,&eyeflag,WD0_XY },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE st5hidefp = {
	{"t5hidefp"},0,0
	,0,0
	,{dx_hide_fp,FPOFFCD,0,0,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&sgrace,FUNC,&dx_check,DX_MSG },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE sgrace = {
	{"grace"},0,0
	,0,0
	,{0,0,0,0,0,0,0,0,0,0,0}
	,500    ,0
	,{
	{0,0,&ssacmad,BITON,&eyeflag,WD0_XY },
	{0,0,&sncerr,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE ssacmad = {
	{"sacmad"},0,0
	,0,0
	,{ec_send_code,SACMAD,0,0,0,0,0,0,0,0,0}
	,100,0
	,{
	{0,0,&spref,BITOFF,&eyeflag,WD1_XY },
	{0,0,&snull,BITOFF,&eyeflag,WD2_XY },
	{0,0,&sncerr,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE spref = {
	{"pref"},0,0
	,0,0
	,{ec_send_code,TRGACQUIRECD,0,0,0,0,0,0,0,0,0}
	,400,0
	,{
	{0,0,&sncerr,BITON,&eyeflag,WD1_XY },
	{0,0,&scorrect,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE snull = {
	{"null"},0,0
	,0,0
	,{ec_send_code,TRGACQUIRECD,0,0,0,0,0,0,0,0,0}
	,100,0
	,{
	{0,0,&sncerr,BITON,&eyeflag,WD2_XY },
	{0,0,&serror_dots,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE sfixbreak_dots = {
	{"fixbreak_dots"},0,0
	,0,0
	,{0,0,0,0,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&sfixbreak,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE sfixbreak_timeout = {
	{"fixbreak_timeout"},0,0
	,0,0
	,{timer_set1,0, 0, 0, 0, 1000, 0,0,0,0,0}
	,0,0
	,{
	{0,0,&sfixbreak_flashon,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE sfixbreak_flashon = {
	{"fixbreak_flashon"},0,0
	,0,0
	,{dx_toggle1,0, 1, 0, 1000, NULLI, NULLI,0,0,0,0}
	,0,0
	,{
	{0,0,&sfixbreak_flashon_wait,FUNC,&dx_check,DX_MSG },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE sfixbreak_flashon_wait = {
	{"fixbreak_flashon_wait"},0,0
	,0,0
	,{0,0,0,0,0,0,0,0,0,0,0}
	,50,0
	,{
	{0,0,&sfixbreak,FUNC,&timer_check1,MET },
	{0,0,&sfixbreak_flashoff,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE sfixbreak_flashoff = {
	{"fixbreak_flashoff"},0,0
	,0,0
	,{dx_toggle1,0, 0, 0, 1000, NULLI, NULLI,0,0,0,0}
	,0,0
	,{
	{0,0,&sfixbreak_flashoff_wait,FUNC,&dx_check,DX_MSG },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE sfixbreak_flashoff_wait = {
	{"fixbreak_flashoff_wait"},0,0
	,0,0
	,{0,0,0,0,0,0,0,0,0,0,0}
	,50,0
	,{
	{0,0,&sfixbreak,FUNC,&timer_check1,MET },
	{0,0,&sfixbreak_flashon,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE sfixbreak = {
	{"fixbreak"},0,0
	,0,0
	,{pr_score_trial,kBrFix, 0, 1,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&sfixbreak_wait,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE sfixbreak_wait = {
	{"fixbreak_wait"},0,0
	,0,0
	,{0,0,0,0,0,0,0,0,0,0,0}
	,100,0
	,{
	{0,0,&sfinish,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE sncerr = {
	{"ncerr"},0,0
	,0,0
	,{pr_score_trial,kNC, 0, 1,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&sncerr_wait,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE sncerr_wait = {
	{"ncerr_wait"},0,0
	,0,0
	,{0,0,0,0,0,0,0,0,0,0,0}
	,100,0
	,{
	{0,0,&sfinish,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE serror_dots = {
	{"error_dots"},0,0
	,0,0
	,{err_feedback,FDBKONCD,0,0,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&serror_dots_wait,FUNC,&dx_check,DX_MSG },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE serror_dots_wait = {
	{"error_dots_wait"},0,0
	,0,0
	,{0,0,0,0,0,0,0,0,0,0,0}
	,400,0
	,{
	{0,0,&serror,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE serror = {
	{"error"},0,0
	,0,0
	,{pr_score_trial,kError, 0, 1,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&serror_wait,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE serror_wait = {
	{"error_wait"},0,0
	,0,0
	,{0,0,0,0,0,0,0,0,0,0,0}
	,1000,0
	,{
	{0,0,&sfinish,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE scorrect = {
	{"correct"},0,0
	,0,0
	,{pr_score_trial,kCorrect, 0, 1,0,0,0,0,0,0,0}
	,10     ,0
	,{
	{0,0,&sreward,FUNC,&pr_get_task_index,3 },
	{0,0,&sreward,FUNC,&pr_get_task_index,4 },
	{0,0,&sreward,FUNC,&pr_get_task_index,5 },
	{0,0,&srewardASL,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE sreward = {
	{"reward"},0,0
	,0,0
	,{pr_set_reward,2, 200, 50, -1, 0, 0,0,0,0,0}
	,0,0
	,{
	{0,0,&sfinish,FUNC,&pr_beep_reward,0 },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE srewardASL = {
	{"rewardASL"},0,0
	,0,0
	,{pr_set_reward,1, 200, 50, -1, 0, 0,0,0,0,0}
	,0,0
	,{
	{0,0,&sfinish,FUNC,&pr_beep_reward,0 },
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE sfinish = {
	{"finish"},0,0
	,0,0
	,{finish_trial,5,0,0,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,0,&sxtime,FUNC,&check_xtime,1 },
	{0,0,&sloop,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};
STATE sxtime = {
	{"xtime"},0,0
	,0,0
	,{0,0,0,0,0,0,0,0,0,0,0}
	,5000,0
	,{
	{0,0,&sloop,TIME,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
	}
};



STATE *aborta[] = {
0};

int sf_init();

STATE sps_state;

AWAKE init_state[] = {
	{0,ON,ON,&sps_state,&sps_state,0,(STATE *)&init_state[1],0,
			&sps_state.escape}
	,{0,0,0,0,0,0,0,0,&sps_state.escape[1]}
};
STATE sps_state = {
	{"spec"},1,0
	,1,0
	,{sf_init,788,0,0,0,0,0,0,0,0,0}
	,0,0
	,{
	{0,init_state,&sps_state,TIME,0,0}
	,{0,0,0,0,0,0}
	}
};
AWAKE nowstate[] = {
	{0,ON,ON,&sfirst,&sfirst,aborta,0,0,0}
	,{0,0,0,0,0,0,0,0,0}
};

void rinitf();
int (*init_list[])() = {
rinitf,
0};
VLIST state_vl[] = {
NS, };
