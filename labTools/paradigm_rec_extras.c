/*
** PARADIGM_REC_EXTRAS.C
**
** Basically a bunch of junk I found in paradigm_rec that people had added but probably aren't used
**		any more. I'm just cutting and pasting them here, in case they are needed.
**
** Created by jig 11/10/16
*/

/* ** FROM PARADIGM_REC.H: ** */

/* A public routine prototype for estimPostSacTD.d , added by TD on 06-21-12 */
/* 1 if a specified number of trials are finished for each stimulus, 0 otherwise. */
int         pr_change_block            (int);
int         current_block;
int         finished_flag;   /* whether at least single trial is finished */

/* ** FROM PARADIGM_REC.C: ** */

/* PUBLIC ROUTINE: pr_change_block
**
**	- A function to signal block change in estimPostSacTD.d
**	- Return 1 if a prespecified number (num) of trials are finished for each stimulus.
**	- gl_rec->tasks[2] is assumed to be the relevant task.
**	- Also, there must be the trial on which every stimulus is presented exactly "num" times.i
**   This means "num" must be divisible by Trial_blocks 
** - num_by_coh (,which change the ratio of different stimuli) can have either 1 or 0. 
**	  
**	TD, 06-21-12	
*/
int	pr_change_block(int num)
{
	_PRtask task = gl_rec->tasks[2]; // should be estim_postTD
	int i;
	int residual = 0;
	
	//printf("-------------\n");
   // printf("trials_length = %d\n", task->trials_length); 
	for(i=0;i<task->trials_length;i++){
		 residual += (task->trials[i]->pmf->finished)%num;
	}
	//printf("i=%d,  residual=%d\n",i,residual);
	//printf("-------------\n");
	return(residual==0);
}

void pr_ini_current_block(void){
	current_block = 0;
}

void pr_inc_current_block(void){
	current_block++;
}

int pr_get_current_block(void){
	return(current_block);
}

void pr_zero_finished_flag(void){
	finished_flag = 0;
}

void pr_one_finished_flag(void){
	finished_flag = 1;
}

int pr_get_finished_flag(void){
	return(finished_flag);
}

/* PUBLIC ROUTINE: pr_trial_set_pmf_currentRT
** Added 2016-07-27 Upate rt_current field of pmf
** used in taka1_7.d to pass current rt to a function in task_util
** to repeat error trials depending on rt
*/

/*
   2016-07-28
   setting gl_rec->trialP->pmf->rt_current doesn't not work.
   At task_utilities.c side (more specifically, at a task specific 
   get method, rt value  is not being read out as I intended...

   I noticed that last_score is updated for the pmf at multiple levels
   in pr_trial_set_pmf_score(), which is called from rex side at the end 
   of each trial. 

   So I will try to do just the same for current rt... Not sure pmf at which
   level is relevant to the get method side, so just update all...  
   
*/
void pr_trial_set_pmf_currentRT(long rt)
{ 
   _PRtrial trial = gl_rec->trialP;

   if(trial->pmf){
     trial->pmf->rt_current = rt;
   }

  if(trial->task && trial->task->pmf){
     trial->task->pmf->rt_current = rt;
   }

  if(trial->task && trial->task->rec && trial->task->rec->pmf){
     trial->task->rec->pmf->rt_current = rt;
  }
  /* printf("pr_trial_set_pmf_currentRT: RT=%ld\n",rt); */
}
