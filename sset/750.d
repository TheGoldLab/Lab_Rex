/*	750.d
**
** Paradigm for the fixed duration dots task   
**	
**	 created by Sharath 3/28/07
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

#define TIMV(n)	pl_list_get_v(gl_rec->trialP->task->task_menus->lists[0],(n))

#define WIND0 0		/* window to compare with eye signal */
#define EYEH_SIG 	0
#define EYEV_SIG 	1

#define FPCOLOR_ASL	   1
#define FPCOLOR_FT	   2 
#define FPCOLOR_INI		2
#define TARG1_COLOR		2
#define TARG2_COLOR		2

/* ROUTINES */

/*
** INITIALIZATION routines
*/
/* ROUTINE: autoinit
**
**	Initialize gl_rec. This will automatically
**		set up the menus, etc.
**
*/
void autoinit(void)
{

printf("autoinit start\n");

	gl_rec = pr_initV(0, 0, 
		umenus, NULL,
		rtvars, pl_list_initV("rtvars", 0, 1,
				"angle", 0, 1.0, NULL),
		ufuncs, 
		"asl",	 1,	
		"ft", 	 1,
		"dotsFD", 1,
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

	/* No dynamic stimuli, so make draw_flag=3 the default.
	**	This draws each command ONCE
	** dx_set_flags: (in dotsx.c)
	*/
	
	dx_set_flags(DXF_D1);

	return(0);
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
		return(1);
	} else {
		return(0);
	}
}


/* THE STATE SET 
*/
%%
id 750
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
		time 1000
		to pause on +PSTOP & softswitch
		to go
	pause:
		do pr_toggle_file(0)
		to go on -PSTOP & softswitch
	
	/*
    ** pr_start_trial: (in paradigm_rec.c) run get_trial and send start ecodes and task 
	** and trial ecodes to PLEXON regular task event ecodes are not tagged. 
	** Task and trial ecodes are tagged, send first the tag then the value. 
    */
	go:
		do pr_toggle_file(1)
		to trstart on MET % pr_start_trial
		to loop

	trstart:
		do start_trial()
		to fpshow
	fpshow:
		do dx_show_fp(FPONCD, 0, 5, 8, 1, 8)		
		to caljmp  
		to fpwinpos on DX_MSG % dx_check
	
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
		do dx_position_window(50, 50, 0, 0, 0)
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
		do dx_position_window(30, 30, 0, 0, 0)
		to taskjmp

	/* Jump to task-specific statelist*/
	taskjmp:
		to t0fp on 0 % pr_get_task_index
		to t1fp on 1 % pr_get_task_index
		to t2fp on 2 % pr_get_task_index	
		to badtask
	badtask:
		do show_error(0)
		to finish

	/* TASK 0: ASL eye tracker caliberation  */
	t0fp:
		do dx_show_fp(FPCHG, 0, 5, 5, FPCOLOR_ASL, FPCOLOR_ASL);
		to t0wait1 on DX_MSG % dx_check
	t0wait1:
		do timer_set1(1000, 100, 600, 200, 0, 0)
 		to t0winpos on MET % timer_check1
	t0winpos:
		time 20
		do dx_position_window(20, 20,-1,0,0)
 		to correctASL
	
/* TASK 1: Memory Guided saccades */
	
	t1fp:
		do dx_show_fp(FPONCD, 0, 4, 4, FPCOLOR_FT, FPCOLOR_FT)
		to t1wait1

	/* First wait period before showing the targets */
	t1wait1:
		do timer_set1(1000, 100, 300, 200, 500, 0)
		to fixbreak on +WD0_XY & eyeflag
 		to t1show1 on MET % timer_check1
	t1show1:
		do dx_toggle2(TARGONCD, 1, 1, 1000, NULLI, NULLI)
		to t1wait2 on DX_MSG % dx_check
	
	/* Second wait period before turning off something */
	t1wait2:
		do timer_set1(1000, 300, 800, 500, 500, 0)
		to fixbreak on +WD0_XY & eyeflag
 		to t1show2 on MET % timer_check1
	t1show2:
		do dx_toggle2(TARGOFFCD, 0, 1, 1000, NULLI, NULLI)
		to t1wait3 on DX_MSG % dx_check
	
	/* Third wait period before turning off all or some */
	t1wait3:
		do timer_set1(1000, 300, 800, 500, 500, 0)
		to fixbreak on +WD0_XY & eyeflag
 		to t1show3 on MET % timer_check1
	t1show3:
		do dx_toggle2(FPOFFCD, 0, 0, 1000, NULLI, NULLI)
		to grace on DX_MSG % dx_check
	
/* TASK 2: FD dots  */
	
	/* First wait period before showing the targets */
	t2fp:
		do dx_show_fp(FPONCD, 0, 3, -1, 4, -1)
		to t2wait1	
	t2wait1:
		do timer_set1(1000, 100, 300, 200, 500, 0)
		to t2show1
	t2show1:
		do dx_toggle2(TARGONCD, 1, 1, 1000, 2, 1000)
		to t2wait2 on DX_MSG % dx_check
	
	/* Second wait period before showing the dots */
	t2wait2:
		do timer_set1(1000, 300, 800, 500, 500, 0)
		to fixbreak on +WD0_XY & eyeflag
 		to t2showd on MET % timer_check1
	
	/* Show the dots & turn them off*/
	t2showd:
		do dx_toggle2(GORANDCD, 1, 0, 0, 3, 1000)
		to t2waitd on DX_MSG % dx_check
	t2waitd:
		do timer_set1(1000,400,800,500,0,0)
		to fixbreak on +WD0_XY & eyeflag
		to t2stopd on MET % timer_check1
	t2stopd:
		do dx_toggle2(ENDCD, 0, 0, 0, 3, 1000)
		to fixbreak on +WD0_XY & eyeflag
		to t2wait3 on DX_MSG % dx_check

	/* Turn on the targets if you need to and wait before turning off FP */
	t2wait3:
		do timer_set1(0,0,0,0,0,0)
		to t2show2
	t2show2:
		do dx_toggle2(TARGOFFCD, 1, 1, 1000, 2, 1000)
		to t2wait4 on DX_MSG % dx_check
	
	/* Turn off FP*/
	t2wait4:
		do timer_set1(0,0,0,0,500,500)
		to fixbreak on +WD0_XY & eyeflag
		to t2fpoff on MET % timer_check1
	t2fpoff:
		do dx_toggle2(FPOFFCD, 0, 0, 1000, 0, 0)
		to grace on DX_MSG % dx_check
		
	/* OUTCOME STATES
	** FIXBREAK
	** NCERR (No-Choice Error)
	**	ERROR
	** CORRECT
	*/

	/* fixation break */
	fixbreak:
		time 2500
		do pr_score_trial(kBrFix, 0, 1)
		to finish

	grace:
		time 300
		to sacmad on +WD0_XY & eyeflag
		to ncerr
	sacmad:
		do ec_send_code(SACMAD)
		to twinpos
	twinpos:
		do dx_position_window(50, 50, 1, 0, 0) /*rex needs 20 ms to settle the window */
		time 100
		to thold on -WD0_XY & eyeflag
		to nwinpos
	thold:
		do ec_send_code(TRGACQUIRECD)
		time 400
		to ncerr on +WD0_XY & eyeflag
     	to correct
	nwinpos:
		do dx_position_window(50, 50, 2, 0, 0)
		time 100
		to nhold on -WD0_XY & eyeflag
		to ncerr
	nhold:
		do ec_send_code(TRGACQUIRECD)
		time 200
		to ncerr on +WD0_XY & eyeflag
     	to error
	
	/* no choice */
	ncerr:
		time 3000
		do pr_score_trial(kNC, 0, 1)
		to finish

	/* error */
	error:
		time 3000
		do pr_score_trial(kError, 0, 1)
		to finish

	/* pref -- reward! */
	correct:
	   do pr_score_trial(kCorrect, 0, 1)
	   to reward
	   
	correctASL:
		do pr_score_trial(kCorrect, 0, 1)
		to rewardASL

	reward:
	   do pr_set_reward(-3, 200, 300, -1, 200, 300)
	   to finish on 0 % pr_beep_reward
	   
	rewardASL:
	  do pr_set_reward(1, 200, 300, -1, 200, 300)
	  to finish on 0 % pr_beep_reward   

	finish:
		pr_finish_trial()
		to loop
}
