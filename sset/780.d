/*	780.d
**
**  Paradigm for Jeff's MT FEF experiment
**	This paradigm includes:
**	 1) asl calibration
**   2) passive fixation with (or without) a dots field
**
**	created by jcl 3/16/08
*/

#include "rexHdr.h"
#include "paradigm_rec.h"
#include "toys.h"
#include "lcode.h"

/* PRIVATE data structures */

/* GLOBAL VARIABLES */
static _PRrecord 	gl_rec = NULL; /* KA-HU-NA */

	/* for now, allocate these here... */
MENU 	 	umenus[30];
RTVAR		rtvars[15];
USER_FUNC	ufuncs[15];

/* ROUTINES */

/* Macros */
#define PRV(n)		pl_list_get_v(gl_rec->prefs_menu,    (n))
#define TIMV(n) 	pl_list_get_v(gl_rec->trialP->task->task_menus->lists[0],(n))
#define TTMV(n) 	pl_list_get_v(gl_rec->trialP->task->task_menus->lists[1],(n))



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
				"angle", 0, 1.0, NULL),
		ufuncs, 
		"asl",	1, 
		"fixd",	1,
		"ft",	1,
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

	} else if(task_index >= 1) {

		/* Task: ft */
		pr_set_rtvar("angle", PL_L2PV(gl_rec->trialP->list, 0));


	} else {

		/* Task: ASL */
		pr_set_rtvar("angle", NULLI);
	}

	return(0);
}


/* ROUTINE: start_fixd
**	set timing parameters for fixation+dots task
*/
int start_fixd(void)
{	 
	return(0);
}


/* ROUTINE: set_fixtime
**	set required fixation time for fixation task
*/
int set_fixtime(void)
{
	timer_set1(0, 0, 0, 0, TTMV("fix_time"), 0);
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


/* ROUTINE: show_error
**
*/
int show_error(long error_type)
{

	if(error_type == 0)
		printf("Bad task index (%d)\n", pr_get_task_index());

	return(0);
}

/* ROUTINE: set_reward 
int set_reward(void)
{
	if (pr_get_task_index()==1)
		pr_set_reward(TTMV("#_reward"), PRV("Reward_on_time"), PRV("Reward_off_time"), TTMV("#_beep"), 58, 100);
	else
		pr_set_reward(1, PRV("Reward_on_time"), PRV("Reward_off_time"), 1, 58, 100);
	

}
*/

/* THE STATE SET 
*/
%%
id 780
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
		do dx_show_fp(FPONCD, 0, 4, 4, 2, 2);
		to fpwinpos on DX_MSG % dx_check
	fpwinpos:
		time 20  /* takes time to settle window */
		do dx_position_window(40, 40, 0, 0, 0)
		to calstart on 1 % do_calibration
		to fpwait

	/* CALIBRATION TASK
	** Check for joystick button press indicating a correct fixation
	** missed targets are scored as NC in order to be shown again later
	*/
	calstart:
		time 5000
		to calacc on 0 % dio_check_joybut
		to calfpchange		
	calfpchange:
		to calstart on 1 % dx_change_fp
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
		do pr_score_trial(kNoFix,0,1)
		to finish
	fpset:
		time 250 /* give gaze time to settle into place (fixation) */
		to fpwait on +WD0_XY & eyeflag
		to fpwin2
	fpwin2:
		time 20 /* again time to settle window */
		do dx_position_window(20, 20, -1, 1, 0)
		to taskjmp

	/* Jump to task-specific statelists
	*/
	taskjmp:
		to t0fp on 0 % pr_get_task_index
		to t1fp on 1 % pr_get_task_index
		to t2fp on 2 % pr_get_task_index
		to badtask
	badtask:
		do show_error(0)
		to finish

	/* Task 0 is asl .. we get here in "validate" mode.
	**	Simple fixation only.
	*/
	t0fp:
		do dx_show_fp(FPONCD, 0, 6, 6, 2, 2);
		to t0wait1 on DX_MSG % dx_check
	t0wait1:
		do timer_set1(1000, 100, 600, 200, 0, 0)
 		to t0winpos on MET % timer_check1
	t0winpos:
		time 20
		do dx_position_window(10,10,-1,0,0)
 		to correct

	/* Task 1 is a simple fixation task:
	**		change fp to standard diameter/color 
	**		wait
	**		check for broken fixation
	**		reward
	*/	
	t1fp:
		do start_fixd()
		to t1showfp
	t1showfp:	
		do dx_show_fp(FPONCD, 0, 6, 6, 2, 2);
		to t1wait1 on DX_MSG % dx_check
	t1wait1:
		do set_fixtime()
		to fixbreak on +WD0_XY & eyeflag 
 		to t1fpoff on MET % timer_check1
	t1fpoff:
		do dx_toggle2(FPOFFCD, 0, 0, 1000, NULLI, NULLI);
		to t1fixdone on DX_MSG % dx_check
	t1fixdone:
		to correct
	
	
	
	/* Task 2 is visually-guided saccade:
	**		change fp to standard diameter/color 
	**		wait1
	**		show target (object 1)
	**		wait2
	**		turn off fp
	**		STOP CHECKING FOR FIX BREAKS AND 
	**			START CHECKING FOR SACCADES
	**		wait1
	**		change f/t 1
	**		wait2
	*/	
	t2fp:
		do dx_show_fp(FPONCD, 0, 4, 4, 2, 2);
		to t2wait1 on DX_MSG % dx_check
	t2wait1:
		do timer_set1(1000, 80, 200, 50, 100, 0)
		to t2targon on MET % timer_check1
	t2targon:
		do dx_toggle2(TARGONCD, 1, 1, 1000, NULLI, NULLI);
		to t2twinpos1 on DX_MSG % dx_check
	t2twinpos1:
		time 20
		do dx_position_window(50,50,1,0,1)
		to t2wait2
	t2wait2:
		do timer_set1(0, 0, 0, 0, 1000, 0)
		to fixbreak on +WD0_XY & eyeflag 
		to t2fpoff on MET % timer_check1
	t2fpoff:
		do dx_toggle2(FPOFFCD, 0, 0, 1000, NULLI, NULLI);
		to t2twinpos2 on DX_MSG % dx_check
	t2twinpos2:
		time 20
		do dx_position_window(40,40,1,0,1)
		to t2checksac1
	t2checksac1:
		do timer_set1(0, 0, 0, 0, 500, 0)
		to t2checksac2 on -WD1_XY & eyeflag
		to t2ncerr on MET % timer_check1
	t2checksac2:
		do timer_set1(0, 0, 0, 0, 50, 0)
		to t2ncerr   on +WD1_XY & eyeflag
		to t2correct on MET % timer_check1
	t2ncerr: 
		do dx_toggle2(TARGOFFCD, 0, 1, 1000, NULLI, NULLI);
		to ncerr on DX_MSG % dx_check
	t2correct: 
		do dx_toggle2(TARGOFFCD, 0, 1, 1000, NULLI, NULLI);
		to correct on DX_MSG % dx_check
	
	
	
	/* OUTCOME STATES
	** NCERR (No-Choice Error)
	**	ERROR
	** CORRECT
	*/

	/* broken fixaton */
	fixbreak:
		time 800
		do pr_score_trial(kBrFix, 0, 1)	
		to finish

	/* no choice */
	ncerr:
		time 800
		do pr_score_trial(kNC, 0, 1)
		to finish

	/* error */
	error:
		time 800
		do pr_score_trial(kError, 0, 1)
		to finish

	/* pref -- reward! */
	correct:
		do pr_score_trial(kCorrect, 0, 1)
		to reward
	
	reward:
		do pr_set_reward(1, 100, 50, -1, 50, 50);
		to	finish on 0 % pr_beep_reward 
		
	finish:
		do	pr_finish_trial()
		to loop

	abort list:		
		finish
}

