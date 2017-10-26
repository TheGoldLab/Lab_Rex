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
#define WIND3 3		/* dummy window for targets */

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

printf("autoinit start\n");

	gl_rec = pr_initV(0, 0, 
		umenus, NULL,
		rtvars, NULL,
		ufuncs, 
		"asl",            1,		
		"ftt",            1,
      "adaptiveODR",    2,
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
		wd_src_check(WIND0, WD_SIGNAL, EYEH_SIG, WD_SIGNAL, EYEV_SIG);
		wd_src_pos  (WIND0, WD_DIRPOS, 0, WD_DIRPOS, 0);
		wd_cntrl    (WIND0, WD_ON);

		wd_src_check(WIND1, WD_SIGNAL, EYEH_SIG, WD_SIGNAL, EYEV_SIG);
		wd_src_pos  (WIND1, WD_DIRPOS, 0, WD_DIRPOS, 0);
		wd_cntrl    (WIND1, WD_ON);

		/* initialize saccade detector */
		sd_set(1);

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
int reward_trial(long beep_time)
{
	pr_set_reward(rew, PRV("Reward_on_time"), PRV("Reward_off_time"), -1, 55, 100);
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
		to trialstart
	trialstart:  	
		to taskjmp

	/* Jump to task-specific statelists
	*/
	taskjmp:
		to t0start on 0 % pr_get_task_index
		to t1start on 1 % pr_get_task_index
		to t2start on 2 % pr_get_task_index
		to t3start on 3 % pr_get_task_index
		to badtask
	badtask:
		do show_error(0)
		to finish

	/* TASK 0: ASL eye tracker caliberation  */
	t0start:
		do dx_show_fp(FPCHG, 0, 5, 5, 1, 1);
		to t0wait1 on DX_MSG % dx_check
	t0wait1:
		do timer_set1(1000, 100, 600, 200, 0, 0)
 		to t0winpos on MET % timer_check1
	t0winpos:
		time 20
		do dx_position_window(20, 20,-1,0,0)
 		to correctASL
	
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

	/* Task 2 is adaptiveODR ... we want to (each condition is optional):
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


	/* OUTCOME STATES
	** FIXBREAK
	** NCERR (No-Choice Error)
	**	ERROR
	** CORRECT
	*/
	grace:
		time 300
		to sacstrt on +WD0_XY & eyeflag
		to ncerr
	sacstrt:
		ec_send_code_hi(SACMAD);
		to sacset on +SF_GOOD & sacflags
		to sacset /* ncerr */
	sacset:
		time 20
		do dx_position_window(30, 30, -1, 0, 0)
		to sacwait
	sacwait:
		time 250
		to ncerr on +WD0_XY & eyeflag
		to sacdone
	sacdone:
		do made_saccade(100)
		to fdbkwait
	fdbkwait:
		time 300
		to fdbkgo
	fdbkgo:
		to correct on +COR & sactest
		to error on +ERR & sactest
		to ncerr
	
	/* pref -- reward! */
	correct:
		do reward_trial(kCorrect)
	   to finish on 0 % pr_beep_reward

	/* error */
	error:
		time 3000
		do pr_score_trial(kError, 0, 1)
		to finish

	/* ASL loop */
	correctASL:
		do pr_score_trial(kCorrect, 0, 1)
		to rewardASL

	rewardASL:
	  do pr_set_reward(1, 100, 50, -1, 50, 50)
	  to finish on 0 % pr_beep_reward   

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

#/** PhEDIT attribute block
#-11:16777215
#0:9341:default:-3:-3:0
#9341:9345:TextFont9:-3:-3:0
#9345:14136:default:-3:-3:0
#**  PhEDIT attribute block ends (-0000170)**/
