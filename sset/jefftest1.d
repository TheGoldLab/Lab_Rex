/*
 *  jefftest1.d
 *
 *  add only the passive fixation task
 * 
 *  Created by Jeff Law on 1/29/08.
 *
 */

 
#include "rexHdr.h"
#include "paradigm_rec.h"
#include "toys.h"
#include "lcode.h"


// GLOBAL VARIABLES
static _PRrecord 	gl_rec = NULL; /* KA-HU-NA */


// other variables
long currentrew= 0;
long timego = 0;
long timesac = 0;
int  current_task_index = 0;

MENU 	 	 umenus[30];
RTVAR		 rtvars[15];
USER_FUNC    ufuncs[15];


// macros
#define TTMV(n) 	pl_list_get_v(gl_rec->trialP->task->task_menus->lists[0],(n))



// constants
#define WIND0			0		/* window to compare with eye signal */
#define WIND1			1		/* window for correct target */
#define WIND2			2		/* window for other target in dots task */
#define EYEH_SIG		0
#define EYEV_SIG		1

#define FPCOLOR_INI		15
#define FPCOLOR_FIXD	15
#define REWINDEX		0
#define ANGLEINDEX		1




/***     ROUTINES     ***/

/*
** ROUTINE: autoinit
**
**	Initialize gl_rec. This will automatically set up the menus, etc.
**	Long's note: the number after ufuncs determines how many sets of menus each task gets
**
*/
void autoinit(void)
{
	gl_rec = pr_initV(0, 0, 
		umenus, NULL,
		rtvars, pl_list_initV("rtvars", 0, 1, 
				"currentrew", 0, 1.0, NULL),
		ufuncs, 
		"fixd", 1, 
		NULL);
}


/*
** ROUTINE: rinitf
**
**	Initialize at first pass or at r s from keyboard 
**
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

		wd_src_pos(WIND1, WD_DIRPOS, 0, WD_DIRPOS, 0);
		wd_src_check(WIND1, WD_SIGNAL, EYEH_SIG, WD_SIGNAL, EYEV_SIG);

		wd_src_pos(WIND2, WD_DIRPOS, 0, WD_DIRPOS, 0);
		wd_src_check(WIND2, WD_SIGNAL, EYEH_SIG, WD_SIGNAL, EYEV_SIG);

		/* init the screen */
		pr_setup();
	}
}


/*
**
** ROUTINE: start_trial
**
*/
int start_trial(void)
{
	if(!pr_start_trial()) {
		pr_toggle_file(0);
		return(0);
	}

	// Set draw flags
	//	if there's any dynamic stimulus, set draw_flag=0
	//  else, set draw_flag=3
	//  (see dx_set_flags in dotsx.c)
	
	dx_set_flags(DXF_D1);

	return(MET);
}


/*
**
** ROUTINE: start_fixd
**
**	set times for states and set reward info used in fixation+dots task 
**
*/
int start_fixd( void )
{
	int angle0, angle_cur;

	/* set basic task timing */
	set_times("viewtimer",	TTMV("dur"),	-1);
	set_times("waitITI",	TTMV("ITI"),	-1);
	set_times("fpwait",		TTMV("abortT"), -1);
		
	return(0);
}


int correctfun(void)
{
		// printf("%d %d %d\n", timego, timesac, timesac-timego);
		return( pr_score_trial(kCorrect, 0, 1) );
 
}


int get_current_task_index(void)
{
	current_task_index = pr_get_task_index;
	return(0);
} 


/*							*/
/*							*/
/*		THE STATE SET		*/
/*							*/
/*							*/
%%
id 1000
restart rinitf
main_set 
{
	status ON
	
	begin	first:
			to prewait

	prewait:
			/* wait time before task */ 
			do timer_set1(0,100,600,200,0,0)
			to loop on +MET % timer_check1
    
    		to loop

	
	/*
	** Start the loop!
	** Note that the real workhorse here is pr_start_trial,
	**    called in trstart. It calls the task-specific
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
			/* send DIO toggle bit (usually used to toggle start and end trial in plexon) */
		
			do pr_toggle_file(1)
			to trstart
			
	trstart:
			/* run start_trial */
			to fpshow on MET % start_trial
			to loop	


	/*	show fixation point and wait for fixation	*/
	fpshow:
			do dx_show_fp(FPONCD, 0, 3, 3, FPCOLOR_INI, FPCOLOR_INI)		
			rl 5
			to fpwinpos
	
	fpwinpos:
			time 20  /* takes time to settle window */
			do dx_position_window(60, 60, 0, 0, 0)
			to fpwait
   
	fpwait:
			to fpset on -WD0_XY & eyeflag
			to fpnofix
			
	fpnofix:    /* failed to attain fixation */
			do pr_score_trial(kNoFix,0,0,0,0,1)
			to finish
			
	fpset:
			time 50 /* give gaze time to settle into place (fixation) */
			do ec_send_code(EYINWD)
			to fpwait on +WD0_XY & eyeflag
			to fpwin2
			
	fpwin2:
			time 20 /* again time to settle window */
			do dx_position_window(40, 40, 0, 0, 0)
			to taskjmp
			
			
	/*		task jumpstation, jump to task-specific state lists		*/
	taskjmp:
			do get_current_task_index()
			to start_fixd_state on +0 & current_task_index
			to badtask 
	
	badtask:
		/*	do show_error(0) */
			to finish
			
	
   
    /*		TASK 1: passive fixation with dots		*/
	start_fixd_state:
			do start_fixd()
			to t1fpon
				
	t1fpon:
			do dx_show_fp(FPCHG, 0, 3, 3,  FPCOLOR_FIXD,  FPCOLOR_FIXD)
			rl 10
			to t1dotson on DX_MSG % dx_check
	
	t1dotson:
			do dx_toggle2(GORANDCD, 1, 0, 0, 3, 1000)
			rl 30
			to viewtimer on DX_MSG % dx_check
		
	viewtimer:
			to fixbreak on +WD0_XY & eyeflag
			to t1dotsoff on MET % timer_check1
		
	t1dotsoff:
			do dx_toggle2(ENDCD, 0, 0, 0, 3, 1000)
			to fixbreak on +WD0_XY & eyeflag
			to t1fpoff on DX_MSG % dx_check
			
	t1fpoff:
			do dx_toggle2(FPOFFCD, 0, 0, 1000, 3, 0)
			rl 40
			to correct on DX_MSG % dx_check
	
	
	/*		deal with correct, error, bf or nc		*/
	correct:
			do correctfun()
			to finish 
	
	fixbreak:
			/* broken fixation */
			to fixbreak_score
			
	fixbreak_score:
			do pr_score_trial(kBrFix,0,0,0,0,1)
			to finish_fixbreak
	
	finish_fixbreak:
			do pr_finish_trial()
			rl 0
			to waitITI
	
	
	/*		clean up, ITI and go to next trial		*/
	finish:
			do pr_finish_trial()
			rl 0
			to waitITI
		
	waitITI:
			to loop



			
abort list:		
	finish


}











