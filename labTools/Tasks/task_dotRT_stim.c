/*
** task_dotRT_stim.c
**
**	Long Ding, modified from task_dotRT_LD20070201.c Aug 2010 
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
void 		dotsrt_estim_make_trials	(_PRtask);
_PRtrial dotsrt_estim_get_trial	(_PRtask, int);
void 		dotsrt_estim_set_trial	(_PRtrial);

/* 
** TASK DOTSRT
**		("dotsrt_estim")
*/

struct _TAtask_struct gl_dotsrt_estim_task[] = {

	/* name */
	"dotsrt_estim",

	/* 
	** Menus 
	*/
	{
	/* 
	** General Timing Parameters (likely to change for each monkey)
	*/
	{ "Timing",		1 },
   		{ "wait4sacon",      	0, 1.0 },
   		{ "wait4hit",   	0, 1.0 },
   		{ "delay2rew",   	0, 1.0 },
   		{ "errorFB",   	0, 1.0 },
         { "inter_block",      1000, 1.0 },
		{NULL},
	/* 
	** Specific Task parameters (likely to change for each exp)
	*/
	{ "Basic-Property",		1 },
		{ "Trial_order",		0, 1.0 },
		{ "Trial_blocks",		0, 1.0 },	/* Trial_blocks: # of repetition of a full set of trials */
	  	{ "Angle_o",      	0, 1.0 },
	  	{ "Coherence_lo", 	0, 1.0 },
   	{ "Coherence_hi", 	0, 1.0 },
		{ "num_by_coh",		0, 1.0 },	
		{ "tgtecc",				0, 1.0 },
		{ "targetwin",			0, 1.0 },
   	{ "reward",      		0, 1.0 },
   	{ "flagRepeat",     	0, 1.0 },
		{ "Drop_codes",		1,	1.0 },
		{ "percent_stim", 		0, 1.0 },	/* total percent of trials with stimulation */
		{NULL},
   /*
   ** Stimulation  parameters 
   */
   { "Estim-Property",     1 },
      { "pre_start",       -1, 1.0 },
      { "pre_end",         0, 1.0 }, 
		{ "dots_start",    	0, 1.0 },   
      { "dots_end",        -1, 1.0 },
      { "postsac_start",    0, 1.0 },
      { "postsac_end",      0, 1.0 },
      { "postrew_start",    0, 1.0 },
      { "postrew_end",      1, 1.0 },
      { "percent_pre",    		0, 1.0 },
      { "percent_dots",       0, 1.0 },
      { "percent_postsac",    0, 1.0 },
      { "percent_reward",     0, 1.0 },
      {NULL},

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
	&dotsrt_estim_make_trials,
	&dotsrt_estim_get_trial,
	&dotsrt_estim_set_trial,
};


/* PUBLIC ROUTINE: dotsrt_estim_make_trials
**
*/
void dotsrt_estim_make_trials(_PRtask task)
{
  	static int coh_list[] = {1, 32, 64, 128, 256, 512, 999};
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
	**	The stimulation parameter will  
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
	cohs_angle = SAFE_ZALLOC(long, cohs_angle_n);
	for (i=0; i<coh_n; i++) {
		cohs_angle[2*i] = cohs[i];
		cohs_angle[2*i+1] = -cohs[i];
	}
	
	/* 
	** Make array of angles and reward contingencies
	**	estim = 0: no stimulation 
	** estim = 1: predots stimulation
	** estim = 2: dots stimulation 
	** estim = 3: postsac stimulation
	** estim = 4: postreward stimulation
	** estim = 5: continuous stim from target onset to sac onset, specified in menu as percent_dots=-1, precent_pre=-1
				this precludes other type of stimulation
	** ALSO NOTE that percent_stim in Property menu must be >0 for any stimulation to occur. 	
	*/
	if (TPMV(task, "percent_stim"))	{
		if ( (TSMV(task, "percent_pre")==-1) & (TSMV(task, "percent_dots")==-1) )	{
			estim_n = 2;
			estim = SAFE_ZALLOC(long,2);
			estim[0] = 0;
			estim[1] = 5;
		} else {
			estim_n = 5;
			estim = SAFE_ZALLOC(long, estim_n);
			for(i=0;i<estim_n;i++) {
				estim[i] = i;
			}
		}
	} else {
		estim_n = 1;
		estim = SAFE_ZALLOC(long,1);
		estim[0] = 0;
	}
		
	/* 
	** Make trial set for angle+coherence/reward contingency combinations
	*/
	tu_make_trials2D(task, "estim", estim_n, estim, 1.0, 
					"cohs_angle", cohs_angle_n, cohs_angle, 1.0);

	/* 
	** Free stuff
	*/
	SAFE_FREE(estim);
	SAFE_FREE(cohs_angle);
}


/* PUBLIC ROUTINE: dotsRTestim_get_random_byCoh
**
** Modified from tu_get_random_altRows_byCoh in task_utilitites.c Long Ding 2010-08-13
**    make a superblock by combining randomized trials according to the percent defined in menus
**		also specify how many trials per coherence level.
** Gets the next trial in a randomized block,
**    making the block if necessary.
**    NOTE: only to be used for the dotsRT task, routine assumes two items for each coherence level,
**          one for angle0, one for the other. also each column is for cohxangle and each row is
**          for a different reward contingency,
**
** Arguments:
**    task        ... See paradgim_rec for details.
**    num_blocks  ... Number of blocks (reps) of the set of trials
**                      that should be used when creating the
**                      pointer array.
**    reset_flag  ... If true, set the pointer back to the beginning
**                      after reaching the end of the array.
**    num_by_coh  ... flag indicating the numbers for each coh level, if 0, use Trial_blocks for all coh levels.
**                   For example, 111 means one x num_blocks trials per coherence level;
**                   5400 means no trials for the lowest two levels, 4 x num_blocks for the third level and 5 x for the fourth level,
**                      no trials for higher coherence levels.
*/
_PRtrial dotsRTestim_get_random_byCoh(_PRtask task, int num_blocks, int reset_flag, int num_by_coh)
{
   int last_score = task->pmf ? task->pmf->last_score : 0;
   int flagRepeat = pl_list_get_v(task->task_menus->lists[1], "flagRepeat");

   if(task->trialPs) {

      /* check previous score ... */

      if ( (last_score <= -1) || ( (last_score==0) && flagRepeat ) ) {

         /* always repeat fixbreak or no-choice trials , or error-trial with flagRepeat on */
         printf (" repeat trial \n");
         return(task->trialPs[task->trialPs_index]);
      }

      if ( (last_score<0) && !flagRepeat ) {

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
      }

   } else {

      /* force 'reset' first time through */
      reset_flag = 1;
   }

   /* check to make block */
   if(reset_flag && !task->trialPs) {

      int num_stim = task->trials_rows;
		int num_coh = task->trials_columns;
      int num_cols = 0;
		int num_blocks_new = 0;
      int i, j, k, m, n;
      int count_per_block = 0;
      int numTrialCoh[num_coh];
     	int numTrialStim[num_stim];  
		_PRtrial *finaltrialPs;

      /* figure out from num_by_coh how many trials per coherence level */
		if (num_by_coh==0) {
			for (i=0; i<num_coh; i++)
			{
				numTrialCoh[i] = 1;
			}
			num_cols = num_coh;
		} else {
	      for (i=0; i<num_coh; i++)
	      {
	         numTrialCoh[i] =  num_by_coh % 10 ;
	         num_by_coh = (num_by_coh - numTrialCoh[i])/10;
	         num_cols = num_cols + numTrialCoh[i];
	      }
		}
		/* figure out how many trials per stim condition, sum up to num_blocks */
		numTrialStim[0] = (100 - TPMV(task, "percent_stim") ) * num_blocks / 100.0;	
		n = num_blocks - numTrialStim[0];
		if (n>0) {
			if (num_stim==2) {
				numTrialStim[1] = n;
			} else {
				numTrialStim[1] = n * TSMV(task, "percent_pre") / 100.0;
				numTrialStim[2] = n * TSMV(task, "percent_dots") / 100.0;
				numTrialStim[3] = n * TSMV(task, "percent_postsac") /100.0;
				numTrialStim[4] = n * TSMV(task, "percent_reward") / 100.0;
			}
			n = 0;
			for (i=0; i<num_stim;i++)
			{
				n = n + numTrialStim[i];
			}
			num_blocks_new = n; 
		} else {
			num_blocks_new = num_blocks;
		}
      count_per_block = num_cols * num_blocks_new;
      /*
      ** Each block contains count_per_row randomized trials.
      */

      SAFE_FREE(task->trialPs);
      task->trialPs = SAFE_ZALLOC(_PRtrial, count_per_block);
      task->trialPs_length = count_per_block ;
      task->trialPs_index = 0;
      /* fill the array with repeated blocks of pointers
      ** to trials, in order
      */
      n = 0;

		for (i=0; i<num_stim; i++)
			for (j=0; j<numTrialStim[i];j++)		
				for (k=0; k<num_coh; k++)
					for (m=0; m<numTrialCoh[k]; m++)
					{
				printf("numTrialStim[%d]: %d, numTrialCoh[%d]: %d filling %d \n", 
					i, numTrialStim[i], k, numTrialCoh[k], i*num_cols+k);		
						task->trialPs[n] = task->trials[i*num_cols+k];
						n++;
					}	
      pr_task_randomize_trialP(task);

printf("# coh: %d num_stim %d num_blocks_new %d total %d \n", num_cols, num_stim, num_blocks_new, n);
      task->trialPs_length = n;
      task->trialPs_index = 0;
   }

   /* check if there is a list of pointers */
   if(!task->trialPs)
      return(NULL);

   /* return pointer to the current trial */
/*printf("dotsrt_estim_get_trial done \n");	*/
   return(task->trialPs[task->trialPs_index]);
}

/* PUBLIC ROUTINE: dotsrt_estim_get_trial
**
** Returns:
**		_PRtrial if trials remain
**		NULL otherwise
*/
_PRtrial dotsrt_estim_get_trial(_PRtask task, int reset_flag)	
{
	int trial_blocks = TPMV(task, "Trial_blocks");
	int num_by_coh = TPMV(task, "num_by_coh");
	return(dotsRTestim_get_random_byCoh(task, trial_blocks, reset_flag, num_by_coh));
}


/* PUBLIC ROUTINE: dotsrt_estim_set_trial
**
**	Note that here we mostly access groups/lists/properties
**	 by INDEX, not NAME, so be careful if anything
**	 above changes.
*/
void dotsrt_estim_set_trial(_PRtrial trial)
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
//	if (cohv<0 || ( cohv == 0 && rand()%2) ) 
	if (cohv<0)
		ang = angle+180;
	cohv = abs(cohv);
	if (cohv==1)
		cohv = 0;
	
printf("dotRT_estim trial  estim = %d angle = %d coh = %d Ecc = %d\n", 
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
			DX_Y, 		0,
			DX_DIAM,		PL_L2PWS(tgm, TU_XYRT_LEN, 1),
			DX_CLUT,		PL_L2PWS(tgm, TU_XYRT_LEN+1, 1),
			DX_COH, 		cohv,
			DX_DIR, 		ang,
 			DX_SPEED,  	PL_L2PVS(tdm, 1 ,0),
 			DX_SEED,  	tu_get_seed(PL_L2PVS(tdm, 0, 0), cohv, ang), 
 			NULL);

	/* Conditionally drop codes */
	if(TPMV(trial->task, "Drop_codes") != 0) {

		ec_send_code_tagged(I_ESTIMCD, 7000 + PL_L2PV(trial->list, 0));	/* send tagged ecode with estim type */

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
			DX_COH,		0, 	I_COHCD,
			DX_DIR,		0, 	I_DOTDIRCD,
			DX_SEED,		0, 	I_DTVARCD,
			NULL);
	}
/* printf("dotsrt_estim_set_trial done \n"); */
}


