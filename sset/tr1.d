/* Training.d
	Long Ding 8-20-2008
	for training naive monkeys
*/



#include "rexHdr.h"
#include "paradigm_rec.h"
#include "toys.h"
#include "lcode.h"

/* PRIVATE data structures */

/* GLOBAL VARIABLES */
static _PRrecord  gl_rec = NULL; /* KA-HU-NA */
long currentrew= 0;

/* for now, allocate these here... */
MENU      umenus[30];
RTVAR     rtvars[15];
USER_FUNC ufuncs[15];

/* MACROS for memory+visual sac task */
#define TTMV(n)   pl_list_get_v(gl_rec->trialP->task->task_menus->lists[0],(n))
#define TPMV(n)   pl_list_get_v(gl_rec->trialP->task->task_menus->lists[1],(n))

#define WIND0 0      /* window to compare with eye signal */
#define WIND1 1      /* window for correct target */
#define WIND2 2      /* window for other target in dots task */
   /* added two dummy window to signal task events in rex window */
#define WIND3 3      /* dummy window for fix point */
#define WIND4 4      /* dummy window for target */
#define EYEH_SIG  0
#define EYEV_SIG  1

#define FPCOLOR_INI  15
#define FPCOLOR_ASL  1
#define FPCOLOR_MGS  2
#define FPCOLOR_VGS  3
#define FPCOLOR_DOT  4
#define FPCOLOR_REGDOT 1
#define REWINDEX     0
#define ANGLEINDEX   1

/* INITIALIZATION */

void autoinit(void)
{
   gl_rec = pr_initV(0, 0,
      umenus, NULL,
      rtvars, pl_list_initV("rtvars", 0, 1,
                  "currentrew", 0, 1.0,
                  "totalrew", 0, 1.0,
                  NULL),
      ufuncs,
      "asl", 1,
      "mgs", 3,
      NULL);
}

void rinitf(void)
{
   static int first_time = 1;

   if (first_time) {
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


/* START TRIALS */

int start_trial(void)
{
   int task_index;
   if ( !( pr_start_trial() ) ) {
      pr_toggle_file(0);
      return(0);
   }
   task_index = pr_get_task_index();

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

int start_mgs(void)
{
   int bigrew;
   int angle0, angle_cur;

   long tarwin;

   /* set basic task timing */

   set_times("precue", TTMV("precue"), -1);
   set_times("delay", TTMV("delay"), -1);
   set_times("waitforsac", TTMV("wait4sacon"), -1);
   set_times("sacon", TTMV("wait4hit"), -1);
   set_times("tgtacq", TTMV("gap2feedback"), -1);
   set_times("rewarddelay", TTMV("delay2rew"), -1);

   /* always equal reward */
   bigrew = TPMV("bigreward");
   currentrew = bigrew;

   /* turn on the target window */
   tarwin = TPMV("targetwin");
   dx_position_window(tarwin, tarwin, 1, 0, 1);
   dx_position_window(tarwin, tarwin, 1, 0, 2);
   /* turn on the dummy window on rex to indicate fix on */
   dx_position_window(10, 10, 0, 0, 3);
   /* start checking fixation break */
   dx_set_fix(1);

   /* set rtvars */
   pr_set_rtvar("currentrew", currentrew);


   return(0);
}

/* ROUTINE: do_calibration
**
** Returns 1 if doing calibration (ASL task)
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

/* SET DUMMY WINDOWS */

int dummy_cue(void)
{
   /* turn on the dummy window on rex to indicate cue on */
   dx_position_window(5, 5, 0, 1, 4);
   return(0);
}

int dummy_fpcueoff(void)
{
   dx_position_window(0, 0, 0, 0, 3);
   dx_position_window(0, 0, 0, 0, 4);
   return(0);
}

int dummy_fpoff(void)
{
   dx_position_window(0, 0, 0, 0, 3);
   return(0);
}

int dummy_tgt(void)
{
   /* turn on the dummy window on rex to indicate tgt on, also turn off dummy fp win */
   dx_position_window(5, 5, 1, 1, 4);
   dx_position_window(0, 0, 0, 1, 3);
   return(0);
}

/* DELIVER REWARD & UPDATE PERFORMANCE */

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
      pr_score_trialRT(kCorrect, 0, 1, 0);
      return(0);
}

/* STATE SETS */

id 1001
restart rinitf
main_set {
status ON
begin first:
      to prewait
   prewait:
      do timer_set1(0,100,600,200,0,0)
    	to loop on +MET % timer_check1
   loop:
      time 1000
      to pause on +PSTOP & softswitch
      to go
   pause:
      to go on -PSTOP & softswitch
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

   /* Jump to task-specific statelists */
   taskjmp:
      to t1fp on 0 % pr_get_task_index 
      to t2fp on 1 % pr_get_task_index
      to t3fp on 2 % pr_get_task_index 
      to t4fp on 3 % pr_get_task_index
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

   /* TASK 2: FIXATION TASK */
   t2fp:
      do dx_show_fp(FPCHG, 0, 3, 3,  FPCOLOR_MGS,  FPCOLOR_MGS)
      rl 10
      to start_t2 on DX_MSG % dx_check
   start_t2:
      time 1000
      to reward_delay
   reward_delay:
      time 1000
      to reward_on
   reward_on:
      do set_reward()
      to correct on 0 % pr_beep_reward

   /* TASK 3: WITHHOLDING VISUALLY GUIDED SACCADE UNTIL FIXOFF */
   t3fp:
      do dx_show_fp(FPCHG, 0, 3, 3,  FPCOLOR_MGS,  FPCOLOR_MGS)
      rl 10
      to start_t3 on DX_MSG % dx_check
   start_t3:
      do start_mgs()
      to precue_t3
   precue_t3:
      do timer_set1(0, 0, 0, 0, 1000, 0)
      to showcue_t3
   showcue_t3:
      do dx_toggle2(TARGONCD, 1, 0, 0, 1, 1000)
      rl 20
      to dummy_cue_t3 on DX_MSG % dx_check
   dummy_cue_t3:
      do dummy_cue()
      to cueon_t3
   cueon_t3:
      do timer_set1((0, 0, 0, 0, 1000, 0)
      to fpoff_t3
   fpoff_t3:
      do dx_toggle2(FPOFF, 0, 0, 1000, 1, 0)
      to dummy_fpoff_t3 on DX_MSG % dx_check
   dummy_fpoff_t3:
      do dummy_fpoff()
      to waitsac_t3
   waitsac_t3:
      time 1000
      do dx_set_fix(0)
      to sacon_t3 on +WD0_XY & eyeflag
      to nosac
   sacon_t3:
      to reward_delay_t3 on -WD1_XY & eyeflag
      to error
   reward_delay_t3:
      time 1000
      to reward_on

   correct:
      time 500
      to correctCD
   correctCD:
      do correctfun()
      to finish

   error:
      time 1
      do pr_score_trial(kError, 0, 1)
      to finish

   finish:
      do pr_finish_trial()
      rl 0
      to wait2nexttrial
   wait2nexttrial:
      do dummy_fpcueoff()
      to loop
	
	correct:
	   time 500
      to correctCD
   correctCD:
      do correctfun()
      to finish

	error:
    	time 1
    	do pr_score_trial(kError, 0, 1)
    	to finish

	finish:
		do pr_finish_trial()
		rl 0
		to wait2nexttrial
	wait2nexttrial:
		do dummy_fpcueoff()
		to loop

abort list:
	finish
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
      do dummy_fpcueoff()
      to efail_score
   efail_score:
      do pr_score_trial(kBrFix, 0, 1)
      to etest

abort list:
	etest
}
