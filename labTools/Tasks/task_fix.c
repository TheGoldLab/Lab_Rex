/*
** task_fix.c
**
**	Simple fixation-only task.
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
void 		fix_make_trials	(_PRtask);
_PRtrial fix_get_trial	(_PRtask, int);
void 		fix_set_trial	(_PRtrial);

/* PRIVATE VARIABLES */

/* 
** TASK: FIX
**		("Fixation only")
*/
struct _TAtask_struct gl_fix_task[] = {

	/* name */
	"fix",

	/* 
	** Menus 
	*/
	{
		/* 
		** Task info
		*/
		{ TI, 			1 },
		TU_XYRT_LIST,
		{ DX_DIAM,		0, 1.0 },
		{ DX_CLUT,		0, 1.0 },
		{"Drop_codes",	0, 1.0 },
   	{ NULL },

		/* END OF MENUS */
		{ NULL }
	},

	/* 
	** Graphics objects, by name
	*/
	{  { DX_TARGET,	1 }, 
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
	&fix_make_trials,
	&fix_get_trial,
	&fix_set_trial,
};

/* PUBLIC ROUTINE: fix_make_trials
**
*/
void fix_make_trials(_PRtask task)
{
	int dummy=0;

	/* utility to make trial set.  */
	tu_make_trials1D(task, "value", 1, &dummy, 0);
}

/* PUBLIC ROUTINE: fix_get_trial
**
** Returns:
**		_PRtrial if trials remain
**		NULL otherwise
*/
_PRtrial fix_get_trial(_PRtask task, int reset_flag)
{
	return(tu_get_block(task, 1, reset_flag));
}

/* PUBLIC ROUTINE: fix_set_trial
**
*/
void fix_set_trial(_PRtrial trial)
{
	_PLlist tim = trial->task->task_menus->lists[0];

	/* compute x, y positions of
	**		fixation point (obj 0)
	**		target (obj 1)
 	*/
	tu_compute_xyrt(tim);

	/* Set x, y positions in actual dXtarget object
	*/
	dx_set_by_indexIV(DXF_NODRAW, 0, 0, NULL, 
		1, PL_L2PWS(tim, 0, 0),
		2, PL_L2PWS(tim, 1, 0),
		3,	PL_L2PWS(tim, TU_XYRT_LEN, 0),
		4,	PL_L2PWS(tim, TU_XYRT_LEN+1, 0),
		ENDI);

	/* Conditionally drop codes */
	if(TIMV(trial->task, "Drop_codes") != 0)

		pl_ecodes_by_nameV(dx_get_object(DX_TARGET), 7000,
			DX_X,		0, 	I_FIXXCD,
			DX_Y,		0, 	I_FIXYCD,
			DX_CLUT,	0, 	I_FIXLCD,
			NULL);
}

