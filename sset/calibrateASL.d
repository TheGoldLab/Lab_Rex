/*	calibrateASL.d
**
**	created by jig 7/6/06
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
		umenus, NULL, rtvars, NULL, ufuncs, "asl", 1, NULL);
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

#define WIND0 		0
#define EYEH_SIG 	0
#define EYEV_SIG 	1

		/* initialize interface (window) parameters */
		wd_src_check(WIND0, WD_SIGNAL, EYEH_SIG, WD_SIGNAL, EYEV_SIG);
		wd_src_pos  (WIND0, WD_DIRPOS, 0, WD_DIRPOS, 0);
		wd_cntrl    (WIND0, WD_ON);

		/* init the screen */
		pr_setup();
	}
}

/* THE STATE SET 
*/
%%
id 666
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
		do	ec_send_code(PAUSECD)
		to go on -PSTOP & softswitch
	go:
		to showfp on MET % pr_start_trial
		to loop

	/* Show fixation point
	*/
	showfp:
		do dx_toggle1(FPONCD, 1, 0, 1000, -1, -1);
		to calstart on DX_MSG % dx_check

	/* check for button press */
	calstart:
		time 5000
		to calacc on 0 % dio_check_joybut
		to ncerr
	calacc:
		do ec_send_code(ACCEPTCAL)
		to pref

	/* no choice */
	ncerr:
		do pr_score_trial(-2, 0, 0, 0, 0, 1)
		to finish

	/* pref -- reward! */
	pref:
		do pr_score_trial(1, 2, 0, 0, 0, 1)
		to	finish on 0 % pr_give_reward 

	finish:
		pr_finish_trial()
		to loop

abort list:		
	finish
}

