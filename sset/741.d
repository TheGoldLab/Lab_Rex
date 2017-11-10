/*	741.d
**
** Hazard-dependent changes of the correct target in a pair of 2-armed
**    bandit saccade tasks. Uses task_2afcSimpleHazard
**
**	created by jig 11/03/2017 from 741.d
**	created by jig  5/09/2016 from 739.d
**	created by jig  2/10/2016 from 738.d
*/

#include "rexHdr.h"
#include "paradigm_rec.h"
#include "toys.h"
#include "lcode.h"

/* PRIVATE data structures */

/* GLOBAL VARIABLES */
static _PRrecord 	gl_rec=NULL; 	 /* KA-HU-NA */
static int        gl_twinsize_h=50, gl_twinsize_v=50; /* yeah...  */
static int			gl_boost_reward=0;

	/* for now, allocate these here... */
MENU 	 	 umenus[30];
RTVAR		 rtvars[15];
USER_FUNC ufuncs[15];

/* ROUTINES */

/* CONSTANTS/MACROS */
#define PRV(n)  pl_list_get_v(gl_rec->prefs_menu,    (n))

#define WIND0 0		/* window to compare with eye signal (FP) */
#define WIND1 1		/* window to compare with eye signal (correct) */
#define WIND2 2		/* window to compare with eye signal (error)  */

#define EYEH_SIG 	0
#define EYEV_SIG 	1

/*
**** INITIALIZATION routines
*/

/* ROUTINE: autoinit
**
**	Initialize gl_rec. This will automatically
**		set up the menus, etc.
*/
void autoinit(void)
{
	gl_rec = pr_initV(0, 0, 
		umenus, NULL, 
		rtvars, NULL,
		ufuncs, 
		"asl",	1, 
		"ft",    1,
		"ht4",   2,
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
		wd_src_check(WIND0, WD_SIGNAL, EYEH_SIG, WD_SIGNAL, EYEV_SIG);
		wd_src_pos 	(WIND0, WD_DIRPOS, 0, WD_DIRPOS, 0);
		wd_cntrl		(WIND0, WD_ON);

		/* WIND1: T1 WINDOW */
		wd_src_check(WIND1, WD_SIGNAL, EYEH_SIG, WD_SIGNAL, EYEV_SIG);
		wd_src_pos 	(WIND1, WD_DIRPOS, 0, WD_DIRPOS, 0);
		wd_cntrl		(WIND1, WD_ON);

		/* WIND2: T2 WINDOW */
		wd_src_check(WIND2, WD_SIGNAL, EYEH_SIG, WD_SIGNAL, EYEV_SIG);
		wd_src_pos 	(WIND2, WD_DIRPOS, 0, WD_DIRPOS, 0);
		wd_cntrl		(WIND2, WD_ON);

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
int position_target_windows(long width_cor, long height_cor, 
									 long width_err, long height_err)
{
	/* if ft task, just put first window over target location, and 
	** turn off other windows
	*/
	if(pr_get_task_index()==1) {
		dx_position_window(width_cor, height_cor, 1, 0, WIND1);
		dx_position_window(0,         0,          1, 0, WIND2);

	/* Otherwise position:
	**	WIND1 over correct target
	** WIND2 over error target
	*/
	} else {
		int index = pr_get_trial_property("target", 0);

		dx_position_window(width_cor, height_cor, index+1, 0, WIND1);
		dx_position_window(width_err, height_err, 2-index, 0, WIND2);
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
id 741
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
		do dx_position_window(10, 10, -1, 1, 0)
		to taskjmp

	/* Jump to task-specific statelists
	*/
	taskjmp:
		to t0start on 0 % pr_get_task_index
		to t1start on 1 % pr_get_task_index
		to t2start on 2 % pr_get_task_index
		to t3start on 3 % pr_get_task_index
		to finish

	/* TASK 0: ASL eye tracker calibration  */
	t0start:
		do timer_set1(1000, 100, 600, 200, 0, 0)
 		to corrects on MET % timer_check1
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

	/* Task 2 is ht4 ... we want to (each condition is optional):
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

	/* Task 3 is ht4 ... we want to (each condition is optional):
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
		do position_target_windows(50,50,50,50)
		to grace
	grace:
		time 300
		to sacmade on +WD0_XY & eyeflag
		to ncerr
	sacmade:
		time 50
		ec_send_code_hi(SACMAD);
		to correct on -WD1_XY & eyeflag
		to error   on -WD2_XY & eyeflag
		to ncerr

	/* correct fixation */
	correctfix: 
		do reward_trial(1,-1,-1)
	   to finish on 0 % pr_beep_reward
	
	/* saccade to correct target */	
	correct:
		time 250
		to ncerr on +WD1_XY & eyeflag	
		to correctacq
	correctacq:
		do ec_send_code(TRGACQUIRECD)
		to corrects
	corrects:	
		do reward_trial(2,-1,-1)
	   to finish on 0 % pr_beep_reward

	/* saccade to error target */
	error:
		time 250
		to ncerr on +WD2_XY & eyeflag	
		to erroracq
	erroracq:
		do ec_send_code(TRGACQUIRECD)
		to errors
	errors:
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

