/*	801
**
** Paradigm for the exploratory version of the probabilistic reward task
**	Written by dlb from 791
**	Written by jig & mn from 750, 780, 781
** YL 2012/09/13: correct target selection algorithm
** - also add WIND3 for what subject is SUPPPOSED to look at ... doesn't quite work
** YL 2012/12/18: task 1 and task 2 are both prob rew tasks (one for hi noise, one for low noise)
** DLB 04/11/14: Adapted for explorer task (date note: DD/MM/YY)
*/

#include "rexHdr.h"
#include "paradigm_rec.h"
#include "task.h"
#include "toys.h"
#include "lcode.h"
#include "sac.h"

/* PRIVATE data structures */

/* GLOBAL VARIABLES */
static _PRrecord 	gl_rec = NULL; 	/* KA-HU-NA */
static int 			sactest; 			/* var for the outcome of the saccade test */
static int 			target_correct; 	/* current trial's correct target */
static int 			isDone; 				/* flag for finishing the trial */

	/* for now, allocate these here... */
MENU 	 	 	umenus[30];
RTVAR		 	rtvars[15];
USER_FUNC 	ufuncs[15];

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
#define DONE		0x01
#define NOTDONE	0x02

/* ROUTINES */

/* ROUTINE: autoinit
**
**	Initialize gl_rec. This will automatically
**		set up the menus, etc.
**
**  TASK0: ASL calibration
**  TASK1: explorer task (hi noise, lo haz)
**  TASK2: explorer task (lo noise, hi haz)
*/
void autoinit(void)
{
printf("autoinit start\n");
gl_rec = pr_initV(0, 0,
	umenus, 		NULL,
	rtvars, 		NULL,
	ufuncs,
	"asl",			1,
	"explorer", 	2,
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

		/* for the dummy window */
		wd_src_pos(WIND3, WD_DIRPOS, 0, WD_DIRPOS, 0);
		wd_src_check(WIND3, WD_SIGNAL, EYEH_SIG, WD_SIGNAL, EYEV_SIG);
		
/*		wd_siz(WIND3, 10, 10);
		wd_cntrl(WIND3, WD_ON); */
	
		/* initialize saccade detector */
		sd_set(1);

		/* init the screen */
		pr_setup();

	}
}

/* ROUTINE: start_trial
** This draws the targets etc.
*/
int start_trial(void)
{	register int i;
	int std,
		task_index = pr_get_task_index(),
		choice_diam = TIMV("choice_diam"),
		choice_clut = TIMV("choice_clut"),
		ang = gl_rec->trialP->task->trialPs_index, tind;
	double *probs=NULL, psum, randval;
	_PRtrial	   trial = gl_rec->trialP;
	_PLlist		dx_target = pl_group_get_list(gl_rec->dx->current_graphic, DX_TARGET);
	_exploreruser_data ud=(_exploreruser_data)(trial->task->user_data);

	#define BASE 7000 /* for dropping codes */		

	/* initialize working copies and actual copies of visible flags in DX_TARGET */
	for(i=2;i<ud->num_targets;i++) {
		/* set the actual visibility values to 0*/
		pl_list_set_vi(dx_target,DX_VIS,i,0);
		/* set the working copy visibility values to 1 */
		pl_list_set_wi(dx_target,DX_VIS,i,1);
	}

	/* no dynamic stimuli, so make draw_flag=3 the default.
	**	This draws each command ONCE
	*/
	dx_set_flags(DXF_D1);
	
	/*  
	** Set the rewarding target for this trial
	*/

	/* Compute reward probabilities
	**
	** jig and mn changed 10-05-10 to include task block menu
	**		descriptions of probability function
	**
	** IF "std" field is >0, use vonMises to compute distribution
	** ELSE use p* fields
	**	
	** May be best to use a probability distribution that ranges around the whole circumference
	*/
	std = BIMV("std", ud->block_menu_index);
	
	
	if (std > 0) {

		/* Call toy_vonMisesA to get array of probabilities */
		probs = toy_vonMisesA(ud->angles, ud->num_targets, 
				(double) pl_list_get_v(gl_rec->trialP->list,"angle"),
				1./pow( ((double) std)/10. ,2) );

		/* Drop code indicating std */
		ec_send_code_tagged(I_PRTSTD, BASE + std);

	} else {

		/* make from p* fields of task block menu 
		** Remember that task->trialPs_index is the index of angle
		**	 (the max-rewarded angle in the array )
		*/ 
		probs = SAFE_ZALLOC(double, ud->num_targets);
		for(i=0;i<((ud->num_targets)-2);i++) {
			tind = min(abs(ang-i), abs(((ud->num_targets)-2)-abs(ang-i)));
	   		probs[i] = gl_rec->trialP->task->task_menus->lists[1]->properties[10+tind]->values[0+ ud->block_menu_index]; 			
		/* THIS SHOULD ALREADY BE SENT... MRN commented out on cinco de mayo '11 */
		/*	ec_send_code_tagged(I_PRTSTD, BASE + probs[i]);  */
		}
	}

	/*	Normalize probabilities */
	for(i=0,psum=0;i<((ud->num_targets)-2);i++)
		psum+=probs[i];
	for(i=0;i<((ud->num_targets)-2);i++)
		probs[i]/=psum;
		
	/* Pick random number and compare to cumulative probability */
	randval = ((double) rand())/((double) RAND_MAX);
	psum	  = 0;
	for(i=0;i<((ud->num_targets)-2);i++)
		if ((psum+=probs[i]) >= randval)
			break;
	/* set the correct target to i; note that this is 2 less than the appropriate
	target in DX_TARGET due to f.p. and choice feedback */
	target_correct=i; 

	/* Clean up array of probabilities */
	SAFE_FREE(probs);
	
	return(0);
}

/* ROUTINE: show_error */
int show_error(long error_type)
{

	if(error_type == 0)
		printf("Bad task index (%d)\n", pr_get_task_index());

	return(0);
}

/* ROUTINE: show_targets 
**
** Updates the target visibilities from working copies and draws.
**
*/
int show_targets(long ecode)
{
	register 	int i;
	int			indices[10];
	_PRtrial	   trial = gl_rec->trialP;
	_exploreruser_data ud=(_exploreruser_data)(trial->task->user_data);

	/* make an array of indices for updating from the working copy */
	for(i=0;i<10;i++)
		indices[i] = i+2;

	/* set the actual value from the working copy */ 
	dx_setw_by_nameIV(0,DX_TARGET,10,indices,DX_VIS,NULL);
   	
	/* drop targ on code */
   	ec_send_code_hi(ecode);

	/* done */
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

/* ROUTINE: made_saccade
**
** Called after monkey makes saccade
**
*/
int made_saccade(long max_dist)
{
	register int i;
	int target_choice, std, *vi,
		choice_diam = TIMV("choice_diam"), 
		choice_clut = TIMV("choice_clut"), 
		ang = gl_rec->trialP->task->trialPs_index, tind;
	_exploreruser_data ud=(_exploreruser_data)(gl_rec->tasks[gl_rec->task_index]->user_data); 
	double psum, randval, eyeh_d = eyeh/40., eyev_d=eyev/40., 
			min_distance, distance, max_distance=max_dist/10;
	_PLproperty target_visible_property = 
			pl_group_get_prop(gl_rec->dx->current_graphic, DX_TARGET, DX_VIS);
	_PLlist		dx_target = pl_group_get_list(gl_rec->dx->current_graphic, DX_TARGET);
	valtype *tv_values 	 = target_visible_property->values+2;

	#define BASE 7000 /* for dropping codes */
	
	/* drop code */
	ec_send_code_hi(SACACC);
	
	/* use saccade endpoint to find closest target */
	min_distance  = 999.;
	target_choice = -999;
	for (i=0;i<((ud->num_targets)-2);i++) {
		if (tv_values[i]==1) {
			distance = sqrt(pow(eyeh_d-(ud->xs[i]),2) +
							pow(eyev_d-(ud->ys[i]),2));
			if (distance < min_distance) {
				min_distance  = distance;
				target_choice = i;
			}	
		}
	}
	
	/* update visibility working copies, sactest, isDone if within max distance of a targ */ 
	if (min_distance < max_distance) {
		pl_list_set_wi(dx_target,DX_VIS,target_choice+2,0);
		sactest=COR;
		if (target_choice == target_correct) {
			isDone = DONE;
		} else {
			isDone = NOTDONE;
		}	
	} else {
		target_choice = -1;
		sactest = NC;	
	} 

	/* set which target was chosen */	
	ud->chose_t0 = ((target_choice == ang) && (sactest == COR));

	/* Drop codes */
	ec_send_code_hi(FDBKONCD);
	ec_send_code_tagged(I_PRTTCHC, BASE+target_choice);
	if (sactest==COR) {
		ec_send_code_tagged(I_PRTTREW, BASE+target_correct);
	}

	/* outta */
	return(0);
}

/* ROUTINE: reward_trial
**
*/
int reward_trial(long beep_time)
{
	_exploreruser_data ud=(_exploreruser_data)(gl_rec->tasks[gl_rec->task_index]->user_data);
	long reward_time, reward_num; /* Establishes two vars, reward_time and reward_num, of type long (capable of being very large numbers) */

	/* update score */
	pr_score_trial(kCorrect, 0, 1);

	/* Set the isDone variable back to not done, for the next trial */
	isDone = NOTDONE;
 
	/* set reward params */
	if (ud->chose_t0) {
		reward_time = BIMV("rew0_time", ud->block_menu_index);
		reward_num  = BIMV("rew0_num",  ud->block_menu_index);
	} else {
		reward_time = BIMV("rew_time", ud->block_menu_index);
		reward_num  = BIMV("rew_num",  ud->block_menu_index);
	}

	/* User feedback */
	printf("reward_time = % d, reward_num = %d \n", reward_time, reward_num);

	/* if beep time <0, use reward time as beep time */
	pr_set_reward(
		(int) reward_num, 
		reward_time,
		PRV("Reward_off_time"),
		(int) reward_num,
		beep_time < 0 ? reward_time : beep_time,
		PRV("Reward_off_time"));
	return(0); 
}

/* THE STATE SET 
** Note that the root files, which are loaded in Rex before the run, can overwrite the input param values for each called routine
*/
%%
id 801
restart rinitf
main_set {
status ON
begin	first:
		to prewait

	prewait:
		do timer_set1(0,100,600,200,0,0) 
		to loop on +MET % timer_check1
		
	loop:
		do timer_set1(0,100,600,200,0,0)
		to pause on +PSTOP & softswitch
		to go on +MET % timer_check1
	pause:
		to go on -PSTOP & softswitch
	go:
		do pr_toggle_file(1)
		to trprep
	trprep:
		to trstart on MET % pr_start_trial
		to loop
	trstart:
		do start_trial()
		to fpshow
	fpshow:
		do dx_show_fp(FPONCD, 0, 1, 1, 1, 1)		
		to caljmp		
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
		do ec_send_code_hi(EYINWD) 	
		to fpwait on +WD0_XY & eyeflag
		to fpwin2
	fpwin2:
		time 20 /* again time to settle window */
		do dx_position_window(30, 30, 0, 0, 0)
		to taskjmp

	/* Jump to task-specific statelist */
	taskjmp:
		to t0start on 0 % pr_get_task_index
		to t1start on 1 % pr_get_task_index
		to t1start on 2 % pr_get_task_index
		to badtask
	badtask:
		do show_error(0)
		to finish

	/* TASK 0: ASL eye tracker calibration  */
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
	
		/* TASK: Explorer  */
	
	t1start: /* First wait period before showing the targets */
		do timer_set1(1000, 100, 300, 200, 500, 0)
		to fixbreak on +WD0_XY & eyeflag
 		to t1show1 on MET % timer_check1
	t1show1: /* show ring of dxtargets */
		do show_targets(TARGONCD)
		to t1wait2 on DX_MSG % dx_check
	t1wait2:	 /* wait again */
		do timer_set1(1000, 100, 300, 200, 500, 0)
		to fixbreak on +WD0_XY & eyeflag
 		to t1fpoff on MET % timer_check1
	t1fpoff: /* Turn off fixation point, signal monkey to make saccade */
		do dx_hide_fp(FPOFFCD)
		to grace on DX_MSG % dx_check
	t1show2: /* show reduced ring of dxtargets */
		do show_targets(TARGONCD)
		to grace

	/* Outcome states
	** fixbreak
	** ncerr (no choice error)
	** t1show2 (failed to find the rewarding target)
	** correct
	*/

	grace: /* Timing period */
		time 5000 /* in ms */
		to sacstrt on +WD0_XY & eyeflag
		to ncerr
	sacstrt:
		ec_send_code_hi(SACMAD);
		to sacset on +SF_GOOD & sacflags
		to sacset /* ncerr */
	sacset:
		time 20
		do dx_position_window(30, 30, -1, 0, 0) /* make the big window centered on the eye position */
		to sacwait
	sacwait:
		time 250
		to grace on +WD0_XY & eyeflag /* go to saccade start if bitwise and of WD0_XY and eyeflag */
		to sacdone /* otherwise go to saccade done */
	sacdone:
		do made_saccade(100) /* go to made saccade */
		to sacresult on +COR & sactest
		to ncerr /* should only go to ncerr if the previous test fails */
	sacresult:
		to t1show2 on +NOTDONE & isDone
		to correct on +DONE & isDone
	
	/* reward! */
	correct:
		do reward_trial(kCorrect)
	   	to finish on 0 % pr_beep_reward

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
