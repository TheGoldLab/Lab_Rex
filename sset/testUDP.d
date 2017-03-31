/*	testUDP.d
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
***** INITIALIZATION routines
*/

/* ROUTINE: autoinit
**
**	Initialize gl_rec. This will automatically
**		set up the menus, etc.
*/
void autoinit(void)
{
	printf("autoninit!!!\n");
	fflush(stdout);

	gl_rec = pr_initV(0, 0, 
		umenus, NULL,
		rtvars, pl_list_initV("rtvars", 0, 1, "angle", 0, 1.0, NULL),
		ufuncs, "ft", 1, "ft", 1, NULL);

	printf("autoninit done!!!\n");
	fflush(stdout);
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

		/* init the screen */
printf("about to setup\n");
fflush(stdout);

		pr_setup();

printf("done to setup\n");
fflush(stdout);
	}
}

/* ROUTINE: start_trial
**
*/
int start_trial(void)
{
	/* just set the "angle" rtvar, for kicks */
	pr_set_rtvar("angle", pr_get_trial_property("angle",0));

	return(0);
}

/* THE STATE SET 
*/
%%
id 666
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
		to t0show

	/* Show fixation point, wait
	*/
	t0show:
		do dx_show_fp(NULLI, NULLI, NULLI, NULLI, NULLI, NULLI)
		to t0went on DX_MSG % dx_check
	t0went:
		do ec_send_code(FPONCD)
		to t0wait
	t0wait:
		do timer_set1(1000, 100, 600, 200, 0, 0)
		to t1show on MET % timer_check1

	/* Show target, wait */
	t1show:
		do dx_toggle_t(1, 1000, NULLI, NULLI, NULLI, NULLI)
		to t1went on DX_MSG % dx_check
	t1went:
		do ec_send_code(TARGC1CD)
		to t1wait
	t1wait:
		do timer_set1(1000, 100, 600, 200, 0, 0)
		to t2show on MET % timer_check1

	/* Hide target, wait */
	t2show:
		do dx_toggle_t(1, 0, NULLI, NULLI, NULLI, NULLI)
		to t2went on DX_MSG % dx_check
	t2went:
		do ec_send_code(TARGC1CD)
		to t2wait
	t2wait:
		do timer_set1(1000, 100, 600, 200, 0, 0)
		to t3show on MET % timer_check1

	/* Hide fp, wait */
	t3show:
		do dx_hide_fp()
		to t3went on DX_MSG % dx_check
	t3went:
		do ec_send_code(FPOFFCD)
		to t3wait
	t3wait:
		do timer_set1(1000, 100, 600, 200, 0, 0)
		to score on MET % timer_check1

	score:
		do pr_score_trial(kCorrect, 0, 0, 0, 0, 1)
		to trend

	trend:
		do pr_finish_trial()
		to loop

abort list:		
	trend
}

