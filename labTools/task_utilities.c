/*
** TASK_UTILITIES.C
**
** Utility functions to use with pRec/pTask 
**		structures & routines
*/

/* TD 2013-06-06
   Created tu_get_tandom_altRaws_TD from Long's version to use in dotRT_TD and dotsreg_TD tasks. 
   Trials are swapped (not repeated) after ncerr and brfix trials.
   I'll keep asym rew part untouched although I won't use it in my task.
   
   Coherence Distribution
   created a set of td_ functions, which are used in my task c programs (dotsreg, dotRT)

   TD 2013-06-12
   Trials are repeated after ncerr and brfix if flagRepeat is 1.
 
   TD 2013-06-20
   TOY_RT_TO_X and TOY_RT_TO_Y are replaced with toy_rt_to_x and toy_rt_to_y, respectively, 
   for a better rounding.

	Note that my functions for biased coherence distribution have a prefix of td_
*/


#include <stddef.h>
#include "toys.h"
#include "task_utilities.h"
#include "property_list.h"

/* PUBLIC ROUTINES */

	/* make trials */

/* PUBLIC ROUTINE: tu_make_trials1D
**
*/
void tu_make_trials1D(_PRtask task, 
	char *name, int num, int *values, float multiplier)
{
	register int i;

	/* check args ... be sure to free trials
	** if none given
	*/
	if(num <= 0) {
		pr_task_clear(task);
		return;
	}

	/* add the array of (empty) trials */
	pr_task_add_trials1D(task, num);

	/* For each, add a list with the value
	** of the given property.
	*/
	for(i=0;i<num;i++) {
		task->trials[i]->id = task->id*100+i;
		task->trials[i]->list = 
			pl_list_initV("1dtc", 0, 1,
				name, values[i], multiplier,
				NULL);
	}
}

/* PUBLIC ROUTINE: tu_make_trials2D
**
**	Utility to make a 2D trial set.
**	Parameter #1 is rows
**	Parameter #2 is columns
**
**	Remember that, as per C conventions, matrix
**		is stored row by row.
**
**	Arguments:
**		_PRtask ... the task to update
**		name1	  ... string name of paramter #1
**		n1	  	  ... number of values in paramter #1 array (rows)
**		v1	  	  ... array of values for paramter #1
**		m1		  ... multiplier for paramter #1
**		name2	  ... string name of paramter #2
**		n2	  	  ... number of values in paramter #2 array (cols)
**		v2	  	  ... array of values for paramter #2
**		m2		  ... multiplier for paramter #2
*/
void tu_make_trials2D(_PRtask task, 
	char *name1, int n1, int *v1, float m1,
	char *name2, int n2, int *v2, float m2)
{
	long dummy = 0;
	register int i, j, k;

	/* check args ... be sure to free trials
	** if none given
	*/
	if(n1 <= 0 && n2 <= 0) {
		pr_task_clear(task);
		return;
	}

	if(n1 <= 0) {
		n1 = 1;
		v1 = &dummy;
		m1 = 1.0;
	}
	if(n2 <= 0) {
		n2 = 1;
		v2 = &dummy;
		m2 = 1.0;
	}

	/* add the array of (empty) trials */
	pr_task_add_trials2D(task, n1, n2);

	/* For each, add a list of 2 properties,
	**	corresponding to the values of the two
	**	variables.
	*/
	for(k=0,i=0;i<n1;i++) {
		for(j=0;j<n2;j++,k++) {
			task->trials[k]->id = task->id*100+k;
			task->trials[k]->list = 
				pl_list_initV("2dtc", 0, 1,
					name1, v1[i], m1,
					name2, v2[j], m2, 
					NULL);
		}
	}
}

	/* get trials */
/* PUBLIC ROUTINE: tu_get_block
**
**	Gets the next trial from a standard (not randomized) block,
**		making the block if necessary
**
**	Arguments:
**		task 			... See paradgim_rec for details.
**		num_blocks 	... Number of blocks (reps) of the set of trials
**								that should be used when creating the 
**								pointer array.
**		reset_flag  ... If true, set the pointer back to the beginning
**								after reaching the end of the array.
**		
*/
_PRtrial tu_get_block(_PRtask task, int num_blocks, int reset_flag)
{
	int last_score = task->pmf ? task->pmf->last_score : 0;

	/* Conditionally make a single block...
	** This will also reset the index to 0
	*/
	if(task->trialPs == NULL) {

		pr_task_make_trialP_all(task, num_blocks);

		/* check that it worked */
		if(task->trialPs == NULL)
			return(NULL);

	/* Otherwise increment the pointer and check for
	** end of the array
	*/
	} else if(last_score >= 0 &&
		++(task->trialPs_index) >= task->trialPs_length) {

		/* end of the line ... check for reset */
		if(reset_flag > 0)

			/* reset pointer back to the beginning of the array */	
			task->trialPs_index = 0;

		else

			/* no reset, end of the line */
			return(NULL);
	}

	/* return the current trial */
	return(task->trialPs[task->trialPs_index]);
}

/* PUBLIC ROUTINE: tu_get_random
**
**	Gets the next trial in a randomized block,
**		making the block if necessary.
**
** Arguments:
**		task 			... See paradgim_rec for details.
**		num_blocks 	... Number of blocks (reps) of the set of trials
**								that should be used when creating the 
**								pointer array.
**		reset_flag  ... If true, set the pointer back to the beginning
**								after reaching the end of the array.
*/
_PRtrial tu_get_random(_PRtask task, int num_blocks, int reset_flag)
{	
	int last_score = task->pmf ? task->pmf->last_score : 0;
   int flagRepeat = pl_list_get_v(task->task_menus->lists[1], "flagRepeat");
	if(task->trialPs) {

		/* check previous score ... */
	   if ( (last_score <= -1) || ( (last_score==0) && flagRepeat ) ) {

         /* always repeat fixbreak or no-choice trials , or error-trial with flagRepeat on */
         /* printf("tu_get_random:  repeat trial\n"); */
         return(task->trialPs[task->trialPs_index]);
      }

		/* bad trial -  swap trial with later trial */
      if ( (last_score<0) && !flagRepeat ) {
         /* printf("tu_get_random:  swap trial\n"); */
			pr_task_swap_trialP(task);

		/* try to increment the pointer */
		} else if((++(task->trialPs_index)) == task->trialPs_length) {

			/* reached the end, clear pointer array
			** so next time through it gets re-randomized
			*/
			SAFE_FREE(task->trialPs);
			task->trialPs_length = 0;
		}

	} else {
		
		/* force 'reset' first time through */
		reset_flag = 1;
	}

	/* check to make block */
	if(reset_flag && !task->trialPs) {

		/* make the randomized set... the second argument
		** determines the minimum number of trial
		** blocks to include in the array -- this
		** is to make sure that redoing trials
		** after an nc/brfix doesn't cause
		** a predictable pattern to accumulate
		** at the end of the list
		*/
		pr_task_make_trialP_allR(task, num_blocks);
	}

	/* check if there is a list of pointers */
	if(!task->trialPs)
		return(NULL);

	/* return pointer to the current trial */
	return(task->trialPs[task->trialPs_index]);
}

/* PUBLIC ROUTINE: tu_get_random_altRows
**
** Modified by Long from tu_get_random()
**		make a superblock by combining randomized blocks by rows.
**	Gets the next trial in a randomized block,
**		making the block if necessary.
**
** Arguments:
**		task 			... See paradgim_rec for details.
**		num_blocks 	... Number of blocks (reps) of the set of trials
**								that should be used when creating the 
**								pointer array.
**		reset_flag  ... If true, set the pointer back to the beginning
**								after reaching the end of the array.
*/
_PRtrial tu_get_random_altRows(_PRtask task, int num_blocks, int reset_flag)
{	
	int last_score = task->pmf ? task->pmf->last_score : 0;
	int flagRepeat = pl_list_get_v(task->task_menus->lists[1], "flagRepeat");

	if(task->trialPs) {

		/* check previous score ... */
		if ( (last_score <= -1) || ( (last_score==0) && flagRepeat ) ) {
		
			/* always repeat fixbreak or no-choice trials , or error-trial with flagRepeat on */
			/* printf("tu_get_random_altRows: repeat trial\n");  */
			return(task->trialPs[task->trialPs_index]);
		}
		
		if ( (last_score<0) && !flagRepeat ) {
		
			/* swap trial with later trial */
			/* printf("tu_get_random_altRows: swap trial\n");  */
			pr_task_swap_trialP(task);
		
		/* try to increment the pointer */
		} else if((++(task->trialPs_index)) == task->trialPs_length) {

			/* reached the end, clear pointer array
			** so next time through it gets re-randomized
			*/
			SAFE_FREE(task->trialPs);
			task->trialPs_length = 0;
		}

	} else {
		
		/* force 'reset' first time through */
		reset_flag = 1;
	}

	/* check to make block */
	if(reset_flag && !task->trialPs) {

		int num_sets = task->trials_rows;
		int num_cols = task->trials_columns;
		int count_per_row = num_cols * num_blocks;
		int count_per_row_padded = count_per_row;	
		int flagAR = pl_list_get_v(task->task_menus->lists[1], "asymRew");
		int numPadded = pl_list_get_v(task->task_menus->lists[1], "StartHiCohTrials") * 2;
	   int i, j, k, m, n;	
		_PRtrial *finaltrialPs;

		if ( flagAR && (numPadded>0) ) 
			count_per_row_padded = count_per_row + numPadded;

		finaltrialPs = SAFE_ZALLOC(_PRtrial, num_sets*count_per_row_padded); 

		/* 
		** Long: create a super block with blocks of randomized sets. 
		** Each block contains num_blocks randomized trials.
		** LD 2010-07-15, to signal block change for Asymmetric reward task
		**   add 2 trials with the highest coherence for each direction at the beginning of a block 
		*/
		
		SAFE_FREE(task->trialPs);
		task->trialPs = SAFE_ZALLOC(_PRtrial, count_per_row);
		task->trialPs_length = count_per_row;
		task->trialPs_index = 0;
		
		n = 0;
		for(m=0; m<num_sets; m++) {
			
			/* fill the array with repeated blocks of pointers 
			** to trials, in order
			*/
			k = 0;
			for(i=0;i<num_blocks;i++)
				for(j=0;j<num_cols;j++) {
					task->trialPs[k] = task->trials[j + m*num_cols];
					k++;
				}
			pr_task_randomize_trialP(task);
      	if (flagAR && numPadded>0) {		
				for (i=0; i<numPadded/2; i++) {
					finaltrialPs[n] = task->trials[num_cols - 2 + m*num_cols];
					finaltrialPs[n+1] = task->trials[num_cols - 1 + m*num_cols];	
					n=n+2;	
				}	
				for (i=0; i<k; i++) {
					finaltrialPs[n] = task->trialPs[i]; 
					n++;
				}
			} else {
            for (i=0; i<k; i++) {
               finaltrialPs[n] = task->trialPs[i];
               n++;
            }	
			}		
		}
		
		SAFE_FREE(task->trialPs);
 		task->trialPs = finaltrialPs;
		task->trialPs_length = n;
		task->trialPs_index = 0;
	}
	
	/* check if there is a list of pointers */
	if(!task->trialPs)
		return(NULL);

	/* return pointer to the current trial */
	return(task->trialPs[task->trialPs_index]);
}

/* PUBLIC ROUTINE: tu_get_random_altRows_byCoh
**
** Modified by Long Ding 2009-07-14 from tu_get_random_altRows()
**    make a superblock by combining randomized blocks by rows, also specify how many trials per coherence level.
** Gets the next trial in a randomized block,
**    making the block if necessary.
**		NOTE: only to be used for the dotsRT task, routine assumes two items for each coherence level,
**				one for angle0, one for the other. also each column is for cohxangle and each row is 
**				for a different reward contingency, 
**
** Arguments:
**    task        ... See paradgim_rec for details.
**    num_blocks  ... Number of blocks (reps) of the set of trials
**                      that should be used when creating the
**                      pointer array.
**    reset_flag  ... If true, set the pointer back to the beginning
**                      after reaching the end of the array.
**		num_by_coh 	... flag indicating the numbers for each coh level. 
**							For example, 111 means one x num_blocks trials per coherence level;
**							5400 means no trials for the lowest two levels, 4 x num_blocks for the third level and 5 x for the fourth level,
**								no trials for higher coherence levels.
*/
_PRtrial tu_get_random_altRows_byCoh(_PRtask task, int num_blocks, int reset_flag, int num_by_coh)
{
   int last_score = task->pmf ? task->pmf->last_score : 0;
   int flagRepeat = pl_list_get_v(task->task_menus->lists[1], "flagRepeat");

   if(task->trialPs) {

      /* check previous score ... */
      if ( (last_score <= -1) || ( (last_score==0) && flagRepeat ) ) {

         /* always repeat fixbreak or no-choice trials , or error-trial with flagRepeat on */
         /* printf("tu_get_random_altRows_byCoh: repeat trial\n"); */
         return(task->trialPs[task->trialPs_index]);
      }

      if ( (last_score<0) && !flagRepeat ) {

         /* swap trial with later trial */
         /* printf("tu_get_random_altRows_byCoh: swap trial\n"); */
         pr_task_swap_trialP(task);

      /* try to increment the pointer */
      } else if((++(task->trialPs_index)) == task->trialPs_length) {

         /* reached the end, clear pointer array
         ** so next time through it gets re-randomized
         */
         SAFE_FREE(task->trialPs);
         task->trialPs_length = 0;
      }

   } else {

      /* force 'reset' first time through */
      reset_flag = 1;
   }

   /* check to make block */
   if(reset_flag && !task->trialPs) {

      int num_sets = task->trials_rows;
      int num_coh = task->trials_columns/2;
	  int num_cols = 0;
      int i, j, k, m, n;
	  int count_per_row = 0;
	  int numTrialcoh[num_coh];
	  _PRtrial *finaltrialPs;

		/* figure out from num_by_coh how many trials per coherence level */
		for (i=0; i<num_coh; i++)
		{
			numTrialcoh[i] =  num_by_coh % 10 ;	
			num_by_coh = (num_by_coh - numTrialcoh[i])/10;
			num_cols = num_cols + numTrialcoh[i];
		}
		num_cols = num_cols * 2;
					
      count_per_row = num_cols * num_blocks;
      finaltrialPs = SAFE_ZALLOC(_PRtrial, num_sets*count_per_row);

      /*
      ** Long: create a super block with blocks of randomized sets. 
      ** Each block contains num_blocks randomized trials.
      */

      SAFE_FREE(task->trialPs);
      task->trialPs = SAFE_ZALLOC(_PRtrial, count_per_row);
      task->trialPs_length = count_per_row;
      task->trialPs_index = 0;
      n = 0;
      for(m=0; m<num_sets; m++) {

         /* fill the array with repeated blocks of pointers
         ** to trials, in order
         */
         k = 0;
			for (i=0; i<task->trials_columns; i++) 
				for (j=0; j<num_blocks*numTrialcoh[i/2]; j++)	{
					task->trialPs[k] = task->trials[i];
					k++;
				}
/*			for (i=0; i<k; i++)
				printf(" %d ", PL_L2PV(task->trialPs[i]->list,1));
*/

         pr_task_randomize_trialP(task);
         for (i=0; i<k; i++) {
            finaltrialPs[n] = task->trialPs[i];
            n++;
         }
      }

      SAFE_FREE(task->trialPs);
      task->trialPs = finaltrialPs;
      task->trialPs_length = n;
      task->trialPs_index = 0;
   }

   /* check if there is a list of pointers */
   if(!task->trialPs)
      return(NULL);

   /* return pointer to the current trial */
   return(task->trialPs[task->trialPs_index]);
}

/* PUBLIC ROUTINE: tu_compute_xyrt
**
**	Computes x,y positions of a set of
**		graphics objects with properties 
**		defined as "TU_XYRT_LIST"  
**		(see task_utilities.h)
**
*/
void tu_compute_xyrt(_PLlist list)
{
	int pi=0;
	register int i=0;

	if(!list || !list->properties_length || 
		!list->values_length)
		return;

	/* find index of property DX_X */
	while(pi<list->properties_length && 	
		strcmp(list->properties[pi]->name, DX_X)) pi++;
	if(pi == list->properties_length)
		return;

	/* set each rt */
	for(i=0;i<list->values_length;i++) {
		tu_compute_rtR(list->properties, pi, i);

/*
**		printf("obj %d, r = %d, t = %d\n", i, 
**			list->properties[pi+kR]->values_wc[i],
**			list->properties[pi+kT]->values_wc[i]);
*/
	}

	/* set each xy */
	for(i=0;i<list->values_length;i++) {
		tu_compute_xyR(list->properties, pi, i);

/*
**		printf("obj %d, x = %d, y = %d\n", i, 
**			list->properties[pi]->values_wc[i],
**			list->properties[pi+1]->values_wc[i]);
*/
	}
}

#define WVI(ki_,vi_) 	ps[pi+(ki_)]->values_wc[(vi_)]
#define WV(ki_)  			WVI(ki_, vi)
#define WC_VDV(v_,dv_) 	WC_V(v_, dv_); WC_DV(v_,dv_)
#define WC_V(v_,dv_)  	if(WV(v_) == NULLI) { WV(v_) = 0; }
#define WC_DV(v_,dv_) 	if(WV(dv_)>0) { 							\
					WV(v_) += (TOY_RAND(2*WV(dv_)+1) - WV(dv_)); \
					WV(dv_) = NULLI; }

void tu_compute_rtR(_PLproperty *ps, int pi, int vi)
{
	int wrti;

	/* 
	** Check r,t
	*/
	WC_VDV(kR, kDR);
	WC_VDV(kT, kDT);

	/* 
	** Get WRT angle ... first get index of wrt object,
	**		then marks as NULLI to avoid inf recurs.
	*/
	wrti 		= WV(kWRT);
	WV(kWRT) = NULLI;

	/* check for no wrt */
	if(wrti == NULLI || wrti == vi || wrti < 0 || 
		wrti >= (*ps)->values_length)
		return;

	/* recursively compute the angle ! */
	tu_compute_rtR(ps, pi, wrti);	

	/* add the angle */
	WV(kR) += WVI(kR, wrti);
}

void tu_compute_xyR(_PLproperty *ps, int pi, int vi)
{
	int vertexi, vx, vy;

	/* 
	** Check if x, y are set ...
	*/
	if(WV(kX) != NULLI && WV(kY) != NULLI) {

		/* check for dx, dy */
		WC_DV(kX, kDX);
		WC_DV(kY, kDY);

		/* outta */
		return;
	}

	/* r, t should already be set */
	vertexi 		= WV(kVERTEX);
	WV(kVERTEX) = NULLI;

	/* check for no vertex */
	if(vertexi == NULLI || vertexi == vi || vertexi < 0 ||
		vertexi >= (*ps)->values_length) {

		vx = 0;
		vy = 0;

	} else {

		tu_compute_xyR(ps, pi, vertexi);

		vx = WVI(kX, vertexi);
		vy = WVI(kY, vertexi);
	}

	/* compute x, y from r, t, and 
	**		add dx,dy if neccesary.
	*/
   //	WV(kX) = TOY_RT_TO_X(vx, WV(kR), WV(kT));
	WV(kX) = toy_rt_to_x(vx, WV(kR), WV(kT)); // TD 20130620. A new way of randing.
	WC_DV(kX, kDX);

	// WV(kY) = TOY_RT_TO_Y(vy, WV(kR), WV(kT));
	WV(kY) = toy_rt_to_y(vy, WV(kR), WV(kT)); // TD 20130620
	WC_DV(kY, kDY);

   //printf("DEBUG: vx %d, WV(kR) %d, WV(kT) %d --> WV(kX), %d\n",vx,WV(kR),WV(kT),WV(kX));

/*
**	printf("vt=%d,vx=%d,vy=%d,kr=%d,kt=%d,kdx=%d,kdy=%d,kx=%d,ky=%d\n",
**		vertexi,vx,vy,WV(kR),WV(kT),WV(kDX),WV(kDY),WV(kX),WV(kY));
*/
}

/* PUBLIC ROUTINE: tu_compute_dangle
** Returns an array containing target angles +/- RAND(dangle)
**	Added by bss on 04/26/2007 
*/
int *tu_compute_dangle(int angle, int toff, int dangle, int radius, int dangle_limit)
{
	int *dangled[3];
	int t1, t2;	
	int ang_betw_t; 
	double d_betw_t;
	
	/* Choose a position for T1 everything else is relative to this */	
	if(TOY_RAND(100)<=49) {
   		angle+=toff;
  	}
	else {
   		angle+=(toff+180);
	}

	/* Choose if you are adding or subtracting dangle */
	if(dangle>1) {
		if(TOY_RAND(100)<=49) {
	       	t1=angle-TOY_RAND(dangle);
	       	}
		else {
       		t1=angle+TOY_RAND(dangle);
		}
		
		if(TOY_RAND(100)<=49) {
     		t2=angle+180-TOY_RAND(dangle);
			}
		else {
     		t2=angle+180+TOY_RAND(dangle);
     	}
    }
	else {
		t1=angle;
		t2=angle+180;
	}
	
	/* Check final angles to make sure that they are not too close */
	ang_betw_t=abs(t2-t1);
	if(ang_betw_t>180) {
		ang_betw_t=360-ang_betw_t; /* Normalize */
	}
	
	d_betw_t=2*(sin(DEG2RAD*((double)ang_betw_t/2)))*((double)radius/10);
	
	if(d_betw_t>dangle_limit) {
		dangled[2]=1;
		}
	else {
		dangled[2]=0;
	}

	dangled[0]=t1;
	dangled[1]=t2;

	return(dangled);
}

/* PUBLIC ROUTINE: tu_get_seed
**
** Generate seed for random number generator.
**	Behavior depends on seed_base:
**		<0 ... generate random seed
**	   =0 ... use zero as seed
**	   >0 ... generate seed from base, coherence, angle
*/
int tu_get_seed(int seed_base, int coherence, int angle)
{
	struct timespec now;
	int seed;

	if (seed_base < 0) {
		clock_gettime(CLOCK_REALTIME, &now);
		seed = now.tv_sec%1000;	
	}
	else if (seed_base == 0) {
		seed = 0;
	}
	else {
		seed = seed_base + coherence + angle;
	}
/*	printf("dot seed = %d\n", seed); */
	return (seed);	
}

/* PUBLIC ROUTINE: tu_put_breakpoint
**  TD, 2014-02-03
*/
int tu_put_breakpoint(void){
   int c;
   printf("enter n to continue the program.\n ");
   while( (c=getchar()) != EOF){
      putchar(c);
      if(c=='n')
         break;
   }
   return(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Takahiro Doi
//   - task_dotRT_TD.c
//   - task_dotsreg_TD.c
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* PUBLIC ROUTINE: td_get_random_altRows
 **
 ** Modified by Taka from Long's tu_get_random_alRaws()
 **		Trial is swapped with a later triial, not repeated, after no-choice erorr or broken fixation as in the original
 **    tu_get_random function. This prevents monks from exploting the predictable stracture of motion direction.
 **    (modified part is limited to the swap and repeat trial part..)
 **
 ** Arguments:
 **		task 			... See paradgim_rec for details.
 **		reset_flag       ... If true, set the pointer back to the beginning
 **								after reaching the end of the array.
 */
_PRtrial td_get_random_altRows(_PRtask task, int reset_flag)
{
	int last_score = task->pmf ? task->pmf->last_score : 0;
	int flagRepeat = pl_list_get_v(task->task_menus->lists[1], "flagRepeat");
   
//   printf("DEBUG: td_get_random_altRows is entered.\n");
 
	if(task->trialPs) {
        
		/* Swap trial with later trial after ncerr or brfix, if flagRepeat is off */
		if( (last_score<0) && !flagRepeat ) {
			printf ("-> Swap trial due to brfix or ncerr \n");
			pr_task_swap_trialP(task);
            
      /* Repeat trial after error choice, ncerr, or brfix if flagRepeat is on  */
        } else if ( (last_score<1) && flagRepeat ){
			printf ("-> Repeat trial due to error choice and flagRepeat\n");
			return(task->trialPs[task->trialPs_index]); /* return pointer ot the current trial */
            
      /* try to increment the pointer after correct choice and error choice with flagRepeat off*/
		} else if((++(task->trialPs_index)) == task->trialPs_length) {
            
			/* reached the end, clear pointer array
             ** so next time through it gets re-randomized
             */
			SAFE_FREE(task->trialPs);
			task->trialPs_length = 0;
		}
        
	} else {
		
		/* force 'reset' first time through */
		reset_flag = 1;
	}
    
	/* check to make block */
    td_make_block(task, reset_flag); /* Capsuled into a separate function. TD */
	
	/* check if there is a list of pointers */
	if(!task->trialPs)
		return(NULL);
    
	/* return pointer to the current trial, which should be zero now. */
	return(task->trialPs[task->trialPs_index]);
}

/* sub routine
*/
void td_make_block(_PRtask task, int reset_flag)
{
if(reset_flag && !task->trialPs)
{
    int num_sets = task->trials_rows;    // this should be 1 in my task since rewcount is always 2.
    int num_cohs = task->trials_columns; // # of SIGNED coherence prepared in the 2D parameter table.
    int count_per_row = 0;               // number of all trials within a single block (note that we have only 1 row)
    int numTrialCoh[num_cohs]; // num of trials for each coh
    int i, j, k;
    
//    printf("-> entered the process to make a new trial block.\n");
    
     /*  A 2D table of singed coherence * 1 has been prepared with tu_make_trials2D(). In this table, 
     ** the set of signed coherence (14 singed cohs from 0 to +/-999) is fixed unlike Long's task c script.  
     ** Now we are going to construct a pointer array by sampling pointers each pointing an element
     ** in the 2D table. Repeated sampling from the same pointer is possible. We calculate the
     ** number of coherences (taking the repetition into account) to allocate
     ** the memory for the pointer array.
     */

    //---------------- Biased sampling of coherence --------------------
    // [1] Calculating the probability distribution of coherence
    //     Populate numTrialCoh
    td_calc_numTrialCoh(task, numTrialCoh, num_cohs); // note that numTrialCoh is a function of SIGNED coh
    for(i=0;i<num_cohs;i++){
        count_per_row += numTrialCoh[i];
    }
    // [2] Memory allocation
    SAFE_FREE(task->trialPs);
    task->trialPs        = SAFE_ZALLOC(_PRtrial, count_per_row);
    task->trialPs_length = count_per_row;
    task->trialPs_index  = 0;
    // [3] Populate task->trialPs by sampling from 2D pointer table task-> trials
    //     In my experiment, this table is actually 1D.
    //     Repeated sampling from the same pointer is used to achieve different frequencies across coherences.
    //     i, coherence; j, number of trials for each coherence; k, total counter
    k = 0;
    for(i=0;i<num_cohs;i++){
        for(j=0;j<numTrialCoh[i];j++, k++)
        {
            task->trialPs[k] = task->trials[i];
            // signed coh is organized as {+1,-1,+32,-32,+64,-64,+128,-128,+256,-256,+512,-512,+999,-999}
       	   //	printf("DEBUG: i %d, j %d, k %d\n",i,j,k); 
        }
    }
    printf("count_per_row %d is matched to k %d? num_cohs %d\n", count_per_row,k,num_cohs);
    // [4] Randomize trial pointers
    pr_task_randomize_trialP(task);

    /*
    task->trialPs_length = k;
    task->trialPs_index  = 0;
    */
}
}

/* PUBLIC ROUTINE: td_calc_numTrialCoh_
 **  - Number of trials for each coherence is calculated here based on menu variables.
 **  - Note that num_cohs is the number of SINGED coherence, whereas probability density
 **    distributions are made as a function of ABSOLUTE coherence
 ** 
 **  Created by TD on 2013-06-06
 */
void td_calc_numTrialCoh(_PRtask task, int *nTrial,int num_cohs)
{   
    int num_acohs    = num_cohs/2;
    // weight parameter, 0 - 100%
    int nTrialpBlock = pl_list_get_v(task->task_menus->lists[1], "nTrialpBlock");
    int wHighCoh     = pl_list_get_v(task->task_menus->lists[1], "Coh_W_High");
    int wUniCoh      = pl_list_get_v(task->task_menus->lists[1], "Coh_W_Uni");
    int Alpha        = pl_list_get_v(task->task_menus->lists[1], "Coh_Alpha");
    int Beta         = pl_list_get_v(task->task_menus->lists[1], "Coh_Beta");
    
    // Probability density distribution 
    double p_uni[num_acohs];
    double p_beta[num_acohs];
    double p_high[num_acohs];
    double p_comb[num_acohs]; // linear combination of three PDFs
    double dnTrial[num_acohs]; // double, num of trials
    double x; // used as x for the beta distribution 
    double dnum_acohs = (double)num_acohs;
    double dAlpha    = (double)Alpha;
    double dBeta     = (double)Beta;
    double dwHighCoh = (double)wHighCoh/100.0;
    double dwUniCoh  = (double)wUniCoh/100.0;
    double sumbeta, residual, r;
    double tmp = 0; 
    int    sumTrials = 0;
    int i;
    
    // uniform distribution
    for(i=0;i<num_acohs;i++){
        p_uni[i] = 1/dnum_acohs;
    }

    // high-coh distribution. For now, only the highest coherence
    for(i=0;i<num_acohs;i++){
        p_high[i] = 0.0;
    }
    p_high[num_acohs-1] = 1.0;
    
    // Beta distribution
    sumbeta = 0;
    for(i=0;i<num_acohs;i++){
        x = i*(1/(dnum_acohs-1.0)); // increment from 0 to 1 with a step of 1/6
        p_beta[i] = pow(x,dAlpha-1)*pow(1-x,dBeta-1);
        sumbeta   += p_beta[i];
    }
    // normalize p_beta to make it PDF
    for(i=0;i<num_acohs;i++){
        p_beta[i] = p_beta[i]/sumbeta;
    }
    
    // combination of uniform and beta distribution
    for(i=0;i<num_acohs;i++){
        p_comb[i] = dwUniCoh*p_uni[i] + (1-dwUniCoh)*p_beta[i];
    }

    // combination of combined and high-coh distribution
    for(i=0;i<num_acohs;i++){
        p_comb[i] = dwHighCoh*p_high[i] + (1-dwHighCoh)*p_comb[i];
        tmp      += p_comb[i]; 
    }
//    printf("DEBUG: sum of p_comb = %f\n",tmp);

    // p_comb should be a pdf
    // Then the conversion from probability to number of trials
    // Note that nTrial is a function of SIGNED coherence, organized as {-1,1,-32,32,-64,64,...}.
    // Put the same number of trials to pos and neg coherences with the same absolute value. 
    for(i=0;i<num_acohs;i++){
        dnTrial[i] = p_comb[i]*(double)nTrialpBlock;
        residual   = dnTrial[i] - floor(dnTrial[i]);
        r = ((double) rand()) / ((double) RAND_MAX);
        nTrial[2*i]   = (int)(floor(dnTrial[i])) + (int)(residual>r);
        nTrial[2*i+1] = nTrial[2*i];   
 
        sumTrials = sumTrials + nTrial[2*i] + nTrial[2*i+1];  
        printf("-> abs coh index %d: nTrial [%d]\n",i, nTrial[2*i]);
    }
//    printf("DEBUG: total num of trials=%d\n",sumTrials);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Takahiro Doi
//   - task_dotsvar_TD.c
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 2015-07-15
 For a variable delay Dots task, I developed
 td_make_block2()
 td_get_random_altRows2()
 
 td_get_random_altRows2() is different from  td_get_random_altRows() only in that it calls td_make_block2() instead of td_make_block()
 */
_PRtrial td_get_random_altRows2(_PRtask task, int reset_flag)
{
	int last_score = task->pmf ? task->pmf->last_score : 0;
	int flagRepeat = pl_list_get_v(task->task_menus->lists[1], "flagRepeat");
    
    //   printf("DEBUG: td_get_random_altRows is entered.\n");
    
	if(task->trialPs) {
        
		/* Swap trial with later trial after ncerr or brfix, if flagRepeat is off */
		if( (last_score<0) && !flagRepeat ) {
			printf ("-> Swap trial due to brfix or ncerr \n");
			pr_task_swap_trialP(task);
            
            /* Repeat trial after error choice, ncerr, or brfix if flagRepeat is on  */
        } else if ( (last_score<1) && flagRepeat ){
			printf ("-> Repeat trial due to error choice and flagRepeat\n");
			return(task->trialPs[task->trialPs_index]); /* return pointer ot the current trial */
            
            /* try to increment the pointer after correct choice and error choice with flagRepeat off*/
		} else if((++(task->trialPs_index)) == task->trialPs_length) {
            
			/* reached the end, clear pointer array
             ** so next time through it gets re-randomized
             */
			SAFE_FREE(task->trialPs);
			task->trialPs_length = 0;
		}
        
	} else {
		
		/* force 'reset' first time through */
		reset_flag = 1;
	}
    
	/* check to make block */
    td_make_block2(task, reset_flag); /* Capsuled into a separate function. TD */
	
	/* check if there is a list of pointers */
	if(!task->trialPs)
		return(NULL);
    
	/* return pointer to the current trial, which should be zero now. */
	return(task->trialPs[task->trialPs_index]);
}

/* sub routine
 */
void td_make_block2(_PRtask task, int reset_flag)
{
    if(reset_flag && !task->trialPs)
    {
        int num_durs = task->trials_rows;    // this should be 1 in my task since rewcount is always 2.
        int num_cohs = task->trials_columns; // # of SIGNED coherence prepared in the 2D parameter table.
        int count_per_row = 0;               // number of all trials within a single block (note that we have only 1 row)
        int numTrialCoh[num_cohs]; // num of trials for each coh
        int i, j, k, m;
        
        //    printf("-> entered the process to make a new trial block.\n");
        
        /*  A 2D table of singed coherence * 1 has been prepared with tu_make_trials2D(). In this table,
         ** the set of signed coherence (14 singed cohs from 0 to +/-999) is fixed unlike Long's task c script.
         ** Now we are going to construct a pointer array by sampling pointers each pointing an element
         ** in the 2D table. Repeated sampling from the same pointer is possible. We calculate the
         ** number of coherences (taking the repetition into account) to allocate
         ** the memory for the pointer array.
         */
        
        //---------------- Biased sampling of coherence --------------------
        // [1] Calculating the probability distribution of coherence
        //     Populate numTrialCoh
        td_calc_numTrialCoh(task, numTrialCoh, num_cohs); // note that numTrialCoh is a function of SIGNED coh
        for(i=0;i<num_cohs;i++){
            count_per_row += numTrialCoh[i];
        }
        // [2] Memory allocation
        SAFE_FREE(task->trialPs);
        task->trialPs        = SAFE_ZALLOC(_PRtrial, count_per_row*num_durs);
        task->trialPs_length = count_per_row*num_durs;
        task->trialPs_index  = 0;
        // [3] Populate task->trialPs by sampling from 2D pointer table task-> trials
        //     In my experiment, this table is actually 1D.
        //     Repeated sampling from the same pointer is used to achieve different frequencies across coherences.
        //     i, coherence; j, number of trials for each coherence; k, total counter
        k = 0;                     // global counter
        for(m=0;m<num_durs;m++){   // duration
            for(i=0;i<num_cohs;i++){   // coherence
                for(j=0;j<numTrialCoh[i];j++, k++) // multiple trials within a coherence
                {
                    task->trialPs[k] = task->trials[i + num_cohs*m]; // the last element should be 11+12*3=47. ok, this is right.
                
                /* 2015-07-15 TD task_dotsvar_TD.c
                 ** 2D trial set
                 **  Parameter #1 (rows) is durations
                 **  Parameter #2 (cols) is sigend coherence
                 **
                 ** k is global counter (linear counter)
                 ** i is counter for param #1
                 ** j is counter for param #2
                 ** in my case, i = {0, 1, 2, 3}
                 **             j = {0, ..... , 11}
                 ** trials[0]:  i=0, j=0
                 ** trials[1]:  i=0, j=1
                 ** trials[2]:  i=0, j=2
                 ** .....
                 ** trials[10]: i=0, j=10
                 ** trials[11]: i=0, j=11
                 
                 ** trials[12]:  i=1, j=0
                 ** trials[13]:  i=1, j=1
                 ** trials[14]:  i=1, j=2
                 ** .....
                 ** trials[47]:  i=3, j=11
                 */
                
                    // signed coh is organized as {+1,-1,+32,-32,+64,-64,+128,-128,+256,-256,+512,-512,+999,-999}
                    // printf("DEBUG: m %d, i %d, j %d, k %d\n",m,i,j,k);
                }
            }
        }
        printf("count_per_row %d is matched to k %d? num_cohs %d\n", count_per_row,k,num_cohs);
        // [4] Randomize trial pointers
        pr_task_randomize_trialP(task);
        
        /*
         task->trialPs_length = k;
         task->trialPs_index  = 0;
         */
    }
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// taka1_5.d, task_dotRT_TD3.c
// Coherence set: 64, 128, 256, 512. Flat distribution.
// Repeat error trials only limited times
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* PUBLIC ROUTINE: td_get_random_altRows4
 **
 ** Developed by Taka from td_get_random_altRows3()
 ** Only difference is  td_make_block3()
 ** Maximum number of possible repeats, MaxNumRep
 **
 ** Arguments:
 **		task 			... See paradgim_rec for details.
 **		reset_flag       ... If true, set the pointer back to the beginning
 **								after reaching the end of the array.
 */
_PRtrial td_get_random_altRows4(_PRtask task, int reset_flag)
{
	int last_score = task->pmf ? task->pmf->last_score : 0;
	int flagRepeat = pl_list_get_v(task->task_menus->lists[1], "flagRepeat");
    static int initflag = 0;
    static int MaxNumRep = 0; // it's C, so static var can't be initialized with another var.
    
	if(!initflag){
        MaxNumRep = flagRepeat;
        initflag  = 1;
	}
    //   printf("DEBUG: td_get_random_altRows is entered.\n");
    
	if(task->trialPs) {
        if(MaxNumRep) {
            //------------------------------------------------------------------------------------
            /* Repeat trial after error choice, ncerr, or brfix if flagRepeat is on  */
            if(last_score<1){
                printf ("-> Repeat trial due to error choice and flagRepeat\n");
                printf("*************************************\n");
                printf("MaxNumRep = %d\n", MaxNumRep);
                if(last_score==0){
                    MaxNumRep--;
                    printf("MaxNumRep was decremented\n");
                }
                printf("*************************************\n");
                // decrement each time a trials is repeated because of an error choice
                return(task->trialPs[task->trialPs_index]); /* return pointer ot the current trial */
                
                /* try to increment the pointer after correct choice and error choice with flagRepeat off*/
            } else {
                // reset count after a correct trial
                if(last_score==1) MaxNumRep = flagRepeat;
                
                if((++(task->trialPs_index)) == task->trialPs_length) {
                    /* reached the end, clear pointer array
                     ** so next time through it gets re-randomized
                     */
                    SAFE_FREE(task->trialPs);
                    task->trialPs_length = 0;
                }
            }
        } else {
            // When MaxNumRep = 0, do as if flagRepeat is off.
            MaxNumRep = flagRepeat;
            
            /* Swap trial with later trial after ncerr or brfix, if flagRepeat is off */
            if(last_score<0) {
                printf ("-> Swap trial due to brfix or ncerr \n");
                pr_task_swap_trialP(task);
                
                /* try to increment the pointer after correct choice and error choice with flagRepeat off*/
            } else if((++(task->trialPs_index)) == task->trialPs_length) {
                /* reached the end, clear pointer array
                 ** so next time through it gets re-randomized
                 */
                SAFE_FREE(task->trialPs);
                task->trialPs_length = 0;
            }
        }
	} else {
		/* force 'reset' first time through */
		reset_flag = 1;
	}
    
	/* check to make block */
    td_make_block3(task, reset_flag); /* Capsuled into a separate function. TD */
	
	/* check if there is a list of pointers */
	if(!task->trialPs)
		return(NULL);
    
	/* return pointer to the current trial, which should be zero now. */
	return(task->trialPs[task->trialPs_index]);
}


/*  sub routine
 ** Developed from td_make_block() by Taka Doi on 20160511
 ** Only difference is to call td_calc_numTrialCoh(), which creates a flat, not biased, coherence distribution
 */
void td_make_block3(_PRtask task, int reset_flag)
{
    if(reset_flag && !task->trialPs)
    {
        int num_sets = task->trials_rows;    // this should be 1 in my task since rewcount is always 2.
        int num_cohs = task->trials_columns; // # of SIGNED coherence prepared in the 2D parameter table.
        int count_per_row = 0;               // number of all trials within a single block (note that we have only 1 row)
        int numTrialCoh[num_cohs]; // num of trials for each coh
        int i, j, k;
        
        //    printf("-> entered the process to make a new trial block.\n");
        
        /*  A 2D table of singed coherence * 1 has been prepared with tu_make_trials2D(). In this table,
         ** the set of signed coherence (14 singed cohs from 0 to +/-999) is fixed unlike Long's task c script.
         ** Now we are going to construct a pointer array by sampling pointers each pointing an element
         ** in the 2D table. Repeated sampling from the same pointer is possible. We calculate the
         ** number of coherences (taking the repetition into account) to allocate
         ** the memory for the pointer array.
         */
        
        //---------------- Biased sampling of coherence --------------------
        // [1] Calculating the probability distribution of coherence
        //     Populate numTrialCoh
        td_calc_numTrialCoh2(task, numTrialCoh, num_cohs); // note that numTrialCoh is a function of SIGNED coh
        for(i=0;i<num_cohs;i++){
            count_per_row += numTrialCoh[i];
        }
        // [2] Memory allocation
        SAFE_FREE(task->trialPs);
        task->trialPs        = SAFE_ZALLOC(_PRtrial, count_per_row);
        task->trialPs_length = count_per_row;
        task->trialPs_index  = 0;
        // [3] Populate task->trialPs by sampling from 2D pointer table task-> trials
        //     In my experiment, this table is actually 1D.
        //     Repeated sampling from the same pointer is used to achieve different frequencies across coherences.
        //     i, coherence; j, number of trials for each coherence; k, total counter
        k = 0;
        for(i=0;i<num_cohs;i++){
            for(j=0;j<numTrialCoh[i];j++, k++)
            {
                task->trialPs[k] = task->trials[i];
                // signed coh is organized as {+1,-1,+32,-32,+64,-64,+128,-128,+256,-256,+512,-512,+999,-999}
                //	printf("DEBUG: i %d, j %d, k %d\n",i,j,k);
            }
        }
        printf("count_per_row %d is matched to k %d? num_cohs %d\n", count_per_row,k,num_cohs);
        // [4] Randomize trial pointers
        pr_task_randomize_trialP(task);
        
        /*
         task->trialPs_length = k;
         task->trialPs_index  = 0;
         */
    }
}


/* PUBLIC ROUTINE: td_calc_numTrialCoh2
 **  - Flat coherence distribution
 **  - Note that num_cohs is the number of SINGED coherence, whereas probability density
 **    distributions are made as a function of ABSOLUTE coherence
 **
 **  Created by TD on 2015-05-11 from td_calc_numTrialCoh
 */
void td_calc_numTrialCoh2(_PRtask task, int *nTrial,int num_cohs)
{
    int num_acohs    = num_cohs/2;
    // weight parameter, 0 - 100%
    int nTrialpBlock = pl_list_get_v(task->task_menus->lists[1], "nTrialpBlock");
    
    // Probability density distribution
    double p_uni[num_acohs];
    double p_comb[num_acohs]; // linear combination of three PDFs

    double dnTrial[num_acohs]; // double, num of trials
    double dnum_acohs = (double)num_acohs;
    double residual, r;
    int    sumTrials = 0;
    int i;
    
    // uniform distribution
    for(i=0;i<num_acohs;i++){
        p_uni[i] = 1/dnum_acohs;
    }
    
    // p_comb should be a pdf
    // Then the conversion from probability to number of trials
    // Note that nTrial is a function of SIGNED coherence, organized as {-1,1,-32,32,-64,64,...}.
    // Put the same number of trials to pos and neg coherences with the same absolute value.
    for(i=0;i<num_acohs;i++){
        dnTrial[i] = p_uni[i]*(double)nTrialpBlock;
        residual   = dnTrial[i] - floor(dnTrial[i]);
        r = ((double) rand()) / ((double) RAND_MAX);
        nTrial[2*i]   = (int)(floor(dnTrial[i])) + (int)(residual>r);
        nTrial[2*i+1] = nTrial[2*i];
        
        sumTrials = sumTrials + nTrial[2*i] + nTrial[2*i+1];
        printf("-> abs coh index %d: nTrial [%d]\n",i, nTrial[2*i]);
    }
    //    printf("DEBUG: total num of trials=%d\n",sumTrials);
}







/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// taka1_6.d, task_dotRT_TD4.c
// Coherence set: 0, 32, 64, 128, 256, 512, 724, 999. arbitrary distribution by num_by_coh 
// Repeat error trials only limited times by flagRepeat
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* PUBLIC ROUTINE: td_get_random_altRows5
 **
 ** Developed by Taka from td_get_random_altRows4()
 ** Maximum number of possible repeats, MaxNumRep
 **
 ** Arguments:
 **		task 	   ... See paradgim_rec for details.
 **		reset_flag ... If true, set the pointer back to the beginning
 **								after reaching the end of the array.
 **             num_by_coh ... specifies the relative num of trials for each coherence level
 */
_PRtrial td_get_random_altRows5(_PRtask task, int reset_flag, int num_by_coh)
{
    int last_score = task->pmf ? task->pmf->last_score : 0;
    int flagRepeat = pl_list_get_v(task->task_menus->lists[1], "flagRepeat");
    static int initflag = 0;
    static int MaxNumRep = 0; // it's C, so static var can't be initialized with another var.
    
    printf("td_get_random_altRows5\n");

	if(!initflag){
        MaxNumRep = flagRepeat;
        initflag  = 1;
	}
    //   printf("DEBUG: td_get_random_altRows is entered.\n");
    
	if(task->trialPs) {
        if(MaxNumRep) {
            //------------------------------------------------------------------------------------
            /* Repeat trial after error choice, ncerr, or brfix if flagRepeat is on  */
            if(last_score<1){
                printf ("-> Repeat trial due to error choice and flagRepeat\n");
                printf("*************************************\n");
                printf("MaxNumRep = %d\n", MaxNumRep);
                if(last_score==0){
                    MaxNumRep--;
                    printf("MaxNumRep was decremented\n");
                }
                printf("*************************************\n");
                // decrement each time a trials is repeated because of an error choice
                return(task->trialPs[task->trialPs_index]); /* return pointer ot the current trial */
                
                /* try to increment the pointer after correct choice and error choice with flagRepeat off*/
            } else {
                // reset count after a correct trial
                if(last_score==1) MaxNumRep = flagRepeat;
                
                if((++(task->trialPs_index)) == task->trialPs_length) {
                    /* reached the end, clear pointer array
                     ** so next time through it gets re-randomized
                     */
                    SAFE_FREE(task->trialPs);
                    task->trialPs_length = 0;
                }
            }
        } else {
            // When MaxNumRep = 0, do as if flagRepeat is off.
            MaxNumRep = flagRepeat;
            
            /* Swap trial with later trial after ncerr or brfix, if flagRepeat is off */
            if(last_score<0) {
                printf ("-> Swap trial due to brfix or ncerr \n");
                pr_task_swap_trialP(task);
                
                /* try to increment the pointer after correct choice and error choice with flagRepeat off*/
            } else if((++(task->trialPs_index)) == task->trialPs_length) {
                /* reached the end, clear pointer array
                 ** so next time through it gets re-randomized
                 */
                SAFE_FREE(task->trialPs);
                task->trialPs_length = 0;
            }
        }
	} else {
		/* force 'reset' first time through */
		reset_flag = 1;
	}
    
	/* check to make block */
        td_make_block4(task, reset_flag, num_by_coh); /* Capsuled into a separate function. TD */
	
	/* check if there is a list of pointers */
	if(!task->trialPs)
		return(NULL);
    
	/* return pointer to the current trial, which should be zero now. */
	return(task->trialPs[task->trialPs_index]);
}

/*  sub routine
 ** Developed from td_make_block3() by Taka Doi on 20160524
 ** Only difference is to call td_calc_numTrialCoh3(), which can create arbitrary, biased coherence distribution with the use of num_by_coh variable 
 */
void td_make_block4(_PRtask task, int reset_flag, int num_by_coh)
{
    if(reset_flag && !task->trialPs)
    {
        int num_sets = task->trials_rows;    // this should be 1 in my task since rewcount is always 2.
        int num_cohs = task->trials_columns; // # of SIGNED coherence prepared in the 2D parameter table.
        int count_per_row = 0;               // number of all trials within a single block (note that we have only 1 row)
        int numTrialCoh[num_cohs]; // num of trials for each coh
        int i, j, k;
        
        //    printf("-> entered the process to make a new trial block.\n");
        
        /*  A 2D table of singed coherence * 1 has been prepared with tu_make_trials2D(). In this table,
         ** the set of signed coherence (14 singed cohs from 0 to +/-999) is fixed unlike Long's task c script.
         ** Now we are going to construct a pointer array by sampling pointers each pointing an element
         ** in the 2D table. Repeated sampling from the same pointer is possible. We calculate the
         ** number of coherences (taking the repetition into account) to allocate
         ** the memory for the pointer array.
         */
        
        //---------------- Biased sampling of coherence --------------------
        // [1] Calculating the probability distribution of coherence
        //     Populate numTrialCoh
        td_calc_numTrialCoh3(task, numTrialCoh, num_cohs, num_by_coh); // note that numTrialCoh is a function of SIGNED coh
        for(i=0;i<num_cohs;i++){
            count_per_row += numTrialCoh[i];
        }
        // [2] Memory allocation
        SAFE_FREE(task->trialPs);
        task->trialPs        = SAFE_ZALLOC(_PRtrial, count_per_row);
        task->trialPs_length = count_per_row;
        task->trialPs_index  = 0;
        // [3] Populate task->trialPs by sampling from 2D pointer table task-> trials
        //     In my experiment, this table is actually 1D.
        //     Repeated sampling from the same pointer is used to achieve different frequencies across coherences.
        //     i, coherence; j, number of trials for each coherence; k, total counter
        k = 0;
        for(i=0;i<num_cohs;i++){
            for(j=0;j<numTrialCoh[i];j++, k++)
            {
                task->trialPs[k] = task->trials[i];
                // signed coh is organized as {+1,-1,+32,-32,+64,-64,+128,-128,+256,-256,+512,-512,+999,-999}
                //	printf("DEBUG: i %d, j %d, k %d\n",i,j,k);
            }
        }
        printf("count_per_row %d is matched to k %d? num_cohs %d\n", count_per_row,k,num_cohs);
        // [4] Randomize trial pointers
        pr_task_randomize_trialP(task);
        
        /*
         task->trialPs_length = k;
         task->trialPs_index  = 0;
         */
    }
}
      



/* PUBLIC ROUTINE: td_calc_numTrialCoh3
 **  - Flat coherence distribution
 **  - Note that num_cohs is the number of SINGED coherence, whereas probability density
 **    distributions are made as a function of ABSOLUTE coherence
 **
 **  - nbc, num_by_coh
 **
 **  Created by TD on 2015-05-24 from td_calc_numTrialCoh2
 */
void td_calc_numTrialCoh3(_PRtask task, int *nTrial, int num_cohs, int nbc)
{
	int num_acohs    = num_cohs/2;
	// weight parameter, 0 - 100%
	int nTrialpBlock = pl_list_get_v(task->task_menus->lists[1], "nTrialpBlock");
	// Probability density distribution
	double prob[num_acohs];

	double dnTrial[num_acohs]; // double, num of trials
	double dnum_acohs = (double)num_acohs;
	double residual, r;
	int    sumTrials = 0;
	int i;
	double tmp=0.0;

   printf("enter trial fun\n");

   if (nbc==0) {
		for (i=0; i<num_acohs; i++){
			prob[i] = 1/dnum_acohs;
		 }      
	} else {
	    for (i=0; i<num_acohs; i++){
		prob[i] =  nbc % 10;
		nbc      = (nbc - prob[i])/10;
		tmp = tmp + prob[i];
	    }
	    // normalize prob to make it a prob distribution
	    for (i=0; i<num_acohs; i++){
		prob[i] = prob[i]/tmp; 
	    }
	}

	for(i=0;i<num_acohs;i++){
		dnTrial[i] = prob[i]*(double)nTrialpBlock;
		residual   = dnTrial[i] - floor(dnTrial[i]);
		r = ((double) rand()) / ((double) RAND_MAX);
		nTrial[2*i]   = (int)(floor(dnTrial[i])) + (int)(residual>r);
		nTrial[2*i+1] = nTrial[2*i];

		// sumTrials = sumTrials + nTrial[2*i] + nTrial[2*i+1];
		printf("-> abs coh index %d: nTrial [%d]\n",i, nTrial[2*i]);
		// printf("DEBUG: total num of trials=%d\n",sumTrials);
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// taka1_7.d, task_dotRT_TD5.c
// Repeat error trials only limited times by flagRepeat
// Also, repeat trials only after a short RT error choice. (threshold is set by a menu variable)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* PUBLIC ROUTINE: td_get_random_altRows6
 **
 ** Developed by Taka from td_get_random_altRows5()
 ** Maximum number of possible repeats, MaxNumRep
 **
 ** Arguments:
 **		task 	   ... See paradgim_rec for details.
 **		reset_flag ... If true, set the pointer back to the beginning
 **								after reaching the end of the array.
 **             num_by_coh ... specifies the relative num of trials for each coherence level
 */
_PRtrial td_get_random_altRows6(_PRtask task, int reset_flag, int num_by_coh)
{
    int last_score = task->pmf ? task->pmf->last_score : 0;
    long last_rt   = task->pmf ? task->pmf->rt_current : 1000;
    int flagRepeat = pl_list_get_v(task->task_menus->lists[1], "flagRepeat");
    int flagRepThre = pl_list_get_v(task->task_menus->lists[1], "flagRepThre"); // threshold RT in ms
    static int initflag = 0;
    static int MaxNumRep = 0; // it's C, so static var can't be initialized with another var.
    
    printf("td_get_random_altRows6\n");

	if(!initflag){
        MaxNumRep = flagRepeat;
        initflag  = 1;
	}
    //   printf("DEBUG: td_get_random_altRows is entered.\n");
    
	if(task->trialPs) {
        if(MaxNumRep) {
            //------------------------------------------------------------------------------------
            /* Repeat trial after error choice, ncerr, or brfix if flagRepeat is on  */
            if((last_score<0)||((last_score==0)&&(last_rt<flagRepThre))){
                printf ("-> Repeat trial due to error choice and flagRepeat\n");
                printf("*************************************\n");
                printf("MaxNumRep = %d\n", MaxNumRep);
                if(last_score==0){
                    MaxNumRep--;
                    printf("MaxNumRep was decremented\n");
                    printf("RT = %ld\n",last_rt);
                }
                printf("*************************************\n");
                // decrement each time a trials is repeated because of an error choice
                return(task->trialPs[task->trialPs_index]); /* return pointer ot the current trial */
                
                /* try to increment the pointer after correct choice and error choice with flagRepeat off*/
            } else {
                // reset count always when trial isn't repeated
                MaxNumRep = flagRepeat;
                
                if((++(task->trialPs_index)) == task->trialPs_length) {
                    /* reached the end, clear pointer array
                     ** so next time through it gets re-randomized
                     */
                    SAFE_FREE(task->trialPs);
                    task->trialPs_length = 0;
                }
            }
        } else {
            // When MaxNumRep = 0, do as if flagRepeat is off.
            MaxNumRep = flagRepeat;
            
            /* Swap trial with later trial after ncerr or brfix, if flagRepeat is off */
            if(last_score<0) {
                printf ("-> Swap trial due to brfix or ncerr \n");
                pr_task_swap_trialP(task);
                
                /* try to increment the pointer after correct choice and error choice with flagRepeat off*/
            } else if((++(task->trialPs_index)) == task->trialPs_length) {
                /* reached the end, clear pointer array
                 ** so next time through it gets re-randomized
                 */
                SAFE_FREE(task->trialPs);
                task->trialPs_length = 0;
            }
        }
	} else {
		/* force 'reset' first time through */
		reset_flag = 1;
	}
    
	/* check to make block */
        td_make_block4(task, reset_flag, num_by_coh); /* Capsuled into a separate function. TD */
	
	/* check if there is a list of pointers */
	if(!task->trialPs)
		return(NULL);
    
	/* return pointer to the current trial, which should be zero now. */
	return(task->trialPs[task->trialPs_index]);
}


