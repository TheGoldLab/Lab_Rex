/*
** task_dots.c
**
**	Standard 2-alternative forced-choice
**		dots task
*/

#include "../toys.h"
#include "../task.h"
#include "../task_utilities.h"

/* PRIVATE MACROS/CONSTANTS */
#define NUM_GR_MENUS 6
#define TI 				"setup"
#define TIMV(ta,n)	pl_list_get_v((ta)->task_menus->lists[0], (n))

/* PRIVATE DATA STRUCTURES */

/* PRIVATE ROUTINES */
void 		dots_make_trials	(_PRtask);
_PRtrial dots_get_trial		(_PRtask, int);
void 		dots_set_trial		(_PRtrial);

/* PRIVATE VARIABLES */

/* 
** TASK: DOTS
**		1 dXdots object
**		3 dXtarget objects (fp, 2 targets)
*/
struct _TAtask_struct gl_dots_task[] = {

	/* name */
	"dots",

	/* menus */
	{
		/* 
		** Task info
		*/
		{ TI, 					1 },
		{ "Trial_order",		0, 1.0 },
		{ "Trial_blocks",		1, 1.0 },
   	{ "Object_1",     	0, 1.0 },
   	{ "Object_2",     	0, 1.0 },
   	{ "Angle_o",      	0, 1.0 },
   	{ "Angle_step",      0, 1.0 },
   	{ "Angle_n",      	0, 1.0 },
   	{ "Coherence_lo", 	0, 0.1 },
   	{ "Coherence_hi", 	0, 0.1 },
   	{ "Target_flag",  	0, 1.0 },
		{ "Stair_up",			0, 1.0 },
		{ "Stair_down",		0, 1.0 },
		{ "Stair_start",		0, 1.0 },
		{ "Target_flag",		0,	1.0 },
   	{ NULL },

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
	{  { "dXtarget",	3 }, 
		{ "dXdots",		1 }, 
		{ NULL }
	},

	/* 
	**	User functions 
	*/
	{	{ NULL }
	},

	/* 
	**	Task methods
	*/
	&dots_make_trials,
	&dots_get_trial,
	&dots_set_trial,
};

/* PUBLIC ROUTINE: dots_make_trials
**
*/
void dots_make_trials(_PRtask task)
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

/* PUBLIC ROUTINE: dots_get_trial
**
** Returns:
**		_PRtrial if trials remain
**		NULL otherwise
*/
_PRtrial dots_get_trial(_PRtask task, int reset_flag)
{
	int trial_blocks = TIMV(task, "Trial_blocks");

	switch(TIMV(task, "Trial_order")) {

		/* BLOCK */
		case 0:

			return(tu_get_block(task, trial_blocks, reset_flag));

		/* RANDOMIZE */
		case 1:

			return(tu_get_random(task, trial_blocks, reset_flag));

		/* STAIRCASE */
		case 2:

			printf("STAIRCASE\n");
			break;

		/* REPEAT */
		default:

			if(task->trialPs && task->trialPs_index >= 0 &&
				task->trialPs_index < task->trialPs_length)
				return(task->trialPs[task->trialPs_index]);
			else
				return(NULL);
	}
}

/* PUBLIC ROUTINE: dots_set_trial
**
**	Note that here we mostly access groups/lists/properties
**	 by INDEX, not NAME, so be careful if anything
**	 above changes.
*/
void dots_set_trial(_PRtrial trial)
{
	_PLlist 	tim = trial->task->task_menus->lists[0],
				tgm = trial->task->task_menus->lists[1],
				tdm = trial->task->task_menus->lists[2];

	int 		ob1 = PL_L2PV(tim, 1),
				ob2 = PL_L2PV(tim, 2),

				/* the following are arrays of indices 
				**		of target objects (fp, t1, t2) in
				**		the task graphics menu and the
				**		dXtargets objects, respectively	
				*/
				tti[] = {0, 3, 4},	
				gti[] = {0, 1, 2};

	/* make working copies of the task object menu values */
	pl_list_set_v2w(tgm);

	/* Error check object_1, object_2 values .. note
	**		that these are indices of values in the
	**		"Graphics" menu, above.
	*/
	if(ob1 < 0 || ob1 >= NUM_GR_MENUS)
		ob1 = 0;	/* ob1 default fp */
	if(ob2 < 0 || ob2 >= NUM_GR_MENUS)
		ob2 = 3;	/* ob2 default t1 */

	/* Conditionally copy values of trial/task
	**	 properties to working copies of task menus:
	**	1. Coherence (of dots) from trial (property 0)
	**	2. Angle 	 (of ob2)  from trial (property 1)
	**	3. Vertex	 (of ob2) = ob1
	*/
	PL_CSET( PL_L2PW( tdm,2), 	 	PL_L2PV(trial->list, 0));
	PL_CSET( PL_L2PWS(tgm,4,ob2), PL_L2PV(trial->list, 1));
	PL_CSET( PL_L2PWS(tgm,8,ob2), ob1);

	/* compute xys */
	tu_compute_xyrt(tgm);

	/* Copy "working copy" values of key parameters
	**		to graphics objects and automatically 
	**		send rSet command ...
	**
	**	First dXtargets, all at once ...
	*/
	dx_setl_by_nameIV(0, 1, 
		tgm, 'w',   3, tti, 
		"dXtarget", 3, gti,
		DX_X, 	NULL, 
		DX_Y, 	NULL,
		DX_DIAM,	NULL,
		DX_CLUT,	NULL,
		NULL);

	/* dXdots: here we first set some from the
	**		graphics menu, then the rest from the dots info
	**		menu. To make a single rSet command to send, we
	**		do it a little bit differently here:
	**
	**	1. Set from graphics menu to working copies 
	**			in dXdots object
	**	2. Set from dots info menu to working 
	**			copies in dXdots object
	**	3. Set and send from working copies of 
	**			dXdots object
	*/
	pl_lists_copyIV(
		tgm, 						'w', 5, NULL, 
		TR_DX_OBJ(trial, 1), 'w', 0, NULL,
		DX_X,		 	NULL,
		DX_Y,		 	NULL,
		DX_DIAM,	 	NULL,
		DX_CLUT,  	NULL,
		NULL);

	pl_lists_copyIV(
		tdm, 						'w', 0, NULL, 
		TR_DX_OBJ(trial, 1), 'w', 0, NULL,
		DX_COH, 		NULL,
		DX_DIR,		NULL,
		DX_SPEED,	NULL,
		DX_SEED,		NULL,
		NULL);

	dx_setw_by_nameIV(0, 2, "dXdots", 0, NULL, NULL);
}
