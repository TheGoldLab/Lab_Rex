/*	734.d
**
** dots, new style
**
**	created from 724, which was created from 723
**	written by jig 2.09.06
**
*/

#include "rexHdr.h"
#include "paradigm_rec.h"
#include "toys.h"
#include "lcode.h"

/*
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <x86/inout.h>


#include "rexHdr.h"
#include "ldev.h"
#include "lcode.h"
#include "dio_mns.h"
#include "property_list.h"
#include "toys.h"
#include "timer.h"
*/

/** windows for eyeflag **/
#define WIND0	    0
#define ZERO		 0

/* calculate actual position from voltages from joystick */
#define CALC_JOY(x)	((int) (0.25 * (double) (x)))

/* PRIVATE data structures */

/* GLOBAL VARIABLES */
static _PRrecord 	gl_rec = NULL; /* KA-HU-NA */

	/* for now, allocate these here... */
MENU 	 	 umenus[30];
RTVAR		 rtvars[15];
USER_FUNC ufuncs[15];

/* ROUTINES */

/*
***** INITIALIZATION routines
*/

/* ROUTINE: autoinit
**
**	Initialize gl_rec. This will automatically
**		set up the menus, etc.
*/
void autoinit(void)
{
	gl_rec = pr_initV(0, 0, 
		umenus, NULL, /* pl_list_initV("prefs",  0, 1, "test", 0, 1.0, NULL), */
		rtvars, pl_list_initV("rtvars", 0, 1, "angle", 0, 1.0, NULL),
		ufuncs, "asl", 1, "ft", 1, NULL);
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

		first_time = 0;

/*
** initialize interface (window) parameters
**
**		da_cntrl_2(0,DA_STBY,0,1,DA_STBY,1);
**		da_cntrl_2(2,DA_STBY,2,3,DA_STBY,3);
**		da_cursor(0,1,CU_DA_ONE);
**		da_cursor(2,3,CU_DA_TWO);
*/

		/* Set a/d sources for window 0 x (eyeh=0) and y (eyev=1)
		**		and activate global variable "eyeflag"
		*/
		wd_src_check(WIND0, WD_SIGNAL, 0, WD_SIGNAL, 1);
		wd_src_pos	(WIND0, WD_DIRPOS, 0, WD_DIRPOS, 0);
		wd_cntrl    (WIND0, WD_ON);

		/* init the screen */
		pr_setup();
	}
}

/* ROUTINE: start_trial
**
*/
int start_trial(void)
{
	/* set the "angle" rtvar, for kicks */
	pr_set_rtvar("angle", pr_get_trial_property("angle",0));

	return(0);
}

/* THE STATE SET 
*/
%%
id 734
restart rinitf
main_set {
status ON
begin	first:
		to fwait

	/* 
	** First wait time, which can be used to add
	**	a delay after reset states (e.g., fixation break)
	*/
	fwait:
		do timer_set1(0,100,600,150,0,0)
		to loop on +MET % timer_check1

	/* 
	** Start the loop!
	**	Note that the real workhorse here is pr_next_trial,
	**		called in go. It calls the task-specific
	**		"get trial" and "set trial" methods,
	**		and drops STARTCD and LISTDONECD, as
	**		appropriate.
	**			
	*/
	loop:
		time 1500
		to pause on +PSTOP & softswitch
		to go
	pause:
		do ec_send_code(PAUSECD)
		to go on -PSTOP & softswitch
	go:
		to trstart on MET % pr_start_trial
		to loop
	trstart:
		do start_trial()
		to fpshow

	/* Show fixation point 
	*/
	fpshow:
		do dx_show_fp(0, 0, 0, 1, 10, 1)
		to cdfpshow on DX_MSG % dx_check
	cdfpshow:
		do ec_send_code(FPONCD)
		to calstart on 0 % pr_get_task_index
		to fpwinpos

   /* CALIBRATION TASK
   ** Check for joystick button press indicating a correct fixation
   ** missed targets are scored as NC in order to be shown again later
   */
   calstart:
      time 5000
      to calcode on 0 % dio_check_joybut
      to ncerr
   calcode:
      do ec_send_code(ACCEPTCAL)
      to pref

	/* Position window and wait for fixation
	*/
	fpwinpos:
		time 10 	/* takes time to settle window */
		do dx_position_window(20, 20, 0, 0, 0)
		to fpwait
	fpwait:
		time 5000
		to fpset on -WD0_XY & eyeflag
		to fpwait on MET % dx_change_fp
		to fpnofix
	fpnofix: 	/* failed to attain fixation */
		time 2500
		do pr_score_trial(kNoFix,0,0,0,0,1)
		to loop
	fpset:
		time 250 /* give gaze time to settle into place (fixation) */
		to fpwait on +WD0_XY & eyeflag
		to fpwin2
	fpwin2:
		do dx_position_window(10, 10, 3, 1, 0)
		time 50 /* again time to settle window */
		to twait1	

	/* FIRST WAIT, TARGET CHANGE */
	twait1:
		do timer_set1(1000, 100, 600, 200, 0, 0)
		to tshow on MET % timer_check1
	tshow:
		do dx_toggle_t(1, 1000, NULLI, NULLI, NULLI, NULLI)
		to tswent on DX_MSG % dx_check
	tswent:
		do ec_send_code(TARGC1CD)
		to twait2
	twait2:
		do timer_set1(1000, 100, 600, 200, 0, 0)
		to thide on MET % timer_check1
	thide:
		do dx_toggle_t(1, 0, NULLI, NULLI, NULLI, NULLI)
		to thwent on DX_MSG % dx_check
	thwent:
		do ec_send_code(TARGC1CD)
		to twait3
	twait3:
		do timer_set1(1000, 100, 600, 200, 0, 0)
		to grace on MET % timer_check1

	/* Grace period, waiting for saccade */
	grace:
		time 50
		do dx_position_window(20, 20, 3, 0, 0)
		to gracea
	gracea:
		time 500
		to saccd on +WD0_XY & eyeflag
		to ncerr
	saccd:
		do ec_send_code(EYINWD)
		to twinpos

	/* end trial possibilities:
	**
	** - brfix. broke fixation, counted regardless of trial type
	** - ncerr. didn't complete trial once something happened
	** - null.  chose wrong target in dots discrimination task
	** - pref.  finished correctly and got rewarded
	*/
	twinpos:
		do dx_position_window(50, 50, 1, 0, 0)
		time 50
		to pchk
	pchk:    /* check the saccade 	 */
		to phold on -WD0_XY & eyeflag    /* got it!  */
		to ncerr                      
	phold:   /* eye is in pref window */
		do ec_send_code(TRGACQUIRECD)
		time 250       /* gotta hold for this long   */
		to ncerr on +WD0_XY & eyeflag
		to pref

   /* NO CHOICE: didn't complete the task */
   ncerr:
      time 1000
      do pr_score_trial(kNC, 0, 0, 0, 0, 1)
      to trend

   /* PREF: update the totals and give a reward */
   pref:
		time 100
      do pr_score_trial(kCorrect, -3, 0, 3, 0, 1)
		to prrew
	prrew:
		time 200
		to prrew on MET % pr_next_reward 
    	to trend

	trend:
		do pr_finish_trial()
		to loop

abort list:		
	trend
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
      do pr_score_trial(kBrFix, 0, 0, 0, 0, 1)
      to etest

abort list:
}
