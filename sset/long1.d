/*	long1.d
**
** 	paradigm for Long's use. Includes dot RT task, regular Mem sac task, 
**	vis sac task
**	Asymmetric reward versions accomplished by setting big/small reward, 
**
**	created by Long 1/08/07
** 3-20-2007 revised event sequence, Long Ding
** 3-13-2007 Long added regular dots task, wrong timing events
** Note 2-27-2007 Long: 
**		for ASL calibration and validation, reward is given by function 
**		pr_give_reward, which get reward size information from menu Preferences
**		menu items (Reward_on_time, Reward_off_time, Beep_every_reward); 
**		For MGS, VGS and dotsRT tasks, reward DIO is set directly in states by calling
**		fun_rewardon and fun_rewardoff. The time of state "rewardon" is set
**		for each trial in start_xxx functions, based on reward contingency and big/small
**		reward size in task property menus.
** running line convention: 0 - intertrial  5 - initial fp on, 10 - fp change after switching to task
**		20 - cue on (tgt on for dots)	 30 - dots on	40 - go	50 - sac	60 - rew
**	Added feature to get rough measure of RT (time from dots on to SACMAD ). Long 3-29-07
*/

#include "rexHdr.h"
#include "paradigm_rec.h"
#include "toys.h"
#include "lcode.h"

/* PRIVATE data structures */

/* GLOBAL VARIABLES */
static _PRrecord 	gl_rec = NULL; /* KA-HU-NA */
long currentrew= 0;
long timego = 0;
long timesac = 0;



	/* for now, allocate these here... */
MENU 	 	 umenus[30];
RTVAR		 rtvars[15];
USER_FUNC ufuncs[15];

/* MACROS for memory+visual sac task */

#define TTMV(n) 	pl_list_get_v(gl_rec->trialP->task->task_menus->lists[0],(n))
#define TPMV(n) 	pl_list_get_v(gl_rec->trialP->task->task_menus->lists[1],(n))

#define WIND0 0		/* window to compare with eye signal */
#define WIND1 1		/* window for correct target */
#define WIND2 2		/* window for other target in dots task */
	/* added two dummy window to signal task events in rex window */
#define WIND3 3 		/* dummy window for fix point */
#define WIND4 4 		/* dummy window for target */
#define EYEH_SIG 	0
#define EYEV_SIG 	1

#define FPCOLOR_INI	15
#define FPCOLOR_ASL	1
#define FPCOLOR_MGS	2
#define FPCOLOR_VGS	3
#define FPCOLOR_DOT	4
#define FPCOLOR_REGDOT 1
#define REWINDEX		0
#define ANGLEINDEX	1

/* ROUTINES */

/*
**** INITIALIZATION routines
*/
/* ROUTINE: autoinit
**
**	Initialize gl_rec. This will automatically
**		set up the menus, etc.
**	Long's note: the number after ufuncs determines how many sets of menus each task gets
*/
void autoinit(void)
{

// printf("autoinit start\n");

	gl_rec = pr_initV(0, 0, 
		umenus, NULL,
		rtvars, pl_list_initV("rtvars", 0, 1, 
				"currentrew", 0, 1.0, NULL),
		ufuncs, 
		"asl", 1, 
		"mgs", 1, 
		"vgs",  1,
		"dotsrt", 1,
		"dotsreg", 1,
		NULL);

// printf("autoinit end\n");
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

		wd_src_pos(WIND1, WD_DIRPOS, 0, WD_DIRPOS, 0);
		wd_src_check(WIND1, WD_SIGNAL, EYEH_SIG, WD_SIGNAL, EYEV_SIG);

		wd_src_pos(WIND2, WD_DIRPOS, 0, WD_DIRPOS, 0);
		wd_src_check(WIND2, WD_SIGNAL, EYEH_SIG, WD_SIGNAL, EYEV_SIG);

		wd_src_pos(WIND3, WD_DIRPOS, 0, WD_DIRPOS, 0);
		wd_src_check(WIND3, WD_SIGNAL, EYEH_SIG, WD_SIGNAL, EYEV_SIG);

		wd_src_pos(WIND4, WD_DIRPOS, 0, WD_DIRPOS, 0);
		wd_src_check(WIND4, WD_SIGNAL, EYEH_SIG, WD_SIGNAL, EYEV_SIG);

		/* init the screen */
		pr_setup();
	}
}

/* ROUTINE: start_trial
**
*/
int start_trial(void)
{
	int task_index;

	if(!pr_start_trial()) {
		pr_toggle_file(0);
		return(0);
	}

 	task_index = pr_get_task_index();


	/* No dynamic stimuli, so make draw_flag=3 the default.
	**	This draws each command ONCE
	*/
	/* dx_set_flags: (in dotsx.c)
	**
 	
dx_set_flags(DXF_D3);
	*/
	
	dx_set_flags(DXF_D1);
	wd_siz(WIND3, 0, 0);
	wd_siz(WIND4, 0, 0);

	return(MET);
}



int show_error(long error_type)
{

	if(error_type == 0)
		printf("Bad task index (%d)\n", pr_get_task_index());

	return(0);
}

/* ROUTINE: start_mgs
**
**	set times for states and set reward info used in memory sac task 
*/
int start_mgs( void )
{
	int smallrew, bigrew, flag_asymRew;
	int angle0, angle_cur;

	long tarwin;

	/* set basic task timing */

	set_times("precue", TTMV("precue"), -1);
	set_times("delay", TTMV("delay"), -1);
	set_times("waitforsac", TTMV("wait4sacon"), -1);
	set_times("sacon", TTMV("wait4hit"), -1);
	set_times("tgtacq", TTMV("gap2feedback"), -1);
	set_times("rewarddelay", TTMV("delay2rew"), -1);
	
	/* set reward size based on current trial target loc and rew contingency */

	smallrew = TPMV("smallreward");
	bigrew = TPMV("bigreward");
	flag_asymRew = PL_L2PV(gl_rec->trialP->list, REWINDEX);
	if ( flag_asymRew == 2) {
		/* equal reward */
		currentrew = (smallrew+bigrew)/2;
	} else {
		/* asymmetric reward, assumes 2 possible targets only */
		angle_cur = PL_L2PV(gl_rec->trialP->list, ANGLEINDEX);
		angle0 = TPMV("Angle_o");
		if (flag_asymRew) {
			currentrew = smallrew;
			if (angle_cur == angle0) currentrew = bigrew;
		} else {
			currentrew = bigrew;
			if (angle_cur == angle0) currentrew = smallrew;
		}
	}
	set_times("rewardon", currentrew, -1);

	/* turn on the target window */
	tarwin = TPMV("targetwin");
	dx_position_window(tarwin, tarwin, 1, 0, 1);
//
	dx_position_window(tarwin, tarwin, 1, 0, 2);

	/* turn on the dummy window on rex to indicate fix on */
	dx_position_window(10, 10, 0, 0, 3);
	
	/* set rtvars */
	pr_set_rtvar("currentrew", currentrew);
 

	return(0);
}

/* ROUTINE: start_vgs
**
**	set times for states and set reward info used in visually-guided sac task 
*/
int start_vgs(void)
{

 	int smallrew, bigrew, flag_asymRew;
	int angle0, angle_cur;

	long tarwin;

	/* set basic task timing */

	set_times("pretgt", TTMV("pretgt"), -1);
	set_times("waitforsac_vgs", TTMV("wait4sacon"), -1);
	set_times("sacon_vgs", TTMV("wait4hit"), -1);
	set_times("rewarddelay_vgs", TTMV("delay2rew"), -1);
	
	/* set reward size based on current trial target loc and rew contingency */

	smallrew = TPMV("smallreward");
	bigrew = TPMV("bigreward");
	flag_asymRew = PL_L2PV(gl_rec->trialP->list, REWINDEX);
	if ( flag_asymRew == 2) {
		/* equal reward */
		currentrew = (smallrew+bigrew)/2;
	} else {
		/* asymmetric reward, assumes 2 possible targets only */
		angle_cur = PL_L2PV(gl_rec->trialP->list, ANGLEINDEX);
		angle0 = TPMV("Angle_o");
		if (flag_asymRew) {
			currentrew = smallrew;
			if (angle_cur == angle0) currentrew = bigrew;
		} else {
			currentrew = bigrew;
			if (angle_cur == angle0) currentrew = smallrew;
		}
	}
	set_times("rewardon", currentrew, -1);

	/* turn on the target window */
	tarwin = TPMV("targetwin");
	dx_position_window(tarwin, tarwin, 1, 0, 1);

	/* turn on the dummy window on rex to indicate fix on */
	dx_position_window(10, 10, 0, 0, 3);
	
	/* set rtvars */
	pr_set_rtvar("currentrew", currentrew);
 

	return(0);
}

/* ROUTINE: start_dots
**
**	set times for states and set reward info used in dots task 
*/
int start_dots( void )
{
	int smallrew, bigrew, flag_asymRew;
	int angle0, angle_cur;

	long tarwin;
	/* set basic task timing */

	set_times("waitforsac_dots", TTMV("wait4sacon"), -1);
	set_times("wait4hit_dots", TTMV("wait4hit"), -1);

/*	set_times("rewarddelay_dots", TTMV("delay2rew"), -1); */

	set_times("bothtgtoff", TTMV("errorFB"), -1);

	/* set reward size based on current trial target loc and rew contingency */

	smallrew = TPMV("smallreward");
	bigrew = TPMV("bigreward");
	flag_asymRew = PL_L2PV(gl_rec->trialP->list, REWINDEX);
	if ( flag_asymRew == 2) {
		/* equal reward */
		currentrew = (smallrew+bigrew)/2;
	} else {
		/* asymmetric reward, assumes 2 possible targets only */
		angle_cur = PL_G2PVS(gl_rec->dx->current_graphic, 1, 6, 0);
		angle0 = TPMV("Angle_o");
		if (flag_asymRew) {
			currentrew = smallrew;
			if (angle_cur == angle0) currentrew = bigrew;
		} else {
			currentrew = bigrew;
			if (angle_cur == angle0) currentrew = smallrew;
		}
	}
	set_times("rewardon", currentrew, -1);

	/* turn on the target window */
	tarwin = TPMV("targetwin");
	dx_position_window(tarwin, tarwin, 1, 0, 1);

	dx_position_window(tarwin, tarwin, 2, 0, 2);

	/* turn on the dummy window on rex to indicate fix on */
	dx_position_window(10, 10, 0, 0, 3);
	
	/* set rtvars */
	pr_set_rtvar("currentrew", currentrew);
 

	return(0);
}


/* ROUTINE: start_dots
reg
**
**	set times for states and set reward info used in regular dots task 
*/
int start_dotsreg( void )
{
	int smallrew, bigrew, flag_asymRew;
	int angle0, angle_cur;

	long tarwin;
	/* set basic task timing */

	set_times("waitforsac_dotsreg", TTMV("wait4sacon"), -1);
	set_times("wait4hit_dotsreg", TTMV("wait4hit"), -1);

	set_times("rewarddelay_dotsreg", TTMV("delay2rew"), -1);

/*	set_times("bothtgtoffreg", TTMV("errorFB"), -1); */

	/* set reward size based on current trial target loc and rew contingency */

	smallrew = TPMV("smallreward");
	bigrew = TPMV("bigreward");
	flag_asymRew = PL_L2PV(gl_rec->trialP->list, REWINDEX);
	if ( flag_asymRew == 2) {
		/* equal reward */
		currentrew = (smallrew+bigrew)/2;
	} else {
		/* asymmetric reward, assumes 2 possible targets only */
		angle_cur = PL_G2PVS(gl_rec->dx->current_graphic, 1, 6, 0);
		angle0 = TPMV("Angle_o");
		if (flag_asymRew) {
			currentrew = smallrew;
			if (angle_cur == angle0) currentrew = bigrew;
		} else {
			currentrew = bigrew;
			if (angle_cur == angle0) currentrew = smallrew;
		}
	}
	set_times("rewardon", currentrew, -1);

	/* turn on the target window */
	tarwin = TPMV("targetwin");
	dx_position_window(tarwin, tarwin, 1, 0, 1);

	dx_position_window(tarwin, tarwin, 2, 0, 2);

	/* turn on the dummy window on rex to indicate fix on */
	dx_position_window(10, 10, 0, 0, 3);
	
	/* set rtvars */
	pr_set_rtvar("currentrew", currentrew);
 

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

int dummycue(void)
{
	/* turn on the dummy window on rex to indicate cue on */
	dx_position_window(5, 5, 0, 0, 4);
	return(0);
}

int dummyfpcueoff(void)
{
	dx_position_window(0, 0, 0, 0, 3);
	dx_position_window(0, 0, 0, 0, 4);
	return(0);
}

int dummytgt(void)
{
	/* turn on the dummy window on rex to indicate tgt on, also turn off dummy fp win */
	dx_position_window(5, 5, 1, 0, 4);
	dx_position_window(0, 0, 0, 0, 3);
	return(0);
}


int dotsonFPchange(void)
{

	int dxtarget_indices[1] = {0};
	
	/* change FP CLUT value to menu CLUT#14 */

	dx_set_by_indexIV(DXF_D1, 0, 1, dxtarget_indices, 
4, 14 , ENDI);
	
	/* addded by Long, 03-29-07, get clock time for rough measure of RT */
	
	timego = getClockTime();
	
	/* added by Long, 3-30-07, set timer for minimal delay to reward */

	timer_set1(0, 0, 0, 0, TTMV("delay2rew"), 0);
	timer_set2(0, 0, 0, 0, TTMV("delay2rew") + currentrew + TTMV("errorFB"), 0);

	return(0);
}
int fun_rewardon(long code)
{
	valtype dv;
	if (code>0)
	 	ec_send_code(code);
	if((dv=DIV("Reward_bit"))>=0) 
	{
		dio_on(PR_DIO_ID(dv));
		ec_send_dio(dv);
	}
	return(0);
/*	 PR_DIO_ON("Reward_bit")
;	*/
}


int fun_rewardoff(long code)
{
	valtype dv;
	if (code>0)
	 	ec_send_code(code);
	if((dv=DIV("Reward_bit"))>=0) 
	{
		dio_off(PR_DIO_ID(dv));
		ec_send_dio(dv);
	}
	return(0);
/*	 PR_DIO_OFF("Reward_bit");	*/
}

int getsactime(void)
{
	timesac = getClockTime();
}

int correctfun(void)
{
		// printf("%d %d %d\n", timego, timesac, timesac-timego);
		return( pr_score_trialRT(kCorrect, currentrew, 0, timesac-timego, 0, 0, 1) );
 
}


/* THE STATE SET 
*/
%%
id 1000
restart rinitf
main_set {
status ON
begin	first:
		to prewait
   /*
   ** First wait time, which can be used to add
   ** a delay after reset states (e.g., fixation break)
   ** timer_set: (in timerLT.c) a function to set a timer from an exponential distribution
   **		of values. 5 such timers are currently available. 
   ** 	e.g., timer_set1 sets the first timer 
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
	/*
    ** pr_start_trial: (in paradigm_rec.c) run get_trial and send start ecodes and task and trial ecodes to PLEXON 
    ** regular task event ecodes are not tagged. Task and trial ecodes are tagged, send first the tag then the value. 
    */
	go:
		do pr_toggle_file(1)
		to trstart
	trstart:
		to fpshow on MET % start_trial
		to loop	

	fpshow:
		do dx_show_fp(FPONCD, 0, 3, 3, FPCOLOR_INI, FPCOLOR_INI)		
		rl 5
		to caljmp on DX_MSG % dx_check
	
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

	fpwinpos:
		time 20  /* takes time to settle window */
		do dx_position_window(60, 60, 0, 0, 0)
 		to fpwait
	fpwait:
 		time 5000
		to fpset on -WD0_XY & eyeflag
		to fpnofix
	fpnofix:    /* failed to attain fixation */
		time 2500
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

	/* Jump to task-specific statelists
	*/
	taskjmp:
		to t1fp on 0 % pr_get_task_index	
		to t2fp on 1 % pr_get_task_index
		to t3fp on 2 % pr_get_task_index	
		to t4fp on 3 % pr_get_task_index	
		to t5fp on 4 % pr_get_task_index	
		to badtask
	badtask:
		do show_error(0)
		to finish

	/* TASK 1: calibrate the ASL eye tracker  */
	t1fp:
		do dx_show_fp(FPCHG, 0, 5, 5, FPCOLOR_ASL, FPCOLOR_ASL);
		rl 10
		to t1wait1 on DX_MSG % dx_check
	t1wait1:
		do timer_set1(1000, 100, 600, 200, 0, 0)
 		to t1winpos on MET % timer_check1
	t1winpos:
		time 20
		do dx_position_window(20, 20,-1,0,0)
 		to correctASL

	/* TASK 2: memory-guided saccade	*/
	t2fp:
		do dx_show_fp(FPCHG, 0, 3, 3,  FPCOLOR_MGS,  FPCOLOR_MGS)
		rl 10
		to start_mgs_state on DX_MSG % dx_check
	start_mgs_state:
		do start_mgs()
		to precue
	precue:
		to fixbreak on +WD0_XY & eyeflag
		to cueon
	cueon:
		do dx_toggle2(TARGONCD, 1, 0, 0, 1, 1000)
		rl 20
		to cuedur on DX_MSG % dx_check
	cuedur:
		time 100
		do dummycue()
		to fixbreak on +WD0_XY & eyeflag
		to cueoff
	cueoff:
		do dx_toggle2(TARGOFFCD, 0, 0, 0, 1, 1000)
		to delay on DX_MSG % dx_check
	delay:
		to fixbreak on +WD0_XY & eyeflag
		to fixoffgo
	fixoffgo:
		do dx_toggle2(FPOFFCD, 0, 0, 1000, 0, 1000)
		rl 40
		to waitforsac on DX_MSG % dx_check
	waitforsac:
		do dummyfpcueoff()
		to sacon on +WD0_XY & eyeflag
		to errfeedback
	sacon:
		do ec_send_code_lo(SACMAD)
		rl 50
		to tgtacq on -WD1_XY & eyeflag 		
		to errfeedback
	tgtacq:	
		do ec_send_code_lo(TRGACQUIRECD)
		to fixbreak on +WD1_XY & eyeflag
		to cuefeedback
	cuefeedback:
		do dx_toggle2(FDBKONCD, 1, 0, 0, 1, 1000)
		to rewarddelay on DX_MSG % dx_check
	rewarddelay:
		to rewardon 
/* for the UW water feeder, toggle method for computer control */
	rewardon:
		rl 60
		do fun_rewardon(REWCD)
		to rewbitoff1
	rewbitoff1:
		do fun_rewardoff(0)
		to rewbiton2
	rewbiton2:
		do fun_rewardon(REWOFFCD)
		to rewardoff
	rewardoff:
		do fun_rewardoff(0)
		to correct


/* for the Crist reward box, follower method for computer control */
/*	rewardon:
		do fun_rewardon(REWCD)	
		to rewardoff
	rewardoff:
		do fun_rewardoff(REWOFFCD)
		to correct
*/		
		
	errfeedback:
		do dx_toggle2(FDBKONCD, 1, 0, 0, 1, 1000)
		to errfeedbackoff on DX_MSG % dx_check
	errfeedbackoff:
		time 100
		to error 


	/* TASK 3: visually-guided saccade	*/
	t3fp:
		do dx_show_fp(FPCHG, 0, 3, 3,  FPCOLOR_VGS,  FPCOLOR_VGS)
		rl 10
		to start_vgs_state on DX_MSG % dx_check
	start_vgs_state:
		do start_vgs()
		to pretgt
	pretgt:
		to fixbreak on +WD0_XY & eyeflag
		to tgton
	tgton:
		do dx_toggle2(TARGONCD, 2, 0, 0, 1, 1000)
		rl 40
		to refrain_vgs on DX_MSG % dx_check
	refrain_vgs:
		time 50
		do dummytgt()
		to fixbreak on +WD0_XY & eyeflag
		to waitforsac_vgs	
	waitforsac_vgs:
		to sacon_vgs on +WD0_XY & eyeflag
		to ncerr
	sacon_vgs:
		do ec_send_code_lo(SACMAD)
		rl 50
		to tgtacq_vgs on -WD1_XY & eyeflag
 		to errfeedback
	tgtacq_vgs:	
		time 150
		do ec_send_code_lo(TRGACQUIRECD)
		to fixbreak on +WD1_XY & eyeflag
		to rewarddelay_vgs
	rewarddelay_vgs:
		to rewardon 


	/* TASK 4: dots RT */
	t4fp:
		do dx_show_fp(FPCHG, 0, 3, 3,  FPCOLOR_DOT,  FPCOLOR_DOT)
		rl 10
		to start_dots_state on DX_MSG % dx_check
	start_dots_state:
		do start_dots()
		to t4wait1 
	t4wait1:
		do timer_set1(0, 0, 0, 0, 500, 0)
		to fixbreak on +WD0_XY & eyeflag
		to target2on on MET % timer_check1
	target2on:
		do dx_toggle2(TARGONCD, 1, 1, 1000, 2, 1000)
		rl 20
		to dummytgton on DX_MSG % dx_check
	dummytgton:	
		do dummycue()
		to t4wait2
	t4wait2:
		do timer_set1(1000, 300, 3000, 1500, 0, 0)
		to fixbreak on +WD0_XY & eyeflag
 		to dotson on MET % timer_check1
	dotson:
		do dx_toggle2(GORANDCD, 1, 0, 0, 3, 1000)
		rl 30
		to fpafterdots_dots on DX_MSG % dx_check
	fpafterdots_dots:
		do dotsonFPchange()
		to refrain_dots on DX_MSG % dx_check
	refrain_dots:
		time 50
		do dummytgt()
		to fixbreak on +WD0_XY & eyeflag
		to waitforsac_dots
	waitforsac_dots:
		to sacon_dots on +WD0_XY & eyeflag
		to ncerr on MET % timer_check2
	sacon_dots:
		do dx_toggle2(SACMAD, 0, 0, 1000, 3, 1000)
		rl 50
		to wait4hit_dots on DX_MSG % dx_check
	wait4hit_dots:
		do getsactime()
		to correct_dots on -WD1_XY & eyeflag
		to incorrect_dots on -WD2_XY & eyeflag
		to ncerr
	correct_dots:
		time 500
		do ec_send_code(TRGACQUIRECD)
		to fixbreak on +WD1_XY & eyeflag
		to targsoff
	targsoff:
	   do dx_toggle2(TARGOFFCD, 0, 1, 1000, 2, 1000)	
		to rewarddelay_dots on DX_MSG % dx_check	
	rewarddelay_dots:
		to rewardon on MET % timer_check1
	incorrect_dots:
		time 500 
		do ec_send_code(TRGACQUIRECD)
		to fixbreak on +WD2_XY & eyeflag	
		to errfeedback_dots 
	errfeedback_dots:
		do dx_toggle2(TARGOFFCD, 0, 1, 1000, 2, 1000)
		to bothtgtoff on DX_MSG % dx_check
	bothtgtoff:
		to error on MET % timer_check2 

	/* TASK 5: regular dots */
	t5fp:
		do dx_show_fp(FPCHG, 0, 3, 3,  FPCOLOR_REGDOT,  FPCOLOR_REGDOT)
		rl 10
		to start_dotsreg_state on DX_MSG % dx_check
	start_dotsreg_state:
		do start_dotsreg()
		to t5wait1 
	t5wait1:
		do timer_set1(0,0,0,0,500,0)
		to fixbreak on +WD0_XY & eyeflag
		to tgtson_reg on MET % timer_check1
	tgtson_reg:
		do dx_toggle2(TARGONCD, 1, 1, 1000, 2, 1000)
		rl 20
		to delay2dots_reg on DX_MSG % dx_check
	delay2dots_reg:
		do timer_set1(1000, 100, 600, 150, 0, 0)
		to fixbreak on +WD0_XY & eyeflag
		to dotson_reg on MET % timer_check1
	dotson_reg:
		do dx_toggle2(GORANDCD, 1, 0, 0, 3, 1000)
		rl 30
		to viewtimer on DX_MSG % dx_check
	viewtimer:
		do timer_set1(1000,100,1500,300,0,0)
		to fixbreak on +WD0_XY & eyeflag
		to dotsoff_reg on MET % timer_check1
	dotsoff_reg:
		do dx_toggle2(ENDCD, 0, 0, 0, 3, 1000)
		to fixbreak on +WD0_XY & eyeflag
		to delay2fixoff_reg on DX_MSG % dx_check
	delay2fixoff_reg:
		do timer_set1(0, 0, 0, 0, 500, 0)
		to fixbreak on +WD0_XY & eyeflag
		to fixoff_reg on MET % timer_check1
	fixoff_reg:
		do dx_toggle2(FPOFFCD, 0, 0, 1000, 3, 0)
		rl 40
		to waitforsac_dotsreg on DX_MSG % dx_check
	waitforsac_dotsreg:
		to sacon_dotsreg on +WD0_XY & eyeflag
		to ncerr
	sacon_dotsreg:
		do ec_send_code(SACMAD) 
		rl 50
		to wait4hit_dotsreg 
	wait4hit_dotsreg:
		to correct_dotsreg on -WD1_XY & eyeflag
		to incorrect_dotsreg on -WD2_XY & eyeflag
		to ncerr
	correct_dotsreg:
		time 150
		do ec_send_code(TRGACQUIRECD)
		to fixbreak on +WD1_XY & eyeflag
		to rewarddelay_dotsreg
	rewarddelay_dotsreg:
		to rewardon
	incorrect_dotsreg:
		time 150
		do ec_send_code(TRGACQUIRECD)
		to fixbreak on +WD2_XY & eyeflag
		to errfeedback_dotsreg
	errfeedback_dotsreg:
		do dx_toggle2(FDBKONCD, 0, 1, 0, 2, 1000)
		to bothtgtoff_dotsreg on DX_MSG % dx_check
	bothtgtoff_dotsreg:
		to error  

	
	/* OUTCOME STATES
	** NCERR (No-Choice Error)
	**	ERROR
	** CORRECT
	*/


	/* fixation break */
	fixbreak:
		do dummyfpcueoff()
		to fixbreak_score
	fixbreak_score:
		time 2500
		do pr_score_trial(kBrFix,0,0,0,0,1)
		to finish

	/* no choice */
	ncerr:
		time 1000
		do pr_score_trial(kNC, 0, 0, 0, 0, 1)
		to finish

	/* error */
	error:
		time 1
		do pr_score_trial(kError, 0, 0, 0, 0, 1)
		to finish

	/* pref -- reward! */
	correct:
		do correctfun()
		to finish 
	correctASL:
		do pr_score_trial(kCorrect, 2, 0, 0, 0, 1)
		to finish on 0 % pr_give_reward 

	finish:
		do pr_finish_trial()
		rl 0
		to wait2nexttrial
	wait2nexttrial:
		do dummyfpcueoff()
		to loop
		
abort list:		
	finish
}

