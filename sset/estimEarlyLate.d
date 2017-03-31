/* estimEarlyLate.d
     - Test psychometric function under 
       - No stim, Full stim, Early stim, and Late stim conditions
       - estim: 0, no stim; 1, full stim; 2, early stim, and 3, late stim
     
     - Use "duration" parameter in "Estim-Property" menu list to set the boundary of Early and Late
       phases. (so that the task c program will not have to be changed.)

     - No. I also have to change the task c program to increase # of estim conditions.

     2012-04-01 Created from estimRevCorr.d by Taka Doi
*/

/*  Taka_estim_RevCorr.d
          created from long2_estim.d

   Electrical stimulation will be applied randomly for the reverse correlation analysis during the
   dotsRT task. From the task menu, we can set 1) pre_start (pre duration before dots onset), 2)
   duration (the time width of a single epoch on which microstim is turned on or off), and 3) percent
   (probability of giving microstim in a given time window).
   
   Microstim is not delivered on some trials. This is controled by "estim" (0 for no stim, 1 for random
   pulses)
   
   When estim==1, estim_flag controls the start and end of the random microstim procedure. See the end
   of the script for some comments about this.
   
   The state chain "estim_set" is used for generating a random number that determins the presence or absence
   of microstim. The duration of a single time window is implemented as a time entry of a state
    "estim_wait".


  Sept-Oct 2011 Taka Doi

  ---- history ----
  2011-12-07 (Wed) at Rig1
   - Change the paradigm name from dotsrt_estimRC to estim_rc.
     - Saving to a root file doesn't work properly, and I guess the problem is a too long
       paradigm name...
     - This might fix the bug that the task stops after completing the first set of trial
       pointers.
 
  2012-02-02
   - Introduce standard, continuous estim condition. (estim: 0, no stim; 1, pulse stim; 2, standard stim) 

*/


#include "rexHdr.h"
#include "paradigm_rec.h"
#include "toys.h"
#include "lcode.h"

/* PRIVATE data structures */

/* GLOBAL VARIABLES */
static _PRrecord 	gl_rec = NULL; /* KA-HU-NA */
long currentrew  = 0;
long totalrew    = 0;
long timego      = 0;
long timesac     = 0;
long time_random = 0;

int  estim         = 0;     // 0 for no estim, 1 for estim. Specify corresponding to "estim" in the task_dotRT_stim_RevCorr.c
long estim_delay   = 0;     // Delay from the time when estim_flag is raised.
int  estim_flag    = 0;     // The flag controlling the initial and end time of noisy microstimulation period 
int  estim_percent = 0;     // Percent to apply a microstim in a given epoch

int  flag_omitreward = 0;
int  flag_estim      = 0;
int  flagTrialRepeat = 0;
int  prevtime = 10; /* for tracking viewing time used in the previous error or fixbreak trial */

	/* for now, allocate these here... */
MENU        umenus[30];
RTVAR       rtvars[15];
USER_FUNC   ufuncs[15];

/* MACROS for memory+visual sac task */

#define TTMV(n) 	pl_list_get_v(gl_rec->trialP->task->task_menus->lists[0],(n))
#define TPMV(n) 	pl_list_get_v(gl_rec->trialP->task->task_menus->lists[1],(n))
#define TSMV(n)   pl_list_get_v(gl_rec->trialP->task->task_menus->lists[2],(n))   /* estim property */
// estim property: pre_start, duration, and prob_estim.

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
#define FPCOLOR_DOT	4
#define ESTIMINDEX	0
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
/* printf("autoinit start\n"); */

	gl_rec = pr_initV(0, 0, 
		umenus, NULL,
		rtvars, pl_list_initV("rtvars",     0, 1, 
				          "currentrew", 0, 1.0,
				          "totalrew",   0, 1.0,	
				          "estim",      0, 1.0,
				          NULL),
		ufuncs,
		"asl",             1,
		"estim_el",        1,
		NULL);

      /* printf("autoinit end\n"); */
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
      set_times("fpshow_delay", 1, -1);
	
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

/* ROUTINE: start_dots
**
**	set times for states and set reward info used in dots task 
*/
int start_dots( void )
{
	int rew;
	int angle0, angle_cur;
	int coh, maxcoh;
      int p_omit;  /* prob of omitting reward in correct trials, in percent */
	int p_estim;

	long tarwin;
	/* set basic task timing */

      set_times("waitforsac_dots", TTMV("wait4sacon"), -1); // The 3rd arg is regarding rand entry of the state. If -1, this will be unchanged. (Taka 10/03/11)
      set_times("wait4hit_dots",   TTMV("wait4hit"),   -1); // Waiting time for recognizing the saccade to one of the targets.
      set_times("bothtgtoff",      TTMV("errorFB"),    -1);
      set_times("estim_early_wait", TSMV("duration"),  -1);
      set_times("estim_late_wait",  TSMV("duration"),  -1);
                              
	/* set reward size  */
	currentrew = TPMV("reward");

	/* turn on the target window */
	tarwin = TPMV("targetwin");
	dx_position_window(tarwin, tarwin, 1, 0, 1);
	dx_position_window(tarwin, tarwin, 2, 0, 2);

	/* turn on the dummy window on rex to indicate fix on */
	dx_position_window(10, 10, 0, 0, 3);
      /* start checking fixation break */
      dx_set_fix(1);

  
	/* do NOT omit reward */
 	/* self note: usage of PL_G2PVS: 1: list 1 for dots targets
												5:	property #5, as defined in dotsX.c
												0: the first dots target 
	*/
	flag_omitreward = 0;

	/* decide if/how to deliver electrical stimulation */
	estim = PL_L2PV(gl_rec->trialP->list, ESTIMINDEX); // estim: 0, no stim; 1, full stim; 2, early stim; 3, late stim

   /* set rtvars */
   pr_set_rtvar("currentrew", currentrew);
   pr_set_rtvar("estim", estim);
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
	dx_position_window(5, 5, 0, 1, 4);
	return(0);
}

int dummyfpcueoff(void)
{
	dx_position_window(0, 0, 0, 0, 3);
	dx_position_window(0, 0, 0, 0, 4);
	estim_flag = 0; 
	return(0);
}

int dummytgt(void)
{
	/* turn on the dummy window on rex to indicate tgt on, also turn off dummy fp win */
	dx_position_window(5, 5, 1, 1, 4);
	dx_position_window(0, 0, 0, 1, 3);
	return(0);
}

int dummydots(void)
{
	dx_position_window(4, 4, 0, 0, 3);
	return(0);
}

int dotsonFPchange(void)
{

	int dxtarget_indices[1] = {0};
	
	/* change FP CLUT value to menu CLUT#14 */

	dx_set_by_indexIV(DXF_D1, 0, 1, dxtarget_indices, 
				4, 4 , ENDI);
	
	/* addded by Long, 03-29-07, get clock time for rough measure of RT */
	
	timego = getClockTime();
	
	/* added by Long, 3-30-07, set timer for minimal delay to reward */

	timer_set1(0, 0, 0, 0, TTMV("delay2rew"), 0);
	timer_set2(0, 0, 0, 0, TTMV("delay2rew") + currentrew + TTMV("errorFB"), 0);

	return(0);
}

int tgton_time(void)
{
   timego = getClockTime();
	return(0);
}

///////// This function turns on estim ///////////////////////////
int fun_estimon(long code)
{
	valtype dv;

	if (code>0)
	 	ec_send_code(code);
	if((dv=DIV("Stim_on_bit"))>=0) 
	{
		dio_on(PR_DIO_ID(dv));
		ec_send_dio(dv);
	}
	
	return(0);
}

//
int fun_estimoff(long code)
{
	valtype dv, dv2;

	if (code>0)
	 	ec_send_code(code);
	if((dv=DIV("Stim_on_bit"))>=0)
	{
		dio_off(PR_DIO_ID(dv));
      if(dv2=DIV("Stim_off_bit"))
		  ec_send_dio(dv2);
	}

	return(0);
}
/* dv2 was created to send a different dio bit to Plexon.
** Actual stim off is done by dv(Stim_on_bit).
**  Taka Doi 11/09/2011
*/
///////////////////////////////////////////////////////////////////



int getsactime(void)
{
	timesac    = getClockTime();
	estim_flag = 0;

	return(0);
}

// Calculate estim_delay (delay from target onset to estim onset) and raise up estim_flag. (Taka 10/11/11)
int estimcheck_pre(void)
{
      dummycue;

      if(estim){
         int startT = TSMV("pre_start"); 
         // "pre_start" must be a negative value. 
         // Its absolute value must be < minimum possible value for time_random.
               
         estim_delay = time_random + startT;
         // "time_random" is a global variable (Taka 10/03/11)
         // , wihch indicate a randomly sampled duration from target onset to dots onset.
            
         if(estim_delay<0) estim_delay = 1;

         /*if(estim==1){
         	estim_percent = TSMV("percent"); // pulse 
         }else if(estim==2){
         	estim_percent = 100;             // continuous
	      }*/
     
         estim_flag = 1;
      }
   
      return(0);
}

int set_reward(void)
{
	int i = 0;
	int rewnum = 0;
	i = pr_get_task_index();
	if (i==4)
	{	
   	rewnum = TPMV("reward_num");
		pr_set_reward(rewnum, currentrew, pl_list_get_v(gl_rec->prefs_menu, "Reward_off_time"), -1, 0, 0);
		totalrew = totalrew + rewnum * currentrew;
	}
	else
	{ 
		pr_set_reward(1, currentrew, pl_list_get_v(gl_rec->prefs_menu, "Reward_off_time"), -1, 0, 0);
		totalrew = totalrew + currentrew;
	}
  	pr_set_rtvar("totalrew", totalrew);

	return(0);
}

int correctfun(void)
{
		printf(" %d ", timesac-timego);
      // return( pr_score_trialRT(kCorrect, currentrew, 0, timesac-timego, 0, 0, 1) );
		pr_score_trialRT(kCorrect, 0, 1, timesac-timego);
		// pr_set_reward(1, currentrew, 1, -1, 0, 0); 
		flagTrialRepeat = 0;
		set_times("extra_delay_dots", 1 ,-1 );
		return(0);

}


int errorfun(void)
{
	int delay_by_RT = 1;
 	printf("error RT %d ", timesac-timego);
	pr_score_trial(kError, 0, 1);
	flagTrialRepeat = 0;
	if (TPMV("flagRepeat")==1)
		flagTrialRepeat = 1;

/*	if ( (TPMV("flagRepeat")==2 )
			&& ( (timesac-timego)<800 )
			&& ( abs( PL_L2PV(gl_rec->trialP->list, ANGLEINDEX) )<100 ) )
		flagTrialRepeat = 1;
*/
/*	if (timesac-timego<1000)	
 		set_times("extra_delay_dots", (TPMV("extra_delayRT")-timesac+timego)*TPMV("extra_delayslope"),-1 );
*/
}


int rew_by_RT( long slope1, long baseRT1, long slope2, long baseRT2, long minRew, long maxRew)
{
/* note: input slope should be 10 times the intended slope */

	double baseRT, tempRew;
	int flag_asymRew;
	int angle0, angle_cur;	
	int slope;

	flag_asymRew = 2;

	if ( flag_asymRew==2 && slope1+slope2>0 ) 
	{	
	  	angle_cur = PL_G2PVS(gl_rec->dx->current_graphic, 1, 6, 0);
     	angle0 = TPMV("Angle_o");
		if (angle_cur == angle0)
		{
			baseRT = baseRT1;
			slope  = slope1;
		}
		else
      {
         baseRT = baseRT2;
			slope  = slope2;
		}
		tempRew = (double)currentrew + (timesac - timego - baseRT) * slope / 10.0;		
		currentrew = (int)tempRew;
		if (tempRew<minRew)	currentrew = minRew;
		if (tempRew>maxRew) 	currentrew = maxRew;
/*		set_times("rewardon", currentrew, -1); */
		pr_set_rtvar("currentrew", currentrew);
	}

	return(0);		

}

int get_timer_times_LD(long index, long probability, long min_time,
              long max_time, long mean_time,
              long override_time, long override_random)
{
	time_random = timer_set1(probability, min_time, max_time, mean_time,
              override_time, override_random);
	return(0);
}

int set_estim_delay_timer(void)
{
	timer_set3(0, 0, 0, 0, estim_delay, 0);
   return(0);
}

int etim_on_or_off(void)
{
 return(TOY_RCMP(estim_percent*10));
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
		time 1000    /* This value is shown and can be accessed from States menu.*/
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
            /* Toggle the 8th TTL
               When 8th TLL switch (on the panel) is at mid position (not up or not down), the control from Rex is effective.
            */
	trstart:
		to fpshow_delay on MET % start_trial
		to loop	
	fpshow_delay:
		to fpshow
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
		do pr_score_trial(kNoFix,0,1)
		to finish
	fpset:
		time 50 /* give gaze time to settle into place (fixation) */
		do ec_send_code(EYINWD)
		to fpwait on +WD0_XY & eyeflag
		to fpwin2
	fpwin2:
		time 20 /* again time to settle window */
		do dx_position_window(40, 40, 0, 1, 0)
		to taskjmp

	/* Jump to task-specific statelists
	*/
	taskjmp:
		to t1fp on 0 % pr_get_task_index	
		to t4fp on 1 % pr_get_task_index
		to t4fp on 2 % pr_get_task_index
		to badtask
            /*
            Probably I will change TASK 4
            */
	badtask:
		do show_error(0)
		to finish

	/* TASK 1: calibrate the ASL eye tracker  */
	t1fp:
		do dx_show_fp(FPCHG, 0, 5, 5, FPCOLOR_ASL, FPCOLOR_ASL)
		rl 10
		to t1wait1 on DX_MSG % dx_check
	t1wait1:
		do timer_set1(1000, 100, 600, 200, 0, 0)
 		to t1winpos on MET % timer_check1
	t1winpos:
		time 20
		do dx_position_window(20, 20,-1,0,0)
 		to correctASL
	
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
		to target2on on MET % timer_check1
	target2on:
		do dx_toggle2(TARGONCD, 1, 1, 1000, 2, 1000)
		rl 20
		to t4wait2 on DX_MSG % dx_check
      t4wait2:
		do get_timer_times_LD(0,1000, 200, 3000, 500, 1, 1) 
		to dummytgton
	dummytgton:
		do estimcheck_pre()                /* estim_delay is calculated. estim_flag is up.*/
      to fixbreak on +WD0_XY & eyeflag	
		to dotson on MET % timer_check1    /* Wait for a few hundred millisec. (Taka 10/03/11) */
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
		do dx_set_fix(0)		
		to sacon_dots on +WD0_XY & eyeflag
		to ncerr      on MET % timer_check2
	sacon_dots:
		do dx_toggle2(SACMAD, 0, 0, 1000, 3, 1000)
		rl 50
		to wait4hit_dots  on DX_MSG % dx_check
	wait4hit_dots:
		do getsactime()
		to correct_dots   on -WD1_XY & eyeflag
		to incorrect_dots on -WD2_XY & eyeflag
		to ncerr
	correct_dots:
		do dx_toggle2(TRGACQUIRECD, 0, 1, 0, 2, 1000)
		to delay_correct_dots on DX_MSG % dx_check
	delay_correct_dots:
		time 400	
    	      to ncerr on +WD1_XY & eyeflag
		to correcttgtfix 
	correcttgtfix:
	   do dx_toggle2(TARGOFFCD, 0, 1, 0, 2, 1000)	
		rl 60	
		to rewarddelay_dots on DX_MSG % dx_check	
      rewarddelay_dots:
            to rewardon
      rewardon:
            do set_reward()
            to correct on 0 % pr_beep_reward
      incorrect_dots:
     	      do dx_toggle2(TRGACQUIRECD, 0, 1, 1000, 2, 0)
	      to delay_incorrect_dots on DX_MSG % dx_check
      delay_incorrect_dots:
	      time 400 
   	      to ncerr on +WD2_XY & eyeflag
	      to errtgtfix 
	errtgtfix:
		do dx_toggle2(FDBKONCD, 2, 1, 1000, 2, 0)
		rl 60
		to delay_errtgtfix on DX_MSG % dx_check
	delay_errtgtfix:
		time 400
		to bothtgtoff_dots 
	bothtgtoff_dots:
		do dx_toggle2(TARGOFFCD, 0, 1, 1000, 2, 1000)	
		to error_dots on DX_MSG % dx_check
	error_dots:
		do errorfun()
		to finish_dots 
	finish_dots:
           do pr_finish_trial()
           rl 0
           to wait2nexttrial

	
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
		do pr_score_trial(kBrFix,0,1)
		to finish_fixbreak
	finish_fixbreak:
		time 2500	
		do pr_finish_trial()
		rl 0
		to wait2nexttrial
	
	/* no choice */
	ncerr:
		do dummyfpcueoff()
		to ncerr_score
	ncerr_score:
		do pr_score_trial(kNC, 0, 1)
		to finish_ncerr
	finish_ncerr:
		time 1000
		do pr_finish_trial()
		to wait2nexttrial


	/* error */
	error:
		time 1
		do pr_score_trial(kError, 0, 1)
		to finish

	/* correct */
	correct:
		time 500
		to correctCD
	correctCD:
		do correctfun()
		to finish 


	correctASL:
		do pr_score_trial(kCorrect, 0, 1)
		to rewardASL
	rewardASL:
		do pr_set_reward(1,100,1,1,100,1)		
		to finish on 0 % pr_beep_reward 

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


/* set to check for fixation bread during task...
** use set_eye_flag to set gl_eye_state to non-zero
** to enable loop */
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
      do dummyfpcueoff()
      to efail_score
   efail_score:
      to etest
abort list:
}


/**************************************************
******* Early of microstimulation *********
**************************************************/
estim_set {
status ON
      begin estim_rest:
            to estim_initdelay  on 1 = estim_flag
      estim_initdelay:
            do set_estim_delay_timer()        /* to aligne at dots onset */
            to estim_condition on MET % timer_check3
            to estim_abort     on   0 = estim_flag 
      estim_condition:
            to estim_full_on    on 1 = estim
            to estim_early_on   on 2 = estim
            to estim_late_wait  on 3 = estim
      estim_full_on:
            do fun_estimon(STIMCD)
            rl 110   
            to estim_abort on 0 = estim_flag 
      estim_early_on:
            do fun_estimon(STIMCD)
            rl 110   
            to estim_early_wait
      estim_early_wait:
            time 500                /* override by set_times() (Taka) */
            to estim_abort on 0 = estim_flag
            to estim_early_off
      estim_early_off:
            do fun_estimoff(STOFFCD)
            to estim_rest on 0 = estim_flag
      estim_late_wait:
            time 500                /* override by set_times() (Taka) */
            to estim_abort on 0 = estim_flag 
            to estim_late_on
      estim_late_on:
            do fun_estimon(STIMCD)
            rl 110   
            to estim_abort on 0 = estim_flag 
      estim_abort: 
		      do fun_estimoff(STOFFCD)
            rl 85 
		      to estim_rest
abort list:
}


/* "estim_flag"
     How the flag is controled in the main, eye, and estim state chains
 	- Up
         - estimcheck_pre()

      - Down
         - dummyfpcueoff(), which is called from the following states
            - "fixbreak" "ncerr" "wait2nexttrial"
            - "efail"
         - getsactime(), which is called from the following state
            - wait4hit_dots:
    
    Taka (10/11/11)
*/

/* 
  int start_dots( void )
  - Set the duration of single time window by reading the corresponding task menu entry
  - Set "estim"
  - Set the probability of estim at a given time window
   Taka (10/11/11)

/* Ecodes for estim on and off
      Each time estim dio is turned on or off (these occur at the beginning of each time window),
      send respective ecodes and increment them from the base values.
*/


/*	long2_estim.d
**		modified from long2.d Aug 2010 LD
** 	definitions of stimulation: 
**			pre epoch: estimcheck_pre; start/end is relative to dots onset; if start>0 or start earlier than random time, start from tgton 		
**			dots epoch: dotsonFPchange; start/end is relative to dots onset; if end<0, end at sacon(getsactime)
**			postsac :	getsactime; start/end is relative to sac onset; if end<0, end at reward onset(set_reward)
**			reward:		set_reward; start/end is relative to rew onset; 
**			predots-sac:	percent_pre=-1 and percent_dots=-1; from tgton to sacon
**			postsac-reward: percent_post=-1 and percent_rew=-1 from sacon to rewardon+postrew_end
** 	paradigm for Long's use. Includes dot RT task with or without estim
**	Symmetric reward version only
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
**	modified to accomodate changes in paradigm_rec.c Long 9-13-07
**		use the new subroutine in paradigm_rec.c to give reward for both ASL and other tasks.
**	  	no longer set reward DIO in states
**		
**	Long Ding 2008-04-16. before today, several ecodes were not dropped because dx_check function was not called.
** 		modified to drop TRGACQUIREDCD and FDBKONCD. For data collected before, use SACMAD as an equivalent event 
**  			plus appropriate delays
**	Long Ding 2008-09-10	Added the option to withheld reward in easy trials. randomly omit reward in 
**			VGS, MGS, and high coh dots trials
** Long Ding 2009-05-19 changed Rew_by_RT function to separately set slope for the two directions
** Long Ding 2009-06-04 added option to deliver estim during dots viewing in dotsRT task only
** Long Ding 2009-06-30 changed dotsreg task to repeat previous viewing time for fixbreak or error+flagRepeat
*/
