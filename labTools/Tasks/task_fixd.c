/*
** task_fixd.c
**
** task "fixation+dots".c ... 
**	 	standard fixation + one dots field
*/

#include "../toys.h"
#include "../task.h"
#include "../task_utilities.h"

/* PRIVATE MACROS/CONSTANTS */
#define NUM_GR_MENUS 3
#define TIMV(ta,n) 	pl_list_get_v((ta)->task_menus->lists[0],(n))

/* PRIVATE DATA STRUCTURES */

/* PRIVATE ROUTINES */
void 		fixd_make_trials	(_PRtask);
_PRtrial fixd_get_trial	(_PRtask, int);
void 		fixd_set_trial	(_PRtrial);

/* PRIVATE VARIABLES */

/* 
** TASK: fixd
**		("Fixation+dots")
**
**		1 dXdots object
**		1 dXtarget object
*/
struct _TAtask_struct gl_fixd_task[] = {

	/* name */
	"fixd",

	/* 
	** Menus 
	*/
	{
		/* 
		** Task info
		*/
		{ "Task info", 					1 },
			{ "Trial_order",			0, 1.0 },
			{ "Trial_blocks",		1, 1.0 },
	   		{ "Angle_o",      		0, 1.0 },
   			{ "Angle_step",     	0, 1.0 },
   			{ "Angle_n",      		0, 1.0 },
			{ "Coherence_lo", 	0, 0.1 },
			{ "Coherence_hi", 	0, 0.1 },
			{ "Drop_codes",		1,	1.0 },
   		{ NULL },

		
		/*
		**	3 'standard' graphics objects:
		**		0 - fixation
		**		1 - dots aperture
		**		2 - dots direction
		*/
		{ "graphics",		NUM_GR_MENUS },
			TU_XYRT_LIST,
   			{ DX_DIAM,			0, 0.1 },
   			{ DX_CLUT,			0, 1.0 },
   		{ NULL },


		/*
		** One dots object
		*/
		{ "dots",		 1 },
			{ DX_SEED,		 0, 0.1 },
			{ DX_SPEED,		 0, 0.1 },
			{ DX_COH,    	 0, 0.1 },
			{ DX_DIR,		 0, 0.1 },
		{ NULL },


		/* END OF MENUS */
		{ NULL }
	},

	/* 
	** Graphics objects, by name
	*/
	{  { "dXtarget",	1 }, 
		{ "dXdots",		1 }, 
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
	&fixd_make_trials,
	&fixd_get_trial,
	&fixd_set_trial,
};

/* PUBLIC ROUTINE: fixd_make_trials
**
*/
void fixd_make_trials(_PRtask task)
{
   static   int coh_list[] = {0, 32, 64, 128, 256, 512, 999};
   int  			 num_coh = sizeof(coh_list)/sizeof(int),
					 *cohs=NULL, *angs, coh_n,
					 coh_lo, coh_hi, ang_n;

   /* Figure out which coherences we'll use.
   ** Set using "Coherence_lo" and "Coherence_hi"
   **    in the task_menu struct. Note that
   **    positive values establish the range of
   **    coherence chosen from the static coh_list;
   **    a negative Coherence_lo means use just
   **    the two values (Coherence_lo and Coherence_hi).
   */
	coh_lo = TIMV(task, "Coherence_lo");
	coh_hi = TIMV(task, "Coherence_hi");

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
   	register int i, ilo = -1, ihi = -1;

      while(++ilo < num_coh-1 && coh_list[ilo] < coh_lo) ;
      while(++ihi < num_coh-1 && coh_list[ihi] < coh_hi) ;

		coh_n = ihi - ilo + 1;
		cohs  = SAFE_ZALLOC(long, coh_n);
		for(i=0;i<coh_n;i++,ilo++)
			cohs[i] = coh_list[ilo];
   }

	/* 
	** Make array of angles
	*/
	ang_n = toy_boundi(TIMV(task, "Angle_n"), 1, 1000);
	angs  = toy_seriesi(ang_n, 
					TIMV(task, "Angle_o"), 
					TIMV(task, "Angle_step"));

	/*
	** utility to make trial set for coh/dir pairs
	*/
	tu_make_trials2D(task, 
			DX_COH,	coh_n, cohs, 0.1,
			"angle",	ang_n, angs, 0.1);

	/* 
	** Free stuff
	*/
	SAFE_FREE(cohs);
	SAFE_FREE(angs);

}

/* PUBLIC ROUTINE: fixd_get_trial
**
** Returns:
**		_PRtrial if trials remain
**		NULL otherwise
*/
_PRtrial fixd_get_trial(_PRtask task, int reset_flag)
{
   int trial_blocks = TIMV(task, "Trial_blocks");

   switch(TIMV(task, "Trial_order")) {

      /* BLOCK */
      case kTrialBlock:

			return(tu_get_block(task, trial_blocks, reset_flag));

      /* RANDOMIZE */
      case kTrialRandomize:

         return(tu_get_random(task, trial_blocks, reset_flag));

      /* STAIRCASE */
      case kTrialStaircase:

         printf("STAIRCASE\n");
         break;

      /* REPEAT */
      default:

			return(PR_TRIAL(task));
   }
}

/* PUBLIC ROUTINE: fixd_set_trial
**
*/
void fixd_set_trial(_PRtrial trial)
{
	_PLlist 	tim = trial->task->task_menus->lists[0],
				tgm = trial->task->task_menus->lists[1],
				tdm = trial->task->task_menus->lists[2];

	int dxtarget_indices[1] = {0};
	int dxdots_indices[1]   = {0};


	/* Make working copies of values in 
	**		graphics menu 
	*/
	pl_list_set_v2w(tgm);
	pl_list_set_v2w(tdm);


	/* Conditionally copy values of trial/task
	**	 properties to working copies of task menus:
	**	1. Coherence (of dots) from trial (property 0)
	**  2. Dot direction
	*/
	PL_CSET( PL_L2PW( tdm,2), 	 	PL_L2PV(trial->list, 0));
	PL_CSET( PL_L2PW( tdm,3), 	 	PL_L2PV(trial->list, 1));
		

	/* compute x, y positions of
	*/
	tu_compute_xyrt(tgm);


	/* Set x, y positions in 
	**		actual dXtarget objects
	**	Note: the indices here are based on the graphics objects
	**		fixation point (obj 0)
	*/
	dx_set_by_indexIV(DXF_NODRAW, 0, 1, dxtarget_indices, 
		1, PL_L2PWS(tgm, 0, 0), 
		2, PL_L2PWS(tgm, 1, 0), 
		3, PL_L2PWS(tgm, TU_XYRT_LEN, 0), 	
		4, PL_L2PWS(tgm, TU_XYRT_LEN+1, 0), ENDI);

	/* Second, set properties of the dXdots object */

	dx_set_by_nameIV(DXF_D1, DX_DOTS, 1, dxdots_indices,
			DX_X,			PL_L2PWS(tgm, 0, 1),
			DX_Y,			PL_L2PWS(tgm, 1, 1),
			DX_DIAM,		PL_L2PWS(tgm, TU_XYRT_LEN, 1),
			DX_CLUT,		PL_L2PWS(tgm, TU_XYRT_LEN+1, 1),
			DX_COH, 		PL_L2PW(tdm, 2),
			DX_DIR, 		PL_L2PW(tdm, 3),
 			DX_SPEED,  	PL_L2PW(tdm, 1),
 			DX_SEED,     tu_get_seed(PL_L2PW(tdm, 0), PL_L2PW(tdm,2), PL_L2PW(tdm,3)) ,	
			NULL);

//printf("%d, %d, %d, %d\n", PL_L2PW(tdm, 1), PL_L2PW(tdm, 2), PL_L2PW(tdm, 3), PL_L2PW(tdm, 4));


	/* Conditionally drop codes */
	if(TIMV(trial->task, "Drop_codes") != 0){

		pl_ecodes_by_nameV(dx_get_object(DX_TARGET), 7000,
			DX_X,		0, 	I_FIXXCD,
			DX_Y,		0, 	I_FIXYCD,
			DX_CLUT,	0, 	I_FIXLCD,
			NULL);
			
		pl_ecodes_by_nameV(dx_get_object(DX_DOTS), 7000,
			DX_X,		0,	I_STXCD,
			DX_Y,		0,	I_STYCD,
			DX_DIAM,	0,	I_STDIACD,
			DX_COH,		0, 	I_COHCD,
			DX_DIR,		0, 	I_DOTDIRCD,
			DX_SPEED,	0,	I_SPDCD,
			DX_SEED,	0, 	I_DTVARCD,
			NULL);
	}
	
}

