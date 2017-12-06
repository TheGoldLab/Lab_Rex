/*	792.d
**
** Paradigm for the adaptive ODR task
**	
**	Written by jig & ks from 791
** 
*/

#include "rexHdr.h"
#include "paradigm_rec.h"
#include "task.h"
#include "toys.h"
#include "lcode.h"
#include "sac.h"

/* PRIVATE data structures */

/* GLOBAL VARIABLES */
static _PRrecord 	gl_rec = NULL; /* KA-HU-NA */
static int sactest;

	/* for now, allocate these here... */
MENU 	 	 umenus[30];
RTVAR		 rtvars[15];
USER_FUNC ufuncs[15];

#define TIMV(n)      pl_list_get_v(gl_rec->trialP->task->task_menus->lists[0],(n))
#define BIMV(n,i)    pl_list_get_vi(gl_rec->trialP->task->task_menus->lists[1], (n), (i))
#define PRV(n)       pl_list_get_v(gl_rec->prefs_menu, (n))

#define WIND0 0		/* window to compare with eye signal */
#define WIND1 1		/* window for targets */
#define WIND2 2		/* window for targets */
#define WIND3 3		/* window for targets */

#define EYEH_SIG 	0
#define EYEV_SIG 	1
#define COR  		0x01
#define ERR 		0x02
#define NC  		0

/* ROUTINES */

/*
** INITIALIZATION routines
*/
/* ROUTINE: autoinit
**
**	Initialize gl_rec. This will automatically
**		set up the menus, etc.
**
**  TASK0: ASL calibration
**  TASK1: probabilistic reward task (hi noise, lo haz)
**  TASK2: probabilistic reward task (lo noise, hi haz)
*/
void autoinit(void)
{

	/* set up the record */
	gl_rec = pr_initV(0, 0, 
		umenus, NULL,
		rtvars, NULL,
		ufuncs, 
		"asl",	1,		
		"ft",		1,
      "adODR",	2,
		NULL);

	/* seed the random number generator */
	TOY_SRAND;

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

		/* initialize interface (window) parameters for FOUR windows:
		** 	WD0 	.. fixation point
		**		WD1	.. relevant target generative mean
		**		WD2	.. relevant target most recent location
		**		WD3 	.. irrelevant target most recent location
		*/
		wd_src_check(WIND0, WD_SIGNAL, EYEH_SIG, WD_SIGNAL, EYEV_SIG);
		wd_src_pos  (WIND0, WD_DIRPOS, 0, WD_DIRPOS, 0);
		wd_cntrl    (WIND0, WD_ON);

		wd_src_check(WIND1, WD_SIGNAL, EYEH_SIG, WD_SIGNAL, EYEV_SIG);
		wd_src_pos  (WIND1, WD_DIRPOS, 0, WD_DIRPOS, 0);
		wd_cntrl    (WIND1, WD_ON);

		wd_src_check(WIND2, WD_SIGNAL, EYEH_SIG, WD_SIGNAL, EYEV_SIG);
		wd_src_pos  (WIND2, WD_DIRPOS, 0, WD_DIRPOS, 0);
		wd_cntrl    (WIND2, WD_ON);

		wd_src_check(WIND3, WD_SIGNAL, EYEH_SIG, WD_SIGNAL, EYEV_SIG);
		wd_src_pos  (WIND3, WD_DIRPOS, 0, WD_DIRPOS, 0);
		wd_cntrl    (WIND3, WD_ON);

		/* initialize saccade detector */
		/* sd_set(1); */

		/* init the screen */
		pr_setup();
	}
}

/* ROUTINE: start_trial
** will eventually put in stuff here for changing commonly changed parameters like
** nuwind and pref
*/

int start_trial(void)
{		
	int task_index = pr_get_task_index();
	
   /* No dynamic stimuli, so make draw_flag=3 the default.
	**	This draws each command ONCE
	*/
	dx_set_flags(DXF_D1);

	return(0);
}

/* ROUTINE: finish_trial
*/
int finish_trial(void)
{
	/* do other finish trial business */
	pr_finish_trial();
	
	return(0);
}

/* ROUTINE: fix_task
**
**		Returns 1 if a fixation-only task (FT task)
*/
int fix_task(void)
{
	return(pr_get_task_menu_value("setup", "Fixation_task", 0) == 1);
}

/* ROUTINE: position_target_window
**
** Check current trial for correct target index, then position
**		the eye window around it
**
*/
int position_target_windows(long width1, long height1, 
									 long width2, long height2, 
									 long width3, long height3)
{
	/* if ft task, just put first window over target location, and 
	** turn off other windows
	*/
	if(pr_get_task_index()==1) {
		dx_position_window(width1, height1, 1, 0, WIND1);
		dx_position_window(0, 		0, 		1, 0, WIND2);
		dx_position_window(0, 		0, 		1, 0, WIND3);

	/* Otherwise position:
	**	WIND1 over generative mean of correct target
	** WIND2 over actual location of correct target
	** WIND3 over actual location of inactive target
	*/
	} else {
		int index = pr_get_trial_property("target", 0) + 1;

		dx_position_window(width1, height1, index+2, 0, WIND1);
		dx_position_window(width2, height2, index,   0, WIND2);
		dx_position_window(width3, height3, 3-index, 0, WIND3);
	}

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
		return(1);
	} else {
		return(0);
	}
}

/* ROUTINE: reward_trial
**
*/
int reward_trial(long num_rewards, long reward_on_time, long reward_off_time)
{

	/* set the reward parameters */
	if(reward_on_time < 0)
		PRV("Reward_on_time");
	if(reward_off_time < 0)
		PRV("Reward_off_time");

	pr_set_reward(num_rewards, reward_on_time, reward_off_time, -1, 55, 100);

	/* score as correct trial */	
	pr_score_trial(kCorrect, 0, 1);

	return(0);
}

/* THE STATE SET 
*/
%%
id 792
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
		do timer_set1(0,100,600,200,0,0)
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
		do timer_set1(0,100,600,200,0,0)
		to pause on +PSTOP & softswitch
		to go on +MET % timer_check1
	pause:
		to go on -PSTOP & softswitch
	go:
		do pr_toggle_file(1)
		to trstart_trial
	trstart_trial:
		to trstart on MET % pr_start_trial
		to loop
	trstart:
		do start_trial()
		to fpshow
	
	/* Show fixation point */
	fpshow:
		do dx_show_fp(FPONCD, 0, 5, 5, 2, 2);
		to fpwinpos on DX_MSG % dx_check
	fpwinpos:
		time 20  /* takes time to settle window */
		do dx_position_window(20, 20, 0, 0, 0)
		to calstart on 1 % do_calibration
		to fpwait

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
		to correctfix

	/* Wait for fixation
	*/
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
		time 250 /* give gaze time to settle into place (fixation) */
		to fpwait on +WD0_XY & eyeflag
		to fpwin2
	fpwin2:
		time 20 /* again time to settle window */
		do dx_position_window(10, 10, 1, 1, 0)
		to taskjmp

	/* Jump to task-specific statelists
	*/
	taskjmp:
		to t0start on 0 % pr_get_task_index
		to t1start on 1 % pr_get_task_index
		to t2start on 2 % pr_get_task_index
		to t3start on 3 % pr_get_task_index
		to finish

	/* TASK 0: ASL eye tracker validation  
	** 	Enters here only if cal0/val1 = 1
	*/
	t0start:
		do timer_set1(1000, 100, 600, 200, 0, 0)
 		to correctfix on MET % timer_check1
		to fixbreak on +WD0_XY & eyeflag
	
	/* Task 1 is ft ... we want to (each condition is optional):
	**		change fp to standard diameter/color 
	**		wait1
	**		change f/t 1
	**		wait2
	**		change f/t 2
	**		wait3
	**		change f/t 3
	**		STOP CHECKING FOR FIX BREAKS AND 
	**			START CHECKING FOR SACCADES
	**		wait1
	**		change f/t 1
	**		wait2
	*/	
	t1start:
		do dx_show_fp(FPCHG, 0, 5, 5, 2, 2);
		to t1wait1 on DX_MSG % dx_check
		to fixbreak on +WD0_XY & eyeflag
	t1wait1:
		do timer_set1(1000, 100, 600, 200, 0, 0)
		to fixbreak on +WD0_XY & eyeflag
 		to t1ftc1 on MET % timer_check1
	t1ftc1:
		do dx_toggle2(TRGC1CD, 0, 0, 1000, NULLI, NULLI);
		to fixbreak on +WD0_XY & eyeflag
		to t1wait2 on DX_MSG % dx_check
	t1wait2:
		do timer_set1(1000, 100, 600, 200, 0, 0)
		to fixbreak on +WD0_XY & eyeflag
 		to t1ftc2 on MET % timer_check1
	t1ftc2:
		do dx_toggle2(TRGC2CD, 0, 0, 1000, NULLI, NULLI);
		to fixbreak on +WD0_XY & eyeflag
		to t1wait3 on DX_MSG % dx_check
	t1wait3:
		do timer_set1(1000, 100, 600, 200, 0, 0)
		to fixbreak on +WD0_XY & eyeflag
 		to t1ftc3 on MET % timer_check1
	t1ftc3:
		do dx_toggle2(FPOFFCD, 0, 0, 1000, NULLI, NULLI);
		to endtrial on DX_MSG % dx_check

	/* Task 2 is adaptiveODR ... we want to (each condition is optional):
	**		change fp to standard diameter/color 
	**		wait 1
	**		change targets 1
	**		wait 2
	**		change targets 2
	**		wait 3
	**		hide fp
	**		jump to saccade check states
	*/	
	t2start:
		do dx_show_fp(FPCHG, 0, 5, 5, 2, 2);
		to fixbreak on +WD0_XY & eyeflag
		to t2wait1 on DX_MSG % dx_check
	t2wait1:
		do timer_set1(1000, 100, 600, 200, 0, 0)
		to fixbreak on +WD0_XY & eyeflag
 		to t2ftc1 on MET % timer_check1
	t2ftc1:
		do dx_toggle2(TRGC1CD, 1, 1, 1000, 2, 1000);
		to fixbreak on +WD0_XY & eyeflag
		to t2wait2 on DX_MSG % dx_check
	t2wait2:
		do timer_set1(1000, 100, 600, 200, 0, 0)
		to fixbreak on +WD0_XY & eyeflag
 		to t2ftc2 on MET % timer_check1
	t2ftc2:
		do dx_toggle2(TRGC2CD, 0, 1, 1000, 2, 1000);
		to fixbreak on +WD0_XY & eyeflag
		to t2wait3 on DX_MSG % dx_check
	t2wait3:
		do timer_set1(1000, 100, 600, 200, 0, 0)
		to fixbreak on +WD0_XY & eyeflag
 		to t2ftc3 on MET % timer_check1
	t2ftc3:
		do dx_toggle2(FPOFFCD, 0, 0, 1000, NULLI, NULLI);
		to endtrial on DX_MSG % dx_check

	/* Task 3 is adaptiveODR ... we want to (each condition is optional):
	**		change fp to standard diameter/color 
	**		wait 1
	**		change targets 1
	**		wait 2
	**		change targets 2
	**		wait 3
	**		hide fp
	**		jump to saccade check states
	*/	
	t3start:
		do dx_show_fp(FPCHG, 0, 5, 5, 2, 2);
		to fixbreak on +WD0_XY & eyeflag
		to t3wait1 on DX_MSG % dx_check
	t3wait1:
		do timer_set1(1000, 100, 600, 200, 0, 0)
		to fixbreak on +WD0_XY & eyeflag
 		to t3ftc1 on MET % timer_check1
	t3ftc1:
		do dx_toggle2(TRGC1CD, 1, 1, 1000, 2, 1000);
		to fixbreak on +WD0_XY & eyeflag
		to t3wait2 on DX_MSG % dx_check
	t3wait2:
		do timer_set1(1000, 100, 600, 200, 0, 0)
		to fixbreak on +WD0_XY & eyeflag
 		to t3ftc2 on MET % timer_check1
	t3ftc2:
		do dx_toggle2(TRGC2CD, 0, 1, 1000, 2, 1000);
		to fixbreak on +WD0_XY & eyeflag
		to t3wait3 on DX_MSG % dx_check
	t3wait3:
		do timer_set1(1000, 100, 600, 200, 0, 0)
		to fixbreak on +WD0_XY & eyeflag
 		to t3ftc3 on MET % timer_check1
	t3ftc3:
		do dx_toggle2(FPOFFCD, 0, 0, 1000, NULLI, NULLI);
		to endtrial on DX_MSG % dx_check

	/* OUTCOME STATES
	** FIXBREAK
	** NCERR (No-Choice Error)
	**	ERROR
	** CORRECT
	*/
	endtrial:
		to correctfix on 1 % fix_task
		to endwinpos1
	endwinpos1:
		time 20
		do position_target_windows(50,50,50,50,50,50)
		to grace
	grace:
		time 300
		to sacmade on +WD0_XY & eyeflag
		to ncerr
	sacmade:
		time 50
		ec_send_code_hi(SACMAD);
		to correctmchk on -WD1_XY & eyeflag
		to correctrchk on -WD2_XY & eyeflag
		to errorchk 	on -WD3_XY & eyeflag
		to ncerr

	/* pref -- reward! */
	/* Fixation */
	correctfix: 
		do reward_trial(1,-1,-1)
	   to finish on 0 % pr_beep_reward
	
	/* Chose the mean target location */	
	correctmchk:
		time 40
		do dx_position_window(30, 30, -1, 0, WIND1)
		to correctmwait
	correctmwait:
		time 250
		to ncerr on +WD1_XY & eyeflag	
		to correctmacq
	correctmacq:
		do ec_send_code(TRGACQUIRECD)
		to correctm
	correctm:	
		do reward_trial(2,-1,-1)
	   to finish on 0 % pr_beep_reward

	correctrchk:
		time 40
		do dx_position_window(30, 30, -1, 0, WIND2)
		to correctrwait
	correctrwait:
		time 250
		to ncerr on +WD2_XY & eyeflag	
		to correctracq
	correctracq:
		do ec_send_code(TRGACQUIRECD)
		to correctr
	correctr:	
		do reward_trial(1,-1,-1)
	   to finish on 0 % pr_beep_reward

	errorchk:
		time 40
		do dx_position_window(30, 30, -1, 0, WIND3)
		to errorwait
	errorwait:
		time 250
		to ncerr on +WD3_XY & eyeflag	
		to erroracq
	erroracq:
		do ec_send_code(TRGACQUIRECD)
		to error

	/* error */
	error:
		time 3000
		do pr_score_trial(kError, 0, 1)
		to finish

	/* no choice */
	ncerr:
		time 3000
		do pr_score_trial(kNC, 0, 1)
		to finish

	/* fixation break */
	fixbreak:
		time 2500
		do pr_score_trial(kBrFix, 0, 1)
		to finish

	finish:
		do pr_finish_trial()
		to loop
}
