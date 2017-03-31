/*
** task_countermand.c
**
** Single fixation point + single target,
**	used in countermand paradigm. Stop-signal
**	delays are given explicitly in menu.
*/

#include "../toys.h"
#include "../task.h"
#include "../task_utilities.h"

/* PRIVATE MACROS/CONSTANTS */
#define NUM_GR_MENUS 2
#define TI 				"setup"
#define TIMV(ta,n) 	pl_list_get_v((ta)->task_menus->lists[0],(n))
#define MAX_DELAYS	10

/* PRIVATE DATA STRUCTURES */

/* PRIVATE ROUTINES */
void 		cm_make_trials	(_PRtask);
_PRtrial cm_get_trial	(_PRtask, int);
void 		cm_set_trial	(_PRtrial);

/* PRIVATE VARIABLES */

/* 
** TASK CM
**		("CounterMand")
*/
struct _TAtask_struct gl_cm_task[] = {

	/* name */
	"cm",

	/* 
	** Menus 
	*/
	{
		/* 
		** Task info
		*/
		{ TI, 					1 },
		{ "Trial_order",		0, 1.0 },
		{ "Trial_blocks",		1, 1.0 },
   	{ "Angle_o",      	0, 1.0 },
   	{ "Angle_step",     	0, 1.0 },
   	{ "Angle_n",      	0, 1.0 },
   	{ "Stop_signal_0",   0, 1.0 },
   	{ "Stop_signal_1",   0, 1.0 },
   	{ "Stop_signal_2",   0, 1.0 },
   	{ "Stop_signal_3",   0, 1.0 },
   	{ "Stop_signal_4",   0, 1.0 },
   	{ "Stop_signal_5",   0, 1.0 },
   	{ "Stop_signal_6",   0, 1.0 },
   	{ "Stop_signal_7",   0, 1.0 },
   	{ "Stop_signal_8",   0, 1.0 },
   	{ "Stop_signal_9",   0, 1.0 },
		{ "Drop_codes",		1,	1.0 },
   	{ NULL },

		/*
		**	2 'standard' graphics objects:
		**		0 - fixation
		**		1 - target
		*/
		{ "graphics",		NUM_GR_MENUS },
		TU_XYRT_LIST,
   	{ DX_DIAM,			0, 0.1 },
   	{ DX_CLUT,			0, 1.0 },
   	{ NULL },

		/* END OF MENUS */
		{ NULL }
	},

	/* 
	** Graphics objects, by name
	*/
	{  { DX_TARGET,	2 }, 
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
	&cm_make_trials,
	&cm_get_trial,
	&cm_set_trial,
};

/* PUBLIC ROUTINE: cm_make_trials
**
*/
void cm_make_trials(_PRtask task)
{
	register int i;
   int *angs, ang_n, delays[MAX_DELAYS], delay_n=0;
	_PLproperty *props;

	/* 
	** Make array of angles
	*/
	ang_n = toy_boundi(TIMV(task, "Angle_n"), 1, 1000);
	angs  = toy_seriesi(ang_n, TIMV(task, "Angle_o"), 
					TIMV(task, "Angle_step"));

	/* Make array of stop-signal delays */
	props = &(task->task_menus->lists[0]->properties[
				pl_list_get_propi(task->task_menus->lists[0], "Stop_signal_0")]);
	for(i=0;i<MAX_DELAYS;i++,props++)
		if((*props)->values[0] != NULLI)
			delays[delay_n++] = (*props)->values[0];

	/*
	** utility to make trial set for coh/dir pairs
	*/
	tu_make_trials2D(task, 
			"angle", ang_n,   angs,   0.1,
			"ssd",	delay_n, delays, 1.0);

	/* 
	** Free stuff
	*/
	SAFE_FREE(angs);
}

/* PUBLIC ROUTINE: cm_get_trial
**
** Returns:
**		_PRtrial if trials remain
**		NULL otherwise
*/
_PRtrial cm_get_trial(_PRtask task, int reset_flag)
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

         printf("NO STAIRCASE\n");
			return(NULL);

      /* REPEAT */
      default:

			return(PR_TRIAL(task));
   }
}

/* PUBLIC ROUTINE: cm_set_trial
**
*/
void cm_set_trial(_PRtrial trial)
{
	_PLlist tgm = trial->task->task_menus->lists[1];
	valtype val = PL_L2PV(trial->list, 0);

	/* Make working copies of values in 
	**		graphics menu 
	*/
	pl_list_set_v2w(tgm);

	/* Conditionally fill-in angle ("r")
	** 	for graphics object 1 (target)
	*/
	PL_CSET( PL_L2PWS(tgm, kT, 1), 
				PL_L2PV(trial->list, 0) );

	/* compute x, y positions of
	**		fixation point (obj 0)
	**		target (obj 1)
 	*/
	tu_compute_xyrt(tgm);

	/* Set x, y positions in 
	**		actual dXtarget objects
	*/
	dx_set_by_indexIV(DXF_NODRAW, 0, -1, NULL, 
		1, PL_L2PWS(tgm, 0, 0), 				PL_L2PWS(tgm, 0, 1),
		2, PL_L2PWS(tgm, 1, 0), 				PL_L2PWS(tgm, 1, 1),
		3,	PL_L2PWS(tgm, TU_XYRT_LEN,   0), PL_L2PWS(tgm, TU_XYRT_LEN, 1),
		4,	PL_L2PWS(tgm, TU_XYRT_LEN+1, 0), PL_L2PWS(tgm, TU_XYRT_LEN+1, 1),
		ENDI);

	/* Conditionally drop codes */
	if(TIMV(trial->task, "Drop_codes") != 0)

		pl_ecodes_by_nameV(dx_get_object(DX_TARGET), 7000,
			DX_X,		0, 	I_FIXXCD,
			DX_Y,		0, 	I_FIXYCD,
			DX_CLUT,	0, 	I_FIXLCD,
			DX_X,		1, 	I_TRG1XCD,
			DX_Y,		1, 	I_TRG1YCD,
			DX_CLUT,	1, 	I_TRG1LCD,
			NULL);
}

