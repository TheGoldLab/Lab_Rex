/*
** task_ftt.c
**
** task "fixation-target-target".c ... 
**	 	standard fixation + 2-target tasks
**			(e.g., overlap and memory saccades)
*/

#include "../toys.h"
#include "../task.h"
#include "../task_utilities.h"

/* PRIVATE MACROS/CONSTANTS */
#define NUM_GR_MENUS 3
#define TI 			"setup"
#define TIMV(ta,n) 	pl_list_get_v((ta)->task_menus->lists[0],(n))

/* PRIVATE DATA STRUCTURES */

/* PRIVATE ROUTINES */
void 		ftt_make_trials	(_PRtask);
_PRtrial    ftt_get_trial	(_PRtask, int);
void 		ftt_set_trial	(_PRtrial);

/* PRIVATE VARIABLES */

/* 
** TASK: FT
**		("Fixation-Target")
*/
struct _TAtask_struct gl_ftt_task[] = {

	/* name */
	"ftt",

	/* 
	** Menus 
	*/
	{
		/* 
		** Task info
		*/
		{ TI, 				1 },
        { "Hazard1",      	0, 1.0 },
        { "Hazard2",        0, 1.0 },
        { "Oddball_diam"    0, 0.1 }, 
        { "Oddball_shape"   0, 1.0 }, 
        { "Oddball_clut"    0, 1.0 }, 
		{ "Fixation_task",	0, 1.0 },
		{ "Drop_codes",		1, 1.0 },
        { NULL },

		/*
		**	3 'standard' graphics objects:
		**		0 - fixation
		**		1 - target #1
        **      2 - target #2
		*/
		{ "graphics",		NUM_GR_MENUS },
        { DX_X,             0, 0.1 }, 
        { DX_Y,             0, 0.1 }, 
        { DX_DIAM,			0, 0.1 },
        { DX_CLUT,			0, 1.0 },
        { DX_SHAPE,         0, 1.0 },
        { NULL },

		/* END OF MENUS */
		{ NULL }
	},

	/* 
	** Graphics objects, by name
	*/
	{  { DX_TARGET,	3 }, 
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
	&ftt_make_trials,
	&ftt_get_trial,
	&ftt_set_trial,
};

/* PUBLIC ROUTINE: ftt_make_trials
**
*/
void ftt_make_trials(_PRtask task)
{
   int vals[2] = {0,1};

	/*
	** Just add two trials, one per target
	*/
	tu_make_trials1D(task, "target", 2, vals, 0.1);
}

/* PUBLIC ROUTINE: ftt_get_trial
**
** Use given Hazard rates to choose next trial
**
** Returns:
**		_PRtrial if trials remain
**		NULL otherwise
*/
_PRtrial ftt_get_trial(_PRtask task, int reset_flag)
{
    double hazard;
    
    /* Check if trialP array exists...
     * if not, make it with just the two trials and randomly pick one
     */
    if(!task->trialPs) {
        pr_task_make_trialP_all(task, 1);
        task->trialPs_index = TOY_RAND(2);
    }
    
    /* Get & test current hazard */
    hazard = task->trialPs_index ? TIMV("Hazard2") : TIMV("Hazard1");
    if(TOY_RCMP(hazard))
        task->trialPs_index = task->trialPs_index == 0 ? 1 : 0;
        
    /* return correct trial */
    return(task->trialPs(task->trialPs_index));
}

/* PUBLIC ROUTINE: ftt_set_trial
**
*/
void ftt_set_trial(_PRtrial trial)
{
	_PLlist tgm = trial->task->task_menus->lists[1];

    /* Set object properties .. use real values (not working copies) 
     * for 3 indexed objects: 0=fp, 1=t1, 2=t2
     * 5 indexed properties:  1=x, 2=y, 3=diam, 4=clut, 5=shape
     */
    dx_set_by_indexIV(DXF_NODRAW, 0, -1, NULL, 
        1, PL_L2PVS(tgm, 0, 0), PL_L2PVS(tgm, 0, 1), PL_L2PVS(tgm, 0, 2),
        2, PL_L2PVS(tgm, 1, 0), PL_L2PVS(tgm, 1, 1), PL_L2PVS(tgm, 1, 2),
        3, PL_L2PVS(tgm, 2, 0), PL_L2PVS(tgm, 2, 1), PL_L2PVS(tgm, 2, 2),
        4, PL_L2PVS(tgm, 3, 0), PL_L2PVS(tgm, 3, 1), PL_L2PVS(tgm, 3, 2),
        5, PL_L2PVS(tgm, 4, 0), PL_L2PVS(tgm, 4, 1), PL_L2PVS(tgm, 4, 2),
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
			DX_X,		2, 	I_TRG2XCD,
			DX_Y,		2, 	I_TRG2YCD,
			DX_CLUT,	2, 	I_TRG2LCD,
			NULL);
}
