/*	735.d
**
** Countermanding paradigm for Rishi
**
**	created by jig & Rishi 7/31/06
*/

#include "rexHdr.h"
#include "paradigm_rec.h"
#include "toys.h"
#include "lcode.h"

/* PRIVATE data structures */

/* GLOBAL VARIABLES */
static _PRrecord 	gl_rec = NULL; /* KA-HU-NA */

	/* for now, allocate these here... */
MENU 	 	 umenus[30];
RTVAR		 rtvars[15];
USER_FUNC ufuncs[15];

/* ROUTINES */

/* Macros */
#define PRV(n) pl_list_get_v(gl_rec->prefs_menu,    (n))

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
	printf("autoinit\n");
fflush(stdout);

	gl_rec = pr_initV(0, 0, 
		umenus, NULL, 
		rtvars, pl_list_initV("rtvars", 0, 1, 
				"angle", 0, 1.0, "ssd", 0, 1.0, NULL),
		ufuncs, 
		"asl", 1, 
		"ft",  2,
		"cm",  2,
		NULL);
}

/* ROUTINE: rinitf
**
** initialize at first pass or at r s from keyboard 
*/
void rinitf(void)
{
	static int first_time = 1;

printf("rinit\n");
fflush(stdout);

	/* This stuff needs to be done only once, but also
	**		needs to be done after the clock has started
	**		(so do NOT put it up in autoinit).
	*/
	if(first_time) {

		/* do this once */
		first_time = 0;

#define WIND0 		0
#define WIND1 		1
#define EYEH_SIG 	0
#define EYEV_SIG 	1

		/* initialize interface (window) parameters */
		wd_src_check(WIND0, WD_SIGNAL, EYEH_SIG, WD_SIGNAL, EYEV_SIG);
		wd_src_pos  (WIND0, WD_DIRPOS, 0, WD_DIRPOS, 0);
		wd_cntrl    (WIND0, WD_ON);

		wd_src_check(WIND1, WD_SIGNAL, EYEH_SIG, WD_SIGNAL, EYEV_SIG);
		wd_src_pos  (WIND1, WD_DIRPOS, 0, WD_DIRPOS, 0);
		wd_cntrl    (WIND1, WD_ON);

		/* init the screen */
		pr_setup();
	}
}

/* ROUTINE: start_trial
**
*/
int start_trial(void)
{
	int task_index = pr_get_task_index();

	/* No dynamic stimuli, so make draw_flag=3 the default.
	**	This draws each command ONCE
	*/
	dx_set_flags(DXF_D3);

	/* set rtvars and other things... */
	if(task_index >= 3) {

		/* Task: countermand */
		pr_set_rtvar("angle", PL_L2PV(gl_rec->trialP->list, 0));
		pr_set_rtvar("ssd", 	 PL_L2PV(gl_rec->trialP->list, 1));

	} else if(task_index >= 1) {

		/* Task: ft */
		pr_set_rtvar("angle", PL_L2PV(gl_rec->trialP->list, 0));
		pr_set_rtvar("ssd", 	 NULLI);

		/* check for fixation only */
		if(pr_get_task_menu_value("setup", "Fixation_task", 0) == 1)
			ec_send_code_lo(FIX1CD);	

	} else {

		/* Task: ASL */
		pr_set_rtvar("angle", NULLI);
		pr_set_rtvar("ssd", 	 NULLI);
	}

	return(0);
}

/* ROUTINE: set_ssd
**
** Set the stop-signal delay on a
** 	countermanding trial
*/
int set_ssd(void)
{
	/* set the timer to countermand */
	timer_set1(0, 0, 0, 0, PL_L2PV(gl_rec->trialP->list, 1), 0);

	ec_send_code_tagged(I_STOPSIGD, 7000+(PL_L2PV(gl_rec->trialP->list, 1)));

	return(0);
}

/* ROUTINE: do_calibration
**
**	Returns 1 if doing calibration (ASL task)
*/
int do_calibration(void)
{
	return(pr_get_task_index() == 0 && 
		pr_get_task_menu_value("setup", "Cal0/Val1", 0) == 0);
}

/* ROUTINE: fix_task
**
**	Returns 1 if a fixation-only task (FT task)
*/
int fix_task(void)
{
	return(pr_get_task_menu_value("setup", "Fixation_task", 0) == 1);
}	

/* ROUTINE: show_error
**
*/
int show_error(long error_type)
{

	if(error_type == 0)
		printf("Bad task index (%d)\n", pr_get_task_index());

	return(0);
}

/* ROUTINE: set_reward */
int set_reward(void)
{
	pr_set_reward(-3, PRV("Reward_on_time"), PRV("Reward_off_time"), -1, 55, 100);
}


/* THE STATE SET 
*/
%%
id 735
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
		time 1000
		to pause on +PSTOP & softswitch
		to go
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
		to correct

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
		to t0fp on 0 % pr_get_task_index
		to t1fp on 1 % pr_get_task_index
		to t2fp on 2 % pr_get_task_index
		to t3fp on 3 % pr_get_task_index
		to t4fp on 4 % pr_get_task_index
		to badtask
	badtask:
		do show_error(0)
		to finish

	/* Task 0 is asl .. we get here in "validate" mode.
	**	Simple fixation only.
	*/
	t0fp:
		do dx_show_fp(FPONCD, 0, 5, 5, 2, 2);
		to t0wait1 on DX_MSG % dx_check
	t0wait1:
		do timer_set1(1000, 100, 600, 200, 0, 0)
 		to t0winpos on MET % timer_check1
	t0winpos:
		time 20
		do dx_position_window(10,10,-1,0,0)
 		to correct

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
	t1fp:
		do dx_show_fp(FPONCD, 0, 5, 5, 2, 2);
		to t1wait1 on DX_MSG % dx_check
	t1wait1:
		do timer_set1(1000, 100, 600, 200, 0, 0)
 		to t1ftc1 on MET % timer_check1
	t1ftc1:
		do dx_toggle2(TRGC1CD, 0, 0, 1000, NULLI, NULLI);
		to t1wait2 on DX_MSG % dx_check
	t1wait2:
		do timer_set1(1000, 100, 600, 200, 0, 0)
 		to t1ftc2 on MET % timer_check1
	t1ftc2:
		do dx_toggle2(TRGC2CD, 0, 0, 1000, NULLI, NULLI);
		to t1wait3 on DX_MSG % dx_check
	t1wait3:
		do timer_set1(1000, 100, 600, 200, 0, 0)
 		to t1ftc3 on MET % timer_check1
	t1ftc3:
		do dx_toggle2(FPOFFCD, 0, 0, 1000, NULLI, NULLI);
		to t1winpos on DX_MSG % dx_check
	t1winpos:
		time 20
		do dx_position_window(10,10,-1,0,0)
		to t1pwait1
	t1pwait1: 
		do timer_set1(1000, 100, 600, 200, 0, 0)
 		to t1pftc1 on MET % timer_check1
		to t1twinpos on +WD0_XY & eyeflag
	t1pftc1:
		do dx_toggle2(TRGC3CD, 0, 0, 1000, NULLI, NULLI);
		to t1pwait2 on DX_MSG % dx_check
		to t1twinpos on +WD0_XY & eyeflag
	t1pwait2:
		do timer_set1(1000, 100, 600, 200, 0, 0)
 		to t1fixdone on MET % timer_check1
		to t1twinpos on +WD0_XY & eyeflag
	t1fixdone:
		to correct on 1 % fix_task
		to error
	t1twinpos:
		time 80
		do dx_position_window(50, 50, 1, 0, 0)
		to t1tchk
	t1tchk:
		to t1thold on -WD0_XY & eyeflag
		to ncerr
	t1thold:
		time 400
		do ec_send_code(TRGACQUIRECD)
		to error on +WD0_XY & eyeflag
     	to error on 1 % fix_task
		to correct

	/* Task 2 is ft ... just like task 1 
	*/
	t2fp:
		do dx_show_fp(FPONCD, 0, 5, 5, 2, 2);
		to t2wait1 on DX_MSG % dx_check
	t2wait1:
		do timer_set1(1000, 100, 600, 200, 0, 0)
		to t2ftc1 on MET % timer_check1
	t2ftc1:
		do dx_toggle2(TRGC1CD, 0, 0, 1000, NULLI, NULLI);
		to t2wait2 on DX_MSG % dx_check
	t2wait2:
		do timer_set1(1000, 100, 600, 200, 0, 0)
		to t2ftc2 on MET % timer_check1
	t2ftc2:
		do dx_toggle2(TRGC2CD, 0, 0, 1000, NULLI, NULLI);
		to t2wait3 on DX_MSG % dx_check
	t2wait3:
		do timer_set1(1000, 100, 600, 200, 0, 0)
		to t2ftc3 on MET % timer_check1
	t2ftc3:
		do dx_toggle2(FPOFFCD, 0, 0, 1000, NULLI, NULLI);
		to t2winpos on DX_MSG % dx_check
	t2winpos:
		time 20
		do dx_position_window(10,10,-1,0,0)
		to t2pwait1
	t2pwait1:
		do timer_set1(1000, 100, 600, 200, 0, 0)
		to t2pftc1 on MET % timer_check1
		to t2twinpos on +WD0_XY & eyeflag
	t2pftc1:
		do dx_toggle2(TRGC3CD, 0, 0, 1000, NULLI, NULLI);
		to t2pwait2 on DX_MSG % dx_check
		to t2twinpos on +WD0_XY & eyeflag
	t2pwait2:
		do timer_set1(1000, 100, 600, 200, 0, 0)
		to t2fixdone on MET % timer_check1
		to t2twinpos on +WD0_XY & eyeflag
	t2fixdone:
		to correct on 1 % fix_task
		to error
	t2twinpos:
		time 80
		do dx_position_window(50, 50, 1, 0, 0)
		to t2tchk
	t2tchk:
		to t2thold on -WD0_XY & eyeflag
		to ncerr
	t2thold:
		time 400
		do ec_send_code(TRGACQUIRECD)
     	to error on 1 % fix_task
		to error on +WD0_XY & eyeflag
		to correct

	/* Task 3: COUNTERMAND
	**
	*/
	t3fp:
		do dx_show_fp(FPONCD, 0, 5, 5, 2, 2);
		to t3wait1 on DX_MSG % dx_check
	t3wait1:
     do timer_set1(1000, 100, 600, 200, 0, 0)
     to t3ftc1 on MET % timer_check1
	t3ftc1:
		do dx_toggle2(TRGC1CD, 0, 0, 1000, NULLI, NULLI);
		to t3wait2 on DX_MSG % dx_check
	t3wait2:
		do timer_set1(1000, 100, 600, 200, 0, 0)
		to t3ftc2 on MET % timer_check1
	t3ftc2:
		do dx_toggle2(FPOFFCD, 0, 0, 1000, NULLI, NULLI);
		to t3winpos on DX_MSG % dx_check
	t3winpos:
		time 20
		do dx_position_window(10,10,-1,0,0)
		to t3setssd
	t3setssd:
		do set_ssd()
     	to t3twinpos on +WD0_XY & eyeflag
 		to t3cmd on MET % timer_check1
	t3cmd:
		do dx_toggle1(CNTRMNDCD, 1, 0, 1000, NULLI, NULLI)
     	to t3twinpos on +WD0_XY & eyeflag
		to t3pwait1 on DX_MSG % dx_check
	t3pwait1:
		time 700
     	to t3twinpos on +WD0_XY & eyeflag
		to correct
	t3twinpos:
		time 80
		do dx_position_window(50, 50, 1, 0, 0)
		to t3tchk
	t3tchk:
		to error on -WD0_XY & eyeflag
		to ncerr

	/* Task 4: COUNTERMAND
	**
	*/
   t4fp:
      do dx_show_fp(FPONCD, 0, 5, 5, 2, 2);
      to t4wait1 on DX_MSG % dx_check
   t4wait1:
     do timer_set1(1000, 100, 600, 200, 0, 0)
     to t4ftc1 on MET % timer_check1
   t4ftc1:
      do dx_toggle2(TRGC1CD, 0, 0, 1000, NULLI, NULLI);
      to t4wait2 on DX_MSG % dx_check
   t4wait2:
      do timer_set1(1000, 100, 600, 200, 0, 0)
      to t4ftc2 on MET % timer_check1
   t4ftc2:
      do dx_toggle2(FPOFFCD, 0, 0, 1000, NULLI, NULLI);
      to t4winpos on DX_MSG % dx_check
   t4winpos:
      time 20
      do dx_position_window(10,10,-1,0,0)
      to t4setssd
   t4setssd:
      do set_ssd()
      to t4twinpos on +WD0_XY & eyeflag
      to t4cmd on MET % timer_check1
   t4cmd:
      do dx_toggle1(CNTRMNDCD, 1, 0, 1000, NULLI, NULLI)
      to t4twinpos on +WD0_XY & eyeflag
      to t4pwait1 on DX_MSG % dx_check
   t4pwait1:
      time 700
      to t4twinpos on +WD0_XY & eyeflag
      to correct
   t4twinpos:
      time 80
      do dx_position_window(50, 50, 1, 0, 0)
      to t4tchk
   t4tchk:
      to error on -WD0_XY & eyeflag
      to ncerr

	/* OUTCOME STATES
	** NCERR (No-Choice Error)
	**	ERROR
	** CORRECT
	*/

	/* broken fixaton */
	fixbreak:
		do pr_score_trial(kBrFix, 0, 1)	
		to finish

	/* no choice */
	ncerr:
		time 1000
		do pr_score_trial(kNC, 0, 1)
		to finish

	/* error */
	error:
		time 1000
		do pr_score_trial(kError, 0, 1)
		to finish

	/* pref -- reward! */
	correct:
		do pr_score_trial(kCorrect, 0, 1)
		to reward
	
	reward:
		do set_reward()
		to	finish on 0 % pr_beep_reward 
		
	finish:
		do	pr_finish_trial()
		to loop

	abort list:		
		finish
}


/* set to check for fixation bread during task...
** use set_eye_flag to set gl_eye_state to non-zero
** to enable loop
*/
eye_set {
status ON
   begin efirst:
      to etest
   etest:
      to echk on 1 % dx_check_fix
   echk:
      to efail on +WD0_XY & eyeflag
		to etest on 0 % dx_check_fix
	efail:
		do pr_score_trial(kBrFix, 0, 0, 0, 1, 1)
		to etest

abort list:
}
