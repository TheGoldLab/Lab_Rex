/*	740.d
**
** combination of several simple fixation/saccade tasks:
**  - ftb:  fixation/target, possibly with beepage
**  - ftt2: two-choice block-wise probabilistic rewards
**
**	created by jig 5/09/2016 from 739.d
**	created by jig 2/10/2016 from 738.d
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
#define WIND1 1		/* window to compare with eye signal (T1) */
#define WIND2 2		/* window to compare with eye signal (T2) */

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
		"asl",  1, 
		"ftb",  2,
		"ftt2", 2,
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
		wd_src_pos(WIND0, WD_DIRPOS, 0, WD_DIRPOS, 0);
		wd_src_check(WIND0, WD_SIGNAL, EYEH_SIG, WD_SIGNAL, EYEV_SIG);

		/* WIND1: T1 WINDOW */
		wd_src_pos(WIND1, WD_DIRPOS, 0, WD_DIRPOS, 0);
		wd_src_check(WIND1, WD_SIGNAL, EYEH_SIG, WD_SIGNAL, EYEV_SIG);

		/* WIND2: T2 WINDOW */
		wd_src_pos(WIND2, WD_DIRPOS, 0, WD_DIRPOS, 0);
		wd_src_check(WIND2, WD_SIGNAL, EYEH_SIG, WD_SIGNAL, EYEV_SIG);
		
      /* init the screen */
		pr_setup();
	}
}

/* ROUTINE: start_trial
**
*/
int start_trial(long twinsize_h, long twinsize_v)
{
	/* No dynamic stimuli, so make draw_flag=3 the default.
	**	This draws each command ONCE
	*/
	dx_set_flags(DXF_D3);

	/* reset flag */
	gl_boost_reward = 0;

	/* Use WIND0 for eye check */
	wd_cntrl(WIND0, WD_ON);
	
	/* Just for convenience, use arguments to save target 
	**		horizontal/vertical window sizes,
	**  	to be used in check_fixflag
	*/
	gl_twinsize_h = twinsize_h;
	gl_twinsize_v = twinsize_v;

	/* outta */
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

/* ROUTINE: check_fixflag
**
*/
int check_fixflag(void)
{
	_PRtask task = gl_rec->trialP->task;

	/* Fixation task */
	if(pl_list_get_v(task->task_menus->lists[0], "Fixation_task")==1)
		return(1);

	/* saccade task -- always position WIND1 at target 1 */
	wd_cntrl(WIND1, WD_ON);
	dx_position_window(gl_twinsize_h, gl_twinsize_v, 1, 0, WIND1);

	/* saccade task -- also possibly add second window */
	if(pr_get_task_index>2) {
		wd_cntrl(WIND2, WD_ON);
		dx_position_window(gl_twinsize_h, gl_twinsize_v, 2, 0, WIND2);
	}

	return(0);
}

/* ROUTINE: Conditional beepulation */
int conditional_beep(long ecode, long prob, long index)
{
	char buf[512];

	if(!TOY_RCMP(prob))
		return(0);

	/* send message to play indexed sound */
	sprintf(buf, "%s('%s',%d);draw_flag=0;",
		DX_PLAY, DX_BEEP, index+1);
	dx_send(buf);

	/* drop code now */
	if(ecode)
		ec_send_code_hi(ecode);

	/* set flag to change reward */
	if(index==1)
		gl_boost_reward = 1;

	/* outta */
    return(0);
}

/* ROUTINE: score_choice
**
*/
int score_choice(long choice, long reward_num, long reward_on_time, 
			long reward_off_time, long beep_boost)
{
	_PRtask  task  = gl_rec->trialP->task;
	_PRtrial trial;
	_PLlist 	tim   = task->task_menus->lists[0];

	/* set trial to chosen target & drop ecode */
	task->trialPs_index = choice-1;
	gl_rec->trialP = trial = task->trialPs[task->trialPs_index];
	ec_send_code_lo(choice==1?TRGC1CD:TRGC2CD);

	/* get/use reward probability */
	printf("T%d: Checking for reward prob %ld\n", choice, 
			trial->list->properties[1]->values[0]);
	if(TOY_RCMP(trial->list->properties[1]->values[0])) {
		printf("REWARDED!\n");
		pr_score_trial(kCorrect, 0, 1);
		pr_set_reward(reward_num+(gl_boost_reward*beep_boost), 
			reward_on_time, reward_off_time, -1, 0, 0);
	} else {
		pr_score_trial(kError, 0, 1);
	}

	return(0);
}

/* ROUTINE: finish_trial
**
*/
int finish_trial(void)
{   
	/* don't check eye positions between trials */
   wd_cntrl(WIND0, WD_OFF);
   wd_cntrl(WIND1, WD_OFF);
   wd_cntrl(WIND2, WD_OFF);

   return(0);
}

int simulate_choices(void)
{
	int rval = TOY_RAND(1000);

	if(rval<100)
		return(0);
	else if(rval < 550)
		return(1);
	else
		return(2);
}

/* THE STATE SET */
%%
id 739
restart rinitf
main_set {
status ON
begin	first:
        to loop

   /* Start the loop!
   ** Note that the real workhorse here is pr_next_trial,
   **    called in go. It calls the task-specific
   **    "get trial" and "set trial" methods,
   **    and drops STARTCD and LISTDONECD, as
   **    appropriate.
   */
	loop:
		time 1000
		to pause on +PSTOP & softswitch
		to go
	pause:
        do pr_toggle_file(0)
		to go on -PSTOP & softswitch
	go:
		do pr_toggle_file(1)
		to trstart on MET % pr_start_trial
		to loop
	trstart:
		do start_trial(50,50)
		to fpshow
	fpshow:
		do dx_show_fp(FPONCD, 0, 5, 5, 2, 2);
        to caljump on DX_MSG % dx_check
    caljump:
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
		to correct

	/* Position window and wait for fixation
	*/
	fpwinpos:
		time 20  /* takes time to settle window */
 		do dx_position_window(50, 50, 0, 0, WIND0)
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
		do dx_position_window(50, 50, -1, 1, 0)
		to taskjmp
	taskjmp:
		to t0start on 0 % pr_get_task_index /* ASL validation   */
		to t1start on 1 % pr_get_task_index /* Fixation/beep    */
		to t2start on 2 % pr_get_task_index /* Saccade          */
		to t3start on 3 % pr_get_task_index /* Hazard task      */
		to t4start on 4 % pr_get_task_index /* Passive Hazard   */
		to finish

	/** 
    **
    ** TASK 0: ASL eye tracker validation  
    **  enters here only if cal0/val1 == 1
    **
    **/
	t0start:
		do timer_set1(0, 100, 600, 200, 500, 0)
 		to correct on MET % timer_check1
		to fixbreak on +WD0_XY & eyeflag
	 
	/** 
	**
	** TASK 1: Fixation only, possibly with beep
	**      wait, beep, wait, end
	**/
	t1start:
		do timer_set1(1000, 100, 300, 200, 500, 0)
		to t1beep on MET % timer_check1
		to fixbreak on +WD0_XY & eyeflag
   t1beep:
      do conditional_beep(TONEON, 0, 0);
		to t1wait2 on DX_MSG % dx_check
	t1wait2:
		do timer_set1(0,0,0,0,300,0)
		to correct on MET % timer_check1
		to fixbreak on +WD0_XY & eyeflag

	/** 
	**
	** TASK 2: visual- or memory- guided saccades
	**
	**/
	/* First wait period before possibly showing the target */
	t2start:
		do timer_set1(1000, 100, 300, 200, 500, 0)
		to t2showt on MET % timer_check1
		to fixbreak on +WD0_XY & eyeflag
	t2showt:
		do dx_toggle1(TARGONCD, 1, 1, 1000, NULLI, NULLI)
		to t2wait2 on DX_MSG % dx_check

	/* Second wait period before possibly turning off target */
	t2wait2:
		do timer_set1(0,0,0,0,300,0)
		to t2hidet on MET % timer_check1
		to fixbreak on +WD0_XY & eyeflag
	t2hidet:
		do dx_toggle1(TARGOFFCD, 0, 1, 1000, NULLI, NULLI)
		to t2wait3 on DX_MSG % dx_check

	/* Third wait period before turning off fp */
	t2wait3:
		do timer_set1(1000, 500, 1000, 800, 500, 0)
		to t2hidefp on MET % timer_check1
		to fixbreak on +WD0_XY & eyeflag
	t2hidefp:
		do dx_hide_fp(FPOFFCD)
		to fixcheck on DX_MSG % dx_check

	/** 
	**
	** TASK 3: 2AFC probabilistic-reward task
	**
	**/
	/* First wait period before possibly showing the targets */
	t3start:
		do timer_set1(1000, 100, 300, 200, 500, 0)
		to t3showt on MET % timer_check1
		to fixbreak on +WD0_XY & eyeflag
	t3showt:
		do dx_toggle2(TARGONCD, 1, 1, 1000, 2, 1000)
		to t3wait2 on DX_MSG % dx_check

	/* Second wait period before possibly giving reward-predicting beep */
	t3wait2:
		do timer_set1(0,0,0,0,300,0)
		to t3beep on MET % timer_check1
		to fixbreak on +WD0_XY & eyeflag
	t3beep:
      do conditional_beep(TONEON, 0, 1);
		to t3wait3 on DX_MSG % dx_check

	/* Third (possible) wait period before turning off fp */
	t3wait3:
		do timer_set1(1000, 500, 1000, 800, 500, 0)
		to t3hidefp on MET % timer_check1
		to fixbreak on +WD0_XY & eyeflag
	t3hidefp:
		do dx_hide_fp(FPOFFCD)
		to t3beep2
	t3beep2:
		do conditional_beep(TONEON, 0, 0)
		to fixcheck on DX_MSG % dx_check

	/** 
	**
	** TASK 4: 2AFC probabilistic-reward task
	**
	**/
	/* First wait period before possibly showing the targets */
	t4start:
		do timer_set1(1000, 100, 300, 200, 500, 0)
		to t4showt on MET % timer_check1
		to fixbreak on +WD0_XY & eyeflag
	t4showt:
		do dx_toggle2(TARGONCD, 1, 1, 1000, 2, 1000)
		to t4wait2 on DX_MSG % dx_check

	/* Second wait period before possibly changing the "oddball" */
	t4wait2:
		do timer_set1(0,0,0,0,300,0)
		to t4beep on MET % timer_check1
		to fixbreak on +WD0_XY & eyeflag
	t4beep:
		do conditional_beep(TONEON, 0, 1)
		to t4wait3 on DX_MSG % dx_check

	/* Third wait period before turning off fp */
	t4wait3:
		do timer_set1(1000, 500, 1000, 800, 500, 0)
		to t4hidefp on MET % timer_check1
		to fixbreak on +WD0_XY & eyeflag
	t4hidefp:
		do dx_hide_fp(FPOFFCD)
		to t4beep2
	t4beep2:
		do conditional_beep(TONEON, 0, 0)
		to fixcheck on DX_MSG % dx_check

    /* 
    **
    ** Check for fixation/saccade, then for hold times
    **
    */
	fixcheck:
		time 20
		to correct on MET % check_fixflag
		to grace
	grace:
		time 500    /* allow for eye movement */
		to sacmad
/*
		to sacmad on +WD0_XY & eyeflag
		to ncerr
*/
	sacmad:
		do ec_send_code(SACMAD)
		time 200
/*
		to t1choice on -WD1_XY & eyeflag
		to t2choice on -WD2_XY & eyeflag
		to ncerr
*/
		to ncerr on 0 % simulate_choices
		to t1choice on 1 % simulate_choices
		to t2choice on 2 % simulate_choices

	/* OUTCOMES:
	**  Fixation break
	**  No-Choice Error
	**  T1 choice
	**  T2 choice
	*/
	/* fixation break */
	fixbreak:
		do pr_score_trial(kBrFix, 0, 1)
		to fixbreak_wait
    fixbreak_wait:
		time 100
		to finish

	/* no choice */
	ncerr:
		do pr_score_trial(kNC, 0, 1) 	/* last parameter is "blank_flag" - blanks screen */
		to ncerr_wait
	ncerr_wait:
		time 100
		to finish

	/* T1/T2 choices */
	t1choice:
		do score_choice(1,1,200,50,2)
		to finish on 0 % pr_beep_reward
	t2choice:
		do score_choice(2,1,200,50,2)
		to finish on 0 % pr_beep_reward

	/* for fixation task */
	correct:
		time 10
		do pr_score_trial(kCorrect, 0, 1)
		to reward
	reward:
		do pr_set_reward(1, 200, 50, -1, 0, 0)
		to finish on 0 % pr_beep_reward
	
	finish:
		do finish_trial()
		to loop

	abort list:		
		finish
}
