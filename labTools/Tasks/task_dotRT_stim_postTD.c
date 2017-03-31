/* task_dotRT_stim_postTD.c
    - Post saccadic stimulation
    - Whether to deliver microstim is determined based on
       - menu entry  stim_corr_choice(+1, correct; -1, incorrect; 0, both)
       - menu enttry stim_motion_dir (+1, Angle_o; -1, Angle_o+180deg; 0, both)
       - Actual choice correctness and motion direction (detected by Rex)
    - Although I kept "estim" variable for future extension, it isn't used now.
      (in future, 5 difference conditions can be interleaved)
    - Only relevant variable in the 2D table is signed coherence.
    
    Created by Taka Doi from task_dotRT_stim_4timing.c on 2012-06-07
*/

/* 
  task_dotRT_stim_4timing.c
   - created from *_stim_EarlyLate.c on 04/21/2012 by Taka Doi
   - 4 microstimulation conditions and no stim condition
   - The starting time and duration of each stimulation condition can be specified from menu
   - # of blocks of each stimulation condition can be specified from menu.

   - e0, nostim; e1, full stim; e2, early; e3, middle or late; e4, end. 

*/


/*
  task_dotRT_stim_EarlyLate.c
   - created from task_dotRT_stim_RevCorr.c on 04/01/2012 by Taka Doi

   - Test psychometric functions under four conditions: no, full, early, and late stimulation.  

*/


/*
   task_dotRT_stim_RevCorr.c (RevCorr means "reverse correlation.")

   -- General description --
   This task c code is used for caudate electrical microstimulation
   in which we applied microstim randomly at a predetermined probability in each of short time window.
   
   This task c file inherits the flexibility as to differential trial percentages across 
   two microstim conditions (see below) and coherenes from task_dotRT_stim.c. 
   (Use "percent_stim" to set the ratio between the two estim conditions, )
   
   We may use lower coherence levels preferentially in microstim conditions and higher coherence in control
   (i.e., no microstim) condition. 

   Notes
    	estim = 0: No stimulation at all
	estim = 1: Random train of ON and OFF
  
   [Modified. TD] is put the codes that I modified.
  
   Taka Doi 2011-09-20

  -- History -- 
       2011-09-20, created from Long's task_dotRT_stim.c (Taka Doi)
       2011-12-05, debug
       2011-12-07, change "dotsrt_estimRC" to "estim_rc" (TD) 
       2011-12-07, the bug didn't exist... The values in some Paradigm menus were wrong... 
       2011-12-14, change num_by_coh because it needs 12 bits w/ the current way to use this variable.
                   the first number indicates 0/0, the second number indicates -32/+32,...

       2012-01-19, 0% coh is written as 1% for matching performance table and actual monkey's choice
                   at 0% coh. In the set method, after the direction is determined, 1% coh is converted
                   into its actual value (i.e., 0%). plexon and matlab should receive this 0% value. 
       
       2012-02-01, a new menu variable "num_zero_coh" was introduced. This specifies the number of trials
                   with pulse-stimulation and zero-coherence. This variable overrides num_by_coh_estimON and
                   useful because now we can the number of trials more than 9.  
        
       2012-02-02, introduce another condition for estim (standard estim from dots onset to sac onset).
                    - "num_by_coh" is shared between the three conditions (standard stim, pulse stim, and no stim).
                    - "num_zero_coh" specifies the num of trials with pulse stim at zero coh.
                    -
*/

#include "../toys.h"
#include "../task.h"
#include "../task_utilities.h"

/* PRIVATE MACROS/CONSTANTS */
#define NUM_GR_MENUS 6
#define TTMV(ta,n) 	pl_list_get_v((ta)->task_menus->lists[0],(n))	/* timing */
#define TPMV(ta,n) 	pl_list_get_v((ta)->task_menus->lists[1],(n))	/* basic property */
#define TSMV(ta,n)   pl_list_get_v((ta)->task_menus->lists[2],(n))   /* estim property */
#define TGMV(ta,n) 	pl_list_get_v((ta)->task_menus->lists[3],(n))	/* graphics */

/* PRIVATE ROUTINES */
void        estim_postTD_make_trials(_PRtask);
_PRtrial    estim_postTD_get_trial(_PRtask, int);
void        estim_postTD_set_trial(_PRtrial);

/* 
** TASK ESTIM POST (TD)
**		("estim_postTD") ,"estim_post" has been already reserved by Long
*/

struct _TAtask_struct gl_estim_postTD_task[] = {

	/* name */
	"estim_postTD",

	/* 
   ** Menus 
	*/
	{
	/* 
	** General Timing Parameters (likely to change for each monkey)
	*/
	{ "Timing",		1 },
            { "wait4sacon",   0,    1.0 },
            { "wait4hit",     0,    1.0 },
            { "delay2rew",    0,    1.0 },
            { "errorFB",      0,    1.0 },
            { "inter_block",  1000, 1.0 },
            {NULL},
	/* 
	** Specific Task parameters (likely to change for each exp)
	*/
	{ "Basic-Property",		 1 },
            { "Trial_order",		 0, 1.0 },
            { "Trial_blocks",	    0, 1.0 },	/* # of blocks for no stim */
          /*
            { "Trial_blocks_e1",	 0, 1.0 },	
            { "Trial_blocks_e2",	 0, 1.0 },	
            { "Trial_blocks_e3",	 0, 1.0 },	
            { "Trial_blocks_e4",	 0, 1.0 },	
          */  
            { "Angle_o",      	 0, 1.0 },
            { "Coherence_lo", 	 0, 1.0 },
            { "Coherence_hi", 	 0, 1.0 },
            { "num_by_coh",          0, 1.0 },	// [Modified. Taka]
            { "tgtecc",			 0, 1.0 },
            { "targetwin",		 0, 1.0 },
            { "reward",      		 0, 1.0 },
            { "flagRepeat",     	 0, 1.0 },
            { "Drop_codes",		 1, 1.0 },
            {NULL},

      /////////////////////////////////////////////////////////////////////
      /*
      ** Stimulation  parameters   [Modified. TD]
      */                             
      { "Estim-Property",     1 },
            { "stim_on_1",            0, 1.0 },    // to chagne stim-on/stim-off block of trials
            { "stim_corr_choice_1",   0, 1.0 },    // stim delivered after [+1] correct, [-1] incorrect, or [0] both choices 
            { "stim_motion_dir_1",    0, 1.0 },    // stim delivered after [+1] Anglie_o, [-1] opposite, or [0] both motion directions 
            { "stim_on_2",            0, 1.0 },    // to chagne stim-on/stim-off block of trials
            { "stim_corr_choice_2",   0, 1.0 },    // stim delivered after [+1] correct, [-1] incorrect, or [0] both choices 
            { "stim_motion_dir_2",    0, 1.0 },    // stim delivered after [+1] Anglie_o, [-1] opposite, or [0] both motion directions 
            { "stim_on_3",            0, 1.0 },    // to chagne stim-on/stim-off block of trials
            { "stim_corr_choice_3",   0, 1.0 },    // stim delivered after [+1] correct, [-1] incorrect, or [0] both choices 
            { "stim_motion_dir_3",    0, 1.0 },    // stim delivered after [+1] Anglie_o, [-1] opposite, or [0] both motion directions 
            { "stim_on_4",            0, 1.0 },    // to chagne stim-on/stim-off block of trials
            { "stim_corr_choice_4",   0, 1.0 },    // stim delivered after [+1] correct, [-1] incorrect, or [0] both choices 
            { "stim_motion_dir_4",    0, 1.0 },    // stim delivered after [+1] Anglie_o, [-1] opposite, or [0] both motion directions             
				{ "stim_onset",         0, 1.0 },    // onset relative to eye registration in a target window 
            { "stim_duration",      0, 1.0 },    // duration of microstimulation
            { "num_trials",         0, 1.0 },    // Number of trials for each stimulus per session
            {NULL},
      /////////////////////////////////////////////////////////////////////
      
	/*
	**	Six 'standard' graphics objects:
	**		0 - fixation
	**		1 - dots aperture
	**		2 - target centroid
	**		3 - target 1
	**		4 - target 2
	**		5 - dots direction
	*/
	{ "graphics",	 NUM_GR_MENUS },
		TU_XYRT_LIST,
 	  	{ DX_DIAM,     11, 0.1 },
   		{ DX_CLUT,   	12, 1.0 },
	   	{ NULL },
	/*
	** One dots object
	*/
	{ "dots",		 1 },
	   	{ DX_SEED,		 0, 1.0 },
   		{ DX_SPEED,		 0, 0.1 },
	   	{ DX_COH,    	 0, 0.1 },
   		{ DX_DIR,		 0, 1.0 },
	   	{ NULL },

		/* END OF MENUS */
		{ NULL }
	},

	/* 
	** Graphics objects, by name
	*/
	{  {DX_TARGET,	3 }, 
		{DX_DOTS,		1 }, 
		{ NULL }
	},

	/* 
	** User functions
	*/
	{	{ NULL },
	},

	/* 
	**	Task methods
	*/
	&estim_postTD_make_trials,
	&estim_postTD_get_trial,
	&estim_postTD_set_trial,
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* PUBLIC ROUTINE: dotsrt_estim_make_trials
**
*/
void estim_postTD_make_trials(_PRtask task)
{
  	static int coh_list[] = {1, 32, 45, 64, 91, 128, 256, 512, 999};
	int num_coh = sizeof(coh_list)/sizeof(int),
					 *cohs=NULL, coh_n,
					 coh_lo, coh_hi;
	int *estim, estim_n;
	int i,  k,  *cohs_angle=NULL, cohs_angle_n;
	 
	/* 
	** coding strategy: because the current _PRtask struct only have 
	** 2-D capability, the strategy here is to code angle0+coh as 
	** positive coh values, and -angle0+coh as negative coh values.
	** This combination will serve as the columns. 
	** The stimulation parameter will  
	** serve as the rows. So the current trials list will not carry
	** actual target angle information. This should be obtained by reading
	** the menu in set_trials routine. Also to make sure coh=0 can be 
	** associated with both directions, min coh is 1 here, but will be set 
	** to zero in set_trials routine.
	*/

	/* Figure out which coherences we'll use.
   	** Set using "Coherence_lo" and "Coherence_hi"
	**    in the task_menu struct. Note that
	**    positive values establish the range of
	**    coherence chosen from the static coh_list;
	**    a negative Coherence_lo means use just
	**    the two values (Coherence_lo and Coherence_hi).
	*/
	coh_lo = TPMV(task, "Coherence_lo");
	coh_hi = TPMV(task, "Coherence_hi");

   	if(coh_hi <= coh_lo || (coh_lo < 0 && coh_hi >= 0)) {

		/* send just one value (abs value of coh_lo) */
		coh_n   = 1;
		cohs 	  = SAFE_ZALLOC(long, 1);
		cohs[0] = abs(coh_lo); 

   	} else if(coh_lo < 0 && coh_hi < 0) {

		/* both lo and hi are negative, use abs values */
		coh_n   = 2;
		cohs 	  = SAFE_ZALLOC(int, 2);
		cohs[0] = -coh_lo; 
		cohs[1] = -coh_hi; 

	} else {

		/* Otherwise find sequence between lo and hi
		**		from coh_list, defined above
		*/
   	register ilo = -1, ihi = -1;

      while(++ilo < num_coh-1 && coh_list[ilo] < coh_lo) ;
      while(++ihi < num_coh-1 && coh_list[ihi] < coh_hi) ;

		coh_n = ihi - ilo + 1;
		cohs  = SAFE_ZALLOC(long, coh_n);
		for(i=0;i<coh_n;i++,ilo++)
			cohs[i] = coh_list[ilo];
      }
	
	cohs_angle_n = coh_n*2;
	cohs_angle   = SAFE_ZALLOC(long, cohs_angle_n);
	for (i=0; i<coh_n; i++) {
		cohs_angle[2*i]   =  cohs[i];
		cohs_angle[2*i+1] = -cohs[i];
	}

      ///////////////////////////////////////////////////////////////////////////////////////////////	
      ///////////////////////////////////////////////////////////////////////////////////////////////
   estim_n = 1;  // 0, no stim; 1, full stim; 2, early; 3, middle or late; 4, end.  
   estim   = SAFE_ZALLOC(long, estim_n);
	for(i=0;i<estim_n;i++) {
		estim[i] = i;
	}
		
	/* 
	** Make trial set for estim/angle+coherence combinations
	*/
	tu_make_trials2D(task, "estim",      estim_n,      estim,      1.0, 
				     "cohs_angle", cohs_angle_n, cohs_angle, 1.0);

	/* 
	** Free stuff
	*/
	SAFE_FREE(estim);
	SAFE_FREE(cohs_angle);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* PUBLIC ROUTINE: dotsRTestimPostTD_get_random_byCoh
**
** Modified from tu_get_random_altRows_byCoh in task_utilitites.c Long Ding 2010-08-13
**    make a superblock by combining randomized trials according to the percent defined in menus
**		also specify how many trials per coherence level.
** Gets the next trial in a randomized block,
**    making the block if necessary.
**    NOTE: only to be used for the dotsRT task, routine assumes two items for each coherence level,
**          one for angle0, one for the other. also each column is for coh x angle and each row is
**          for a different estim conditions 
**
** Arguments:
**    task        ... See paradgim_rec for details.
**    num_blocks  ... Number of blocks (reps) of the set of trials
**                      that should be used when creating the
**                      pointer array.
**    reset_flag  ... If true, set the pointer back to the beginning
**                      after reaching the end of the array.
**    num_by_coh  ... Flag indicating the numbers for each coh level, if 0, use Trial_blocks for all coh levels.
**                      For example, 111 means one x num_blocks trials per coherence level;
**                      5400 means no trials for the lowest two levels, 4 x num_blocks for the third level and 5 x for the fourth level,
**                      no trials for higher coherence levels.
*/
/*
** num_blocks parameter is determined by "Trial_blocks" in Basic-property task menu (Taka 09/14/11)
*/

_PRtrial dotsRTestimPostTD_get_random_byCoh(_PRtask task, int num_blocks[], int reset_flag, int num_by_coh, int num_zero_coh)
{
   int last_score = task->pmf ? task->pmf->last_score : 0;
   int flagRepeat = pl_list_get_v(task->task_menus->lists[1], "flagRepeat");
   //  score: 1, correct; 0, error; -1, no choice; -2, broken fix.

   /*   printf("entered in get_random_byCoh\n");
        printf("[1] reset_flag = %d\n", reset_flag); 
        printf("[1] trialPs_index=%d\n",task->trialPs_index); */
   if(task->trialPs) {

      /* check previous score ... */
      if ( (last_score <= -1) || ( (last_score==0) && flagRepeat ) ) {

         /* always repeat fixbreak or no-choice trials , or error-trial with flagRepeat on */
         //printf (" repeat trial \n");
         return(task->trialPs[task->trialPs_index]);
      }

      if ( (last_score<0) && !flagRepeat ) {      // (no choice or fix broken) and (no flag repeat) 

         /* swap trial with later trial */
         printf (" swap trial \n");
         pr_task_swap_trialP(task);

      /* try to increment the pointer */
      } else if((++(task->trialPs_index)) == task->trialPs_length) {
         
         /* reached the end, clear pointer array
         ** so next time through it gets re-randomized
         */
         SAFE_FREE(task->trialPs);
         task->trialPs_length = 0;
         //printf("trialPs are freed.\n");
      }

   } else {

      /* force 'reset' first time through */
      reset_flag = 1;
      //printf("reset_flag is set to 1.\n");
   }


   /////////////////////////////////////////////////////////////////////////////////////////////////
   /* check to make block */   
   //printf("[2] reset_flag = %d\n", reset_flag);
   //printf("[2] trialPs_index=%d (after a possible increment) \n",task->trialPs_index);
   if(reset_flag && !task->trialPs){
      int num_stim = task->trials_rows;    // should be 3 (02/02/12)
      int num_coh  = task->trials_columns; // # of signed coherence prepared in the 2D parameter table.
      int total_num_coh[] = {0}; // [0] for no stim, [1] for full stim, [2] for early stim, [3] for late stim, [4] for end stim.
                                     // This number could be different
                                     // from "num_coh" when the same coherence is sampled multiple times
                                     // or some coherences are omitted when constructing the pointer array.
      int num_blocks_new = 0;
      int i, j, k, m, n, tmp;
      int count_per_block = 0;
      int numTrialCoh[num_stim][num_coh]; // numTrialCoh[0] for estim0; numTrialCoh[1] for estim1, and so on. [Modified. Taka]
     	int numTrialStim[num_stim];  
      _PRtrial *finaltrialPs;

      printf("entered the process to make a new trial block.\n");
      

	/* A 2D table of singed coherence * estim condition is prepared at the line 249.
      ** Now we are going to construct a pointer array by sampling pointers each pointing an element
      ** in the 2D table. Repeated sampling from the same pointer is possible. We calculate the
      ** number of coherences and estim conditions (taking the repetition into account) to allocate
      ** the memory for the pointer array.
      */ 
      /* [Modified. Taka]
         num_by_coh 
          will be shared between the three conditions.  
      */ 

      //-------------------------------------------------------------------
      // [Modified. TD]
      /* figure out from num_by_coh how many trials per coherence level */
      for (j=0; j<num_stim; j++){            // j=0, estim off; j=1, pulse estim; j=2, standard estim. 
         tmp = num_by_coh; 

         if (tmp==0) {
      		for (i=0; i<num_coh; i++)
      		{
      			numTrialCoh[j][i] = 1;
               //  printf("numTrialCoh[%d][%d] = [%d]\n", j, i, numTrialCoh[j][i]);
	         }
                  total_num_coh[j] = num_coh;
         } else { 
	            for (i=0; i<num_coh; i=i+2)
      	      {
      	         numTrialCoh[j][i]   =  tmp % 10;
      	         numTrialCoh[j][i+1] =  tmp % 10;
      	         tmp                 = (tmp - numTrialCoh[j][i])/10;
                  
                     total_num_coh[j]    = total_num_coh[j] + numTrialCoh[j][i] + numTrialCoh[j][i+1];
      	      
                     printf("numTrialCoh[%d][%d]=%d\n",j,i,  numTrialCoh[j][i]);
                     printf("numTrialCoh[%d][%d]=%d\n",j,i+1,numTrialCoh[j][i+1]);
                  }
         }
         printf("total_num_coh[%d]=%d\n", j, total_num_coh[j]);
      }

      //-------------------------------------------------------------------
	   /* figure out how many trials per stim condition, sum up to num_blocks */
      numTrialStim[0] = num_blocks[0];
   /* numTrialStim[1] = num_blocks[1];
      numTrialStim[2] = num_blocks[2];
      numTrialStim[3] = num_blocks[3];
      numTrialStim[4] = num_blocks[4]; */
      
      num_blocks_new = num_blocks[0];
      printf("#block <1>,     %d\n", numTrialStim[0]);
 /*  	num_blocks_new = num_blocks[0]+num_blocks[1]+num_blocks[2]+num_blocks[3]+num_blocks[4]; // I keep "num_blocks_new" becasue subsequent codes use it. (Taka 09/22/11)
      printf("#block <1>,     %d\n", numTrialStim[0]);
      printf("#block <2>,     %d\n", numTrialStim[1]);
      printf("#block <3>,     %d\n", numTrialStim[2]);
      printf("#block <4>,     %d\n", numTrialStim[3]);
      printf("#block <5>,     %d\n", numTrialStim[4]);*/
      //-------------------------------------------------------------------

      for (j=0; j<num_stim; j++){  
            count_per_block = count_per_block + total_num_coh[j]*numTrialStim[j]; // estim-condition-wise countup
	         //printf("count_per_block=%d,total_num_coh[%d]=%d,numTrialStim[%d]=%d\n",count_per_block, j, total_num_coh[j],j,numTrialStim[j]);
      }
      // "count_per_block" specifies the size of pointer array we must allocate.
      // Trial order randomization is done to this pointer array.      
      printf("count_per_block, %d\n", count_per_block);

      //-------------------------------------------------------------------
      SAFE_FREE(task->trialPs);
      task->trialPs        = SAFE_ZALLOC(_PRtrial, count_per_block);
      task->trialPs_length = count_per_block ;
      task->trialPs_index  = 0;
      //printf("[1] task->trialPs_length is set to %d\n",task->trialPs_length);

      //-------------------------------------------------------------------      
      /* fill the array with repeated blocks of pointers
      ** to trials, in order
      */
      // "i" index for estim condition (i=0, estin off; i=1, estim rev corr)
      // "k" index for signed coherence (coh, 0,0,32,-32,64,-64,...)
      // "j" and "m" are just counters.
      // I'm a bit unconfident for this for loops...
      //  num_stim, # of estim conditions. 2 in my experiment.
      //  num_coh,  # of signed coherence prepared in the 2D table. 
      //  printf("estim index 1=off, 2=rev corr / coh = 0,0,32,-32,64,-64,...\n"); 
      n = 0;
   	for (i=0; i<num_stim; i++)
   		for (j=0; j<numTrialStim[i];j++)	            // Repeated sampling from the same estim condition	
   			for (k=0; k<num_coh; k++)
   				for (m=0; m<numTrialCoh[i][k]; m++)    // Repeated sampling from the same coherence 
   				{
                  /* printf("numTrialStim[%d]: %d, numTrialCoh[%d][%d]: %d filling %d \n", 
                          i, numTrialStim[i],    i, k, numTrialCoh[i][k], i*num_coh+k);
                  */
                          printf("[%2d] trial index=%d, estim index=%d, signed coh index=%d\n",n,i*num_coh+k, i, k);
                              
                          task->trialPs[n] = task->trials[i*num_coh+k];
              	           n++;
                              
                              /* In original Long's code, the index into task->trials[] is calculated as i*num_cols+k.
                              ** "num_cols" is the total count of coherences inluding multiple counts of unique coherence.
                              ** My guess is that "num_coh" should be used instead of "num_cols" becasue "num_coh" is
                              ** task->trials_columns (# of columns in the 2D trial array). 
                              ** Need to ask Long!
                                  --> Confirmed my guess was correct.  
									   */
				}	
      
      pr_task_randomize_trialP(task);

      printf("num_coh %d, num_stim %d, num_blocks_new %d, total %d trials \n", num_coh, num_stim, num_blocks_new, n);
      //printf("num_blocks estim0 %d, num_blocks estim1 %d, num_blocks estim2 %d, num_blocks estim3 %d, num_blocks estim4 %d\n", numTrialStim[0], numTrialStim[1], numTrialStim[2], numTrialStim[3], numTrialStim[4]); 
      
      // num_cols is replaced w/ num_coh. 
      task->trialPs_length = n;
      task->trialPs_index  = 0;
      //printf("[1] task->trialPs_length is set to %d\n",task->trialPs_length);
   }

   /* check if there is a list of pointers */
   if(!task->trialPs){
     //printf("reset_flag = %d", reset_flag); 
     return(NULL);
   }

   /* return pointer to the current trial */
   //printf("dotsrt_estim_get_trial done \n");
   return(task->trialPs[task->trialPs_index]);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/* PUBLIC ROUTINE: dotsrt_estim_get_trial
**
** Returns:
**		_PRtrial if trials remain
**		NULL otherwise
*/
_PRtrial estim_postTD_get_trial(_PRtask task, int reset_flag)	
{

   int num_by_coh;
   int num_zero_coh;
//   int trial_blocks[] = {0,0,0,0,0};
   int trial_blocks[] = {0};
   
	trial_blocks[0] = TPMV(task, "Trial_blocks");
/*	trial_blocks[1] = TPMV(task, "Trial_blocks_e1");
	trial_blocks[2] = TPMV(task, "Trial_blocks_e2");
	trial_blocks[3] = TPMV(task, "Trial_blocks_e3");
	trial_blocks[4] = TPMV(task, "Trial_blocks_e4");
*/
   num_by_coh   = TPMV(task, "num_by_coh");
   num_zero_coh = TPMV(task, "num_zero_coh");

	return(dotsRTestimPostTD_get_random_byCoh(task, trial_blocks, reset_flag, num_by_coh, num_zero_coh));
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* PUBLIC ROUTINE: estim_postTD_set_trial
**
**	Note that here we mostly access groups/lists/properties
**	 by INDEX, not NAME, so be careful if anything
**	 above changes.
*/
void estim_postTD_set_trial(_PRtrial trial)
{
	/*
	** set properties for regular targets 3 (correct) & 4 (other)
	**
	*/
	_PLlist 	tgm = trial->task->task_menus->lists[3];
	_PLlist	tdm = trial->task->task_menus->lists[4];
	int angle = TPMV(trial->task, "Angle_o"), ang;
	int cohv;
	int dxtarget_indices[3] = {0, 1, 2};
	int dxdots_indices[1] = {0};
	int temp = 0;
	
	/* First, set properties for the dXtarget objects */
	/* Make working copies of values in 
	**		graphics menu 
	*/
	pl_list_set_v2w(tgm);

	/* Conditionally fill-in angle ("r")
	** 	for graphics object 3 (target)
	**			object 4 (other)
	*/
	cohv = PL_L2PV(trial->list, 1);
	ang = angle;
   if (cohv<0)
		ang = angle+180;
	cohv = abs(cohv);
   if (cohv==1)
      cohv = 0;
	
   printf("dotRT_estimPostTD trial  estim = %d angle = %d coh = %d Ecc = %d\n", 
	PL_L2PV(trial->list, 0), ang, cohv, TPMV(trial->task, "tgtecc")  );

	PL_CSET( PL_L2PWS(tgm, 5, 3), ang);
	PL_CSET( PL_L2PWS(tgm, 4, 3), TPMV(trial->task, "tgtecc") );
	PL_CSET( PL_L2PWS(tgm, 5, 4), ang+180);
	PL_CSET( PL_L2PWS(tgm, 4, 4), TPMV(trial->task, "tgtecc") );	

	/* compute x, y positions of
	**	Note: the indices are based on the graphics menu
	**		fixation point (obj 0)
	**		target (obj 3)
	**		other (obj 4)
 	*/
	tu_compute_xyrt(tgm);

	/* Set x, y positions in 
	**		actual dXtarget objects
	**	Note: the indices here are based on the graphics objects
	**		fixation point (obj 0)
	**		target (obj 1)
	**		other (obj 2)
	*/
	dx_set_by_indexIV(DXF_NODRAW, 0, 3, dxtarget_indices, 
		1, PL_L2PWS(tgm, 0, 0), 		PL_L2PWS(tgm, 0, 3), 		PL_L2PWS(tgm, 0, 4), 
		2, PL_L2PWS(tgm, 1, 0), 		PL_L2PWS(tgm, 1, 3), 		PL_L2PWS(tgm, 1, 4), 
		3, PL_L2PWS(tgm, TU_XYRT_LEN, 0), 	PL_L2PWS(tgm, TU_XYRT_LEN, 3), PL_L2PWS(tgm, TU_XYRT_LEN, 4),
		4, PL_L2PWS(tgm, TU_XYRT_LEN+1, 0), PL_L2PWS(tgm, TU_XYRT_LEN+1, 3), PL_L2PWS(tgm, TU_XYRT_LEN+1, 4),
		ENDI);

	/* Second, set properties of the dXdots object */

	dx_set_by_nameIV(DXF_D1, DX_DOTS, 1, dxdots_indices,
			DX_X,		0,
			DX_Y, 	0,
			DX_DIAM,	PL_L2PWS(tgm, TU_XYRT_LEN, 1),
			DX_CLUT,	PL_L2PWS(tgm, TU_XYRT_LEN+1, 1),
			DX_COH, 	cohv,
			DX_DIR, 	ang,
 			DX_SPEED,  	PL_L2PVS(tdm, 1 ,0),
 			DX_SEED,  	tu_get_seed(PL_L2PVS(tdm, 0, 0), cohv, ang), 
 			NULL);

	/* Conditionally drop codes */
	if(TPMV(trial->task, "Drop_codes") != 0) {

		/*ec_send_code_tagged(I_ESTIMCD, 7000 + PL_L2PV(trial->list, 0));*/	/* send tagged ecode with estim type */
		// commented out because I_ESTIMCD is sent from Rex side

		pl_ecodes_by_nameV(dx_get_object(DX_TARGET), 7000,
			DX_X,		0, 	I_FIXXCD,
			DX_Y,		0, 	I_FIXYCD,
			DX_CLUT,	0, 	I_FIXLCD,
			DX_X,		1, 	I_TRG1XCD,
			DX_Y,		1, 	I_TRG1YCD,
			DX_CLUT,	1, 	I_TRG1LCD,
			DX_X,		2, 	I_TRG2XCD,
			DX_Y,		2, 	I_TRG2YCD,
			DX_CLUT,	2, 	I_TRG2LCD,
			NULL);

		pl_ecodes_by_nameV(dx_get_object(DX_DOTS), 7000,
			DX_COH,	0, 	I_COHCD,
			DX_DIR,	0, 	I_DOTDIRCD,
			DX_SEED,	0, 	I_DTVARCD,
			NULL);
	}
  // printf("estim_postTD_set_trial done \n");
}



