/*
** task_dotRT_stim_post.c
**
**	Long Ding, modified from task_dotRT_stim.c Nov 2010 
** for electrical stimulation post sac/reward
** use only two levels of task difficulty: specified in coh_lo and coh_hi
** each trial is followed by a 0% coherence trial
** to test for an effect of postdecision stimulation on next choice
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
void 		estim_post_make_trials	(_PRtask);
_PRtrial estim_post_get_trial	(_PRtask, int);
void 		estim_post_set_trial	(_PRtrial);

/* 
** TASK DOTSRT
**		("estim_post")
*/

struct _TAtask_struct gl_estim_post_task[] = {

	/* name */
	"estim_post",

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
      { "postsac_start",    0, 1.0 },
      { "postsac_end",      0, 1.0 },
      { "postrew_start",    0, 1.0 },
      { "postrew_end",      1, 1.0 },
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
	&estim_post_make_trials,
	&estim_post_get_trial,
	&estim_post_set_trial,
};


/* PUBLIC ROUTINE: estim_post_make_trials
**
*/
void estim_post_make_trials(_PRtask task)
{
  	static int coh_list[] = {0, 32, 64, 128, 256, 512, 999};
	int num_coh = 2,
		 *cohs=NULL, coh_n,
		 coh_lo, coh_hi;
	int *estim, estim_n;
	int i, j, k,  *cohs_angle=NULL, cohs_angle_n;
	 
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

		coh_n   = 2;
		cohs 	  = SAFE_ZALLOC(int, coh_n);
		cohs[0] = coh_lo; 
		cohs[1] = coh_hi; 

	
	cohs_angle_n = coh_n*2;
	cohs_angle = SAFE_ZALLOC(long, cohs_angle_n);
	for (i=0; i<coh_n; i++) {
		cohs_angle[2*i] = cohs[i];
		cohs_angle[2*i+1] = -cohs[i];
	}
	
	/* 
	** Make array of angles and reward contingencies
	**	estim = 0: no stimulation 
	** estim = 3: postsac stimulation
	** estim = 4: postreward stimulation
	** estim = 6: continuous stim from saccade onset to reward onset+500, specified in menu as percent_sac=-1, precent_rew=-1
				this precludes other type of stimulation
	** ALSO NOTE that percent_stim in Property menu must be >0 for any stimulation to occur. 	
	*/
	if (TPMV(task, "percent_stim")) {
      if ( (TSMV(task, "percent_postsac")==-1) & (TSMV(task, "percent_reward")==-1) )  {
         estim_n = 2;
         estim = SAFE_ZALLOC(long,2);
         estim[0] = 0;
         estim[1] = 6;
		} else {
			estim_n = 3;
			estim = SAFE_ZALLOC(long, estim_n);
         estim[0] = 0;
         estim[1] = 3;
			estim[2] = 4;
		}
	} else {
		estim_n = 1;
		estim = SAFE_ZALLOC(long,1);
		estim[0] = 0;
	}
		
	/* 
	** Make trial set 
	*/

	/* added the following codes from tu_make_trials2D */

   /* add the array of (empty) trials */
   pr_task_add_trials2D(task, estim_n, cohs_angle_n*2); /* doubled the 2nd number to add zero coh trials later */

   /* For each, add a list of 2 properties,
   ** corresponding to the values of the two
   ** variables.
   */
   for(k=0,i=0;i<estim_n;i++) {
      for(j=0;j<cohs_angle_n;j++,k++) {
         task->trials[k]->id = task->id*100+k;
         task->trials[k]->list =
            pl_list_initV("2dtc", 0, 1,
               "estim", estim[i], 1.0,
               "cohs_angle", cohs_angle[j], 1.0,
               NULL);
      }
   }
	/* 	add zero coherence, no estim trials
	**		the previous coh is indicated by adding 2000 
	**		the previous estim condition is indicated by a negative sign 
	*/
   for(i=0;i<estim_n;i++) {
      for(j=0;j<cohs_angle_n;j++,k++) {
         task->trials[k]->id = task->id*100+k;
         task->trials[k]->list =
            pl_list_initV("2dtc", 0, 1,
               "estim", -estim[i], 1.0,
               "cohs_angle", cohs_angle[j]+2000,1.0,
               NULL);
      }
   }

		/*	tu_make_trials2D(task, "estim", estim_n, estim, 1.0, 
							"cohs_angle", cohs_angle_n, cohs_angle, 1.0);
		*/
	/* 
	** Free stuff
	*/
	SAFE_FREE(estim);
	SAFE_FREE(cohs_angle);
}


/* PUBLIC ROUTINE: dotsRTestim_get_random_follow0
**
** Gets the next trial in a randomized block, each "good" trial followed by a zero coh trial; 
**		equal number of trials for estim conditions
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
*/
_PRtrial dotsRTestim_get_random_follow0(_PRtask task, int num_blocks, int reset_flag)
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
      int i, j, k, m, n;
      int count_per_block = 0;
		int half = num_stim * num_coh/2;
		int dummy[half*num_blocks];
		int countid[half*2];
	
		count_per_block = num_blocks * num_stim * num_coh;	
     	SAFE_FREE(task->trialPs);
     	task->trialPs = SAFE_ZALLOC(_PRtrial, count_per_block);
     	task->trialPs_length = count_per_block;
     	task->trialPs_index = 0;
		
printf("num_blocks %d num_coh %d num_stim %d \n", num_blocks, num_coh, num_stim);
		n = 0;	
		for (i=0, n=0; i<num_blocks; i++)
			for (j=0; j<half; j++, n++) {
					dummy[n] = j;
		}

printf("n = %d half = %d count_per_block = %d\n", n, half, count_per_block);
		k=0;
		for (i=n; i>0; i--) {
  		    /* get a random number between 0 and i-1 (index) */
			j = TOY_RAND(i);
			m = dummy[j];
			dummy[j] = dummy[i-1];	
			task->trialPs[k*2] = task->trials[m];
			task->trialPs[k*2+1] = task->trials[half+m];						
			k++;
		}	
		printf("\n");
/* debug */
/*		for (i=0; i<num_coh*num_stim;i++)
			countid[i] = 0;

		printf("trialPs_length = %d \n", task->trialPs_length);
		for (i=0;i<task->trialPs_length; i++) {
			printf("estim_post trial id= %d estim = %d cov_angle = %d \n",
   			task->trialPs[i]->id, 
				PL_L2PV(task->trialPs[i]->list, 0), 
				PL_L2PV(task->trialPs[i]->list, 1));

			countid[task->trialPs[i]->id-200]++;	
		}
		for (i=0; i<num_coh * num_stim; i++)
			printf("count id %d  = %d \n", i+200, countid[i]);

*/
   }

   /* check if there is a list of pointers */
   if(!task->trialPs)
      return(NULL);

   /* return pointer to the current trial */
printf("estim_post_get_trial done \n");	
   return(task->trialPs[task->trialPs_index]);
}

/* PUBLIC ROUTINE: estim_post_get_trial
**
** Returns:
**		_PRtrial if trials remain
**		NULL otherwise
*/
_PRtrial estim_post_get_trial(_PRtask task, int reset_flag)	
{
	int trial_blocks = TPMV(task, "Trial_blocks");
	return(dotsRTestim_get_random_follow0(task, trial_blocks, reset_flag));
}


/* PUBLIC ROUTINE: estim_post_set_trial
**
**	Note that here we mostly access groups/lists/properties
**	 by INDEX, not NAME, so be careful if anything
**	 above changes.
*/
void estim_post_set_trial(_PRtrial trial)
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
	if ( cohv>1000 )
		cohv = 0;
	ang = angle;
	if (cohv<0 || ( cohv == 0 && rand()%2) ) 
		ang = angle+180;
	cohv = abs(cohv);
	
printf("estim_post trial id= %d estim = %d angle = %d coh = %d Ecc = %d\n", 
	trial->id, PL_L2PV(trial->list, 0), ang, cohv, TPMV(trial->task, "tgtecc")  );

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
/* printf("estim_set_trial done \n"); */
}


