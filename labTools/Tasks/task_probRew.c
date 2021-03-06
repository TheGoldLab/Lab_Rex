/*
** task_probRew.c
**
** task "probabilistic reward".c ... 
**
** PREFIX: prr
**
** 2012/09/13 yl: commented out some print stuff
** 2012/12/18 yl: commented out more print stuff
** 2013/05/27 yl: forces changepoint after maxRunLength
*/

#include "../toys.h"
#include "../task.h"
#include "../task_utilities.h"

/* PRIVATE MACROS/CONSTANTS */
#define NUM_GR_MENUS 	3           /* make 3 menus*/
#define TI 					"setup"     /* Call the task info menu "setup"  */
#define TIMV(ta,n) 		pl_list_get_v((ta)->task_menus->lists[0],(n))
#define TIMW(ta,n) 		pl_list_get_w((ta)->task_menus->lists[0],(n))
#define STIMW(ta,n,v) 	pl_list_set_w((ta)->task_menus->lists[0],(n),(v))
#define BIMV(ta,n,i) 	pl_list_get_vi((ta)->task_menus->lists[1],(n),(i))
#define BIMW(ta,n,i) 	pl_list_get_wi((ta)->task_menus->lists[1],(n),(i))
#define SBIMW(ta,n,i,v) pl_list_set_wi((ta)->task_menus->lists[1],(n),(i),(v))
#define NUM_BLOCK_MENUS 6

/* PRIVATE DATA STRUCTURES */

/* PRIVATE ROUTINES */
void 		prr_make_trials		(_PRtask);
_PRtrial prr_get_trial			(_PRtask, int);
void 		prr_set_trial			(_PRtrial);
void 		prr_clear_user_data	(_PRtask);

/* PRIVATE VARIABLES */

int prevTargBest = -1;
int runLength = 0;

/* 
** TASK:probRew
**		("probabilistic reward")
*/
struct _TAtask_struct gl_prr_task[] = {

	/* name */
	"probRew",

	/* 
	** Menus 
	*/
	{
		/* 
		** Task info
		*/
		{ TI, 					1 },
		{ "block_order",		0,  1.0 },
		{ "block_size",		1,  1.0 },
    	
    	{ "num_targets", 		1,  1.0},
    	{ "target_i",			1,  1.0},
		{ "target_distance",	0,  1.0}, 
    	{ "target_diam",		1,  1.0},
		{ "target_clut",		0,  1.0}, 

		{ "choice_diam",		0,  1.0},
		{ "choice_clut",		0,  1.0},

		{ "rewarded_diam",	0,  1.0},
		{ "rewarded_clut",	0,  1.0},

		{ "Override", 			0,  1.0},

		{ "Drop_codes",		1,	 1.0 },
   	{ NULL },

		/*
		** BLOCK MENU
		*/
		{ "block",				NUM_BLOCK_MENUS },
		{ "block_size",		0, 	1.0},

		{ "fp_clut",			0, 	1.0},
		{ "fp_size",			0, 	1.0},
		{ "fp_shape",			0, 	1.0},

		{ "hazard",				0, 	1.0},

		{ "rew_time",			0, 	1.0},
		{ "rew_num",			0, 	1.0},
		{ "rew0_time",			0, 	1.0},
		{ "rew0_num",			0, 	1.0},

		{ "std",					0, 	1.0},

		{ "p0",					0, 	1.0},
		{ "p1",					0, 	1.0},
		{ "p2",					0, 	1.0},
		{ "p3",					0, 	1.0},
		{ "p4",					0, 	1.0},
		{ "p5",					0, 	1.0},
		{ "p6",					0, 	1.0},
		{ "p7",					0, 	1.0},
		{ "p8",					0, 	1.0},
		{ "p9",					0, 	1.0},
		{ "p10",					0, 	1.0},
		
		{ "maxRunLen",  40, 1.0},
		{ NULL },

		/* END OF MENUS */
		{ NULL }
	},

	/* 
	** Graphics objects, by name
	** target 1 = fixation point
	** target 2 = point of gaze
	** TARGETS= foraging targets
	*/
	{  { DX_TARGET,	3},
	   { DX_TARGETS,	1},
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
	&prr_make_trials,
	&prr_get_trial,
	&prr_set_trial,
	&prr_clear_user_data,
};

/* PUBLIC ROUTINE: prr_make_trials
**
*/
void prr_make_trials(_PRtask task)
{
   int *angs, ang_i, ang_n, *blocks, *bp, block_n, fpc; 
	register int i;
	_PRRuser_data ud;

	/* 
	** get relevant information from task menu:
	*/

	ang_n = TIMV(task, "num_targets");
	ang_i = TIMV(task, "target_i");	
	angs  = toy_seriesi(ang_n, ang_i, 360/ang_n);

	/* Use fp_clut field to determine used blocks
	**
	** A little ugly -- go through once to count the
	**		number of good blocks, then allocate the 
	**		array, then go through again to fill
	**		the array with block indices
	*/
	block_n = 0;
	for(i=0;i<NUM_BLOCK_MENUS;i++) {
		/* printf("%d: fp_clut=%d\n", i, BIMV(task, "fp_clut", i)); */
		if(BIMV(task, "fp_clut", i)>=0)
			block_n++;
	}
	if(block_n == 0) {
		return;
	}

	bp = blocks = SAFE_ZALLOC(int, block_n);
	for(i=0;i<NUM_BLOCK_MENUS;i++) {
		if(BIMV(task, "fp_clut", i)>=0)
			*bp++ = i;
	}

	printf("MAKE TRIALS: made %d angles, %d blocks\n", ang_n, block_n);

	/* 
	** Make one trial-type for each angle/block combo
	*/
	tu_make_trials2D(task, "angle", ang_n, angs, 0.1, 
							"block", block_n, blocks, 1.0);

	printf("MAKE TRIALS: done with 2d\n");

	/* save user data */
	ud = SAFE_STALLOC(_PRRuser_data_struct);
	ud->num_targets = ang_n;
	ud->angles 	    = SAFE_ZALLOC(double, ang_n);
	for(i=0;i<ang_n;i++)
		ud->angles[i] = (double) angs[i];
	ud->xs 			 = NULL;
	ud->ys 			 = NULL;
	task->user_data = ud;

	/* 
	** Free stuff
	*/
	SAFE_FREE(angs);
	SAFE_FREE(blocks);
}

/* PUBLIC ROUTINE: prr_get_trial
**
** Returns:
**		_PRtrial if trials remain
**		NULL otherwise
*/
_PRtrial prr_get_trial(_PRtask task, int reset_flag)
{
	/* block size is number of trials used in a given block
	**		A value set in the task info menu applies to all blocks
	**		Otherwise it must be set by hand in each block menu	
	**
	** block order is:  0 = random
	**						 -1 = descending
	**						  1 = ascending
	**
	**	The user data struct (defined in task.h) includes:
	**		block_count: the number of trials that
	**			have thus far been used in the current block
	**		block_index: the index of the current block (i.e., the
	**			actual index in the 2D trial array in the task struct)
	**		block_menu_index: the index of the relevant menu, which 
	**			may or may not be the same as the current block index
	**						
	*/
   int override = TIMV(task, "Override"),
		 num_blocks, last_score = task->pmf ? task->pmf->last_score : -1;
	_PRRuser_data ud = (_PRRuser_data) (task->user_data);
	int maxRunLength = BIMV(task,"maxRunLen", ud->block_menu_index);
	register int i;
	_PRtrial *tp;

	/* be nice */
	if (task==NULL || task->trials==NULL || task->trials_columns<=0)
		return(NULL);

	/* num blocks is just the number of columns in the 2D
	** 	trial array in the task struct
	*/
	num_blocks = task->trials_columns;
	
	/* Create a new block if:
	** 1. None exists
	** 2. Given reset_flag = true
	**	3. Reached end of last block	
	*/
	if ((task->trialPs == NULL) || (reset_flag > 0)) {
		ud->block_count 		= 0;
		ud->block_index 		= 0;
		ud->block_menu_index = 0;
	}

	if (ud->block_count <= 0) {

		printf("block count = 0\n");	

		/* free exising array, if it exists  */
		SAFE_FREE(task->trialPs);

		/* Choose new block, based on "block_order" 
		** field of Task Info Menu 
		*/
		switch(TIMV(task, "block_order")) {

			case 0: 	/* random */
				ud->block_index = TOY_RAND(num_blocks);
				break;

			case -1: /* descending */
				if(--(ud->block_index) < 0)
					ud->block_index = num_blocks-1;
				break;

			case 1: /* ascending */
				if(++(ud->block_index) == num_blocks)
					ud->block_index = 0;
				break;
		}

		/* make trialPs array  */
		pr_task_make_trialP_column(task, ud->block_index, 1);
		
		/* find block_menu_index as property of first triala */
		if(task->trialPs_length>0)
					ud->block_menu_index = pl_list_get_vi(task->trialPs[0]->list,
				"block", 0);
		else
			ud->block_menu_index = 0;

		printf("block menu index is %d , block index is %d \n", ud->block_menu_index, ud->block_index);

		/* set repetition property of each trial to 1  */
		for(i=0;i<task->trialPs_length;i++)
			task->trialPs[i]->repetitions = 1;
	
		/* reset block count to current block count, which can
		** be defined either in the Task Menu or the Block Menu
		*/
   	ud->block_count = TIMV(task, "block_size");
		if (ud->block_count <= 0)
			ud->block_count = BIMV(task, "block_size", ud->block_menu_index);

		printf("block count is %d\n", ud->block_count);

		/* randomly pick start trial */
		task->trialPs_index = TOY_RAND(task->trialPs_length);

		printf("start trial index is %d\n", task->trialPs_index);
   	 /*   printf("block menu index = %d \n",  ud->block_menu_index ); */
   
		/* send code to indicate block change */
		ec_send_code(BLOCKCD);

	} else {

		/* Decrement block count  */
		ud->block_count--;
	}

	/* pick trial from block 
	** 1. Use override if there
	**	2. Otherwise: (a) use Hazard to choose or (b) maxRunLength
	*/
	if ((override >= 0) && (override <= task->trialPs_length)) {

		task->trialPs_index = override;

		printf("GET TRIAL: override %d \n", override);

	} else if (last_score >= 0) {

		/*	HAZARD is the block-specific hazard rate
		**		given as number between 0 and 1000 ...
		** 	if a random number is smaller than the given hazard,
		** 	then pick a new index at random
		*/
	
/*	printf("hazard = %d l\n", BIMV(task, "hazard", ud->block_menu_index)); */

   	if (TOY_RAND(1000) <= BIMV(task, "hazard", ud->block_menu_index) | runLength >= maxRunLength) {
			task->trialPs_index = TOY_RAND(task->trialPs_length);
/*			ec_send_code(CHNGPT); */  /* COMMENTED OUT YL 2012/09/13 - doesn't seem to be defined */

			printf("GET TRIAL: changepoint\n");
		}
	}
	
	/* print out the runlength */
	if(task->trialPs_index != prevTargBest) {
		runLength = 1;
		prevTargBest = task->trialPs_index;
	} else if (last_score >= 0) {
		runLength++;
	}
	printf("GET TRIAL: runLength (max) = %d (%d)\n", runLength, maxRunLength);
	
/*	   printf("block menu index = %d l\n",  ud->block_menu_index ); */
   
/*
**	printf("GET TRIAL: index=%d, angle=%d, std=%d\n", 
**			task->trialPs_index, 
**			(int) pl_list_get_v(task->trialPs[task->trialPs_index]->list, "angle"),
**			(int) pl_list_get_v(task->trialPs[task->trialPs_index]->list, "std"));
*/
   	return(task->trialPs[task->trialPs_index]);

	
}


/* PUBLIC ROUTINE: prr_set_trial
**
*/
void prr_set_trial(_PRtrial trial)
{ 
	


	_PRtask task  = trial->task;
	_PRRuser_data ud = (_PRRuser_data) (task->user_data);
	int fp_size			  = BIMV(task, "fp_size",  ud->block_menu_index), 
		 fp_clut		     = BIMV(task, "fp_clut",  ud->block_menu_index), 
		 fp_shape		  = BIMV(task, "fp_shape", ud->block_menu_index), 
		 choice_diam 	  = TIMV(task, "choice_diam"), 
		 rewarded_diam   = TIMV(task, "rewarded_diam"), 
		 fixation_clut   = TIMV(task, "fixation_clut"), 
		 choice_clut	  = TIMV(task, "choice_clut"), 
		 rewarded_clut	  = TIMV(task, "rewarded_clut"), 
		 target_distance = TIMV(task, "target_distance"); 
	register int i;
	
	/*
	 printf("ENTER set trial block menu fp_size working copy = %d \n",  BIMW(task, "fp_size",  ud->block_menu_index) );
     printf("fp_size value = %d \n",  fp_size );
     printf("block menu index = %d \n",  ud->block_menu_index );
     */
     


	/* Decide whether stimulus parameters have changed
	*/
	/* don't check this .. just update on each trial ... 2013.08.13 yl */
	/*
	if(fp_size			!= BIMW(task, "fp_size",  ud->block_menu_index) ||
		fp_clut			!= BIMW(task, "fp_clut",  ud->block_menu_index) ||
		fp_shape			!= BIMW(task, "fp_shape", ud->block_menu_index)	||
		choice_diam 	!= TIMW(task, "choice_diam") 		||
		choice_clut		!= TIMW(task, "choice_clut")) {
*/
/*
	 printf("Enter stim parameters changed... \n" ); 
	*/
		
	/*	dx_set_by_nameIV(0, DX_TARGET, -1, NULL, 
			DX_DIAM,  fp_size,  rewarded_diam, choice_diam, 
			DX_CLUT,  fp_clut,  rewarded_clut, choice_clut, 
			DX_SHAPE, fp_shape, 0, 			 	  0,
			NULL);   */
			
		dx_set_by_nameIV(0, DX_TARGET, -1, NULL, 
			DX_DIAM,  fp_size,  rewarded_diam, choice_diam, 
			DX_CLUT,  fp_clut,  rewarded_clut, choice_clut, 
			NULL);	
			
			
 
 /*		printf("Stimulus parameters changed\n"); */

		/* set working copies, to tag that these have been udpated */
		SBIMW(task, "fp_size",  ud->block_menu_index, fp_size);
		SBIMW(task, "fp_clut",  ud->block_menu_index, fp_clut);
		SBIMW(task, "fp_shape", ud->block_menu_index, fp_shape);
		STIMW(task, "choice_diam", choice_diam);
		STIMW(task, "choice_clut", choice_clut);
/*	}*/

	/* Now decide whether stimulus GEOMETRY has changed
	** 	(i.e., new call to make_trials)
	*/
	if(ud->xs == NULL ||
		target_distance != TIMW(task, "target_distance") ) {

/*	printf("Stimulus geometry changed\n"); */


		ud->xs   = SAFE_ZALLOC(double, ud->num_targets);
		ud->ys   = SAFE_ZALLOC(double, ud->num_targets);
		for (i=0;i<ud->num_targets;i++) {
			ud->xs[i] = 
				TOY_RT_TO_XF(0, target_distance/10., ud->angles[i]);
			ud->ys[i] = 
				TOY_RT_TO_YF(0, target_distance/10., ud->angles[i]);
		}

		dx_set_array_by_nameIV(0, DX_TARGETS, 0,
			DX_X, 	(ud->num_targets), 	ud->xs,
			DX_Y, 	(ud->num_targets), 	ud->ys,
			DX_CLUT,	1,		  				(double) TIMV(task, "target_clut"), 
			DX_DIAM,	1,		  				((double) TIMV(task, "target_diam"))/10., 
			"numTargets", 1, 				((double) (ud->num_targets)),
			NULL);

		/* set working copies in menu */
		STIMW(task, "target_distance", target_distance);
	}

	/* Conditionally drop codes */
	if(TIMV(task, "Drop_codes") != 0) {

#define BASE 7000

		/* graphics codes */
		pl_ecodes_by_nameV(dx_get_object(DX_TARGET), BASE,
			DX_DIAM,	0, I_FIXDCD,
			DX_CLUT,	0, I_FIXLCD,
			DX_DIAM,	1, I_CTGDCD,
			DX_CLUT,	1, I_CTGLCD,
			NULL);

		/* DROP trial-specific codes
		*/
		ec_send_code_tagged(I_PRNT,   BASE + ud->num_targets);
		ec_send_code_tagged(I_PRTI,   BASE + TIMV(task, "target_i"));
		ec_send_code_tagged(I_PRTD,   BASE + target_distance);
		ec_send_code_tagged(I_PRTHR,  BASE + BIMV(task, "hazard", ud->block_menu_index));
		ec_send_code_tagged(I_PRTANG, BASE + (long) pl_list_get_v(trial->list,"angle"));
	   
	    /* Drop probability ecodes... added by MRN 01/25/11
	    */
	    
 
	  ec_send_code_tagged( I_PRTSTD,   BASE +  BIMV(task, "std", ud->block_menu_index));   
      ec_send_code_tagged(I_PRTP0,   BASE +  BIMV(task, "p0", ud->block_menu_index)); 
	  ec_send_code_tagged(I_PRTP1,   BASE +  BIMV(task, "p1", ud->block_menu_index)); 
      ec_send_code_tagged(I_PRTP2,   BASE +  BIMV(task, "p2", ud->block_menu_index));
      ec_send_code_tagged(I_PRTP3,   BASE +  BIMV(task, "p3", ud->block_menu_index)); 
	  ec_send_code_tagged(I_PRTP4,   BASE +  BIMV(task, "p4", ud->block_menu_index)); 
	  ec_send_code_tagged(I_PRTP5,   BASE +  BIMV(task, "p5", ud->block_menu_index)); 
	  ec_send_code_tagged(I_PRTP6,   BASE +  BIMV(task, "p6", ud->block_menu_index)); 
	  ec_send_code_tagged(I_PRTP7,   BASE +  BIMV(task, "p7", ud->block_menu_index)); 
	  ec_send_code_tagged(I_PRTP8,   BASE +  BIMV(task, "p8", ud->block_menu_index)); 
	  ec_send_code_tagged(I_PRTP9,   BASE +  BIMV(task, "p9", ud->block_menu_index)); 
	  ec_send_code_tagged(I_PRTPA,   BASE +  BIMV(task, "p10", ud->block_menu_index)); 

	
	
	}
}

void prr_clear_user_data(_PRtask task)
{
	_PRRuser_data ud;
	
	if(task->user_data == NULL)
		return;

 	ud = (_PRRuser_data) (task->user_data);
	SAFE_FREE(ud->angles);
	SAFE_FREE(ud->xs);
	SAFE_FREE(ud->ys);

	SAFE_FREE(ud);
}
