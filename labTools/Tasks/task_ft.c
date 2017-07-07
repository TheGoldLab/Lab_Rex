/*
** task_ft.c
**
** task "fixation-target".c ... 
**	 	standard fixation + 1-target tasks
**			(e.g., overlap and memory saccades)
*/

#include "../toys.h"
#include "../task.h"
#include "../task_utilities.h"

/* PRIVATE MACROS/CONSTANTS */
#define NUM_GR_MENUS 2
#define TI 				"setup"
#define TIMV(ta,n) 	pl_list_get_v((ta)->task_menus->lists[0],(n))

/* PRIVATE DATA STRUCTURES */

/* PRIVATE ROUTINES */
void 		ft_make_trials	(_PRtask);
_PRtrial ft_get_trial	(_PRtask, int);
void 		ft_set_trial	(_PRtrial);

/* PRIVATE VARIABLES */

/* 
** TASK: FT
**		("Fixation-Target")
*/
struct _TAtask_struct gl_ft_task[] = {

	/* name */
	"ft",

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
		{ "Fixation_task",	0, 1.0 },
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
	&ft_make_trials,
	&ft_get_trial,
	&ft_set_trial,
};

/* PUBLIC ROUTINE: ft_make_trials
**
*/
void ft_make_trials(_PRtask task)
{
   int *angs, ang_n;

	/* 
	** Make array of angles
	*/
	ang_n = toy_boundi(TIMV(task, "Angle_n"), 1, 1000);
	angs  = toy_seriesi(ang_n, TIMV(task, "Angle_o"), 
					TIMV(task, "Angle_step"));

	/*
	** utility to make trial set for coh/dir pairs
	*/
	tu_make_trials1D(task, "angle", ang_n, angs, 0.1);

	/* 
	** Free stuff
	*/
	SAFE_FREE(angs);
}

/* PUBLIC ROUTINE: ft_get_trial
**
** Returns:
**		_PRtrial if trials remain
**		NULL otherwise
*/
_PRtrial ft_get_trial(_PRtask task, int reset_flag)
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

	/* HAZARD-DEPENDENT CHANGE
	case kTrialHazard:

		if(PR_TRIAL(task)==NULL || TOY_RAND(1000)<TIMV(task, “Hazard”))
			return(tu_get_random(task, trial_blocks, reset_flag));
		else
			return(PR_TRIAL(task));

      /* REPEAT */
      default:
		if(PR_TRIAL(task)==NULL)
			return(tu_get_random(task, trial_blocks, reset_flag));
		else
			return(PR_TRIAL(task));
   }
}

/* PUBLIC ROUTINE: ft_set_trial
**
*/
void ft_set_trial(_PRtrial trial)
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
		3,	PL_L2PWS(tgm, TU_XYRT_LEN, 0), 	PL_L2PWS(tgm, TU_XYRT_LEN, 1),
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

