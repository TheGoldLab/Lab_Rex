/*
** task_estim.c
**
** estim task, modified from vgs task
**	Long Ding 12-07-2007
**	
*/

#include "../toys.h"
#include "../task.h"
#include "../task_utilities.h"

/* PRIVATE MACROS/CONSTANTS */
#define NUM_GR_MENUS 2
#define TTMV(ta,n) 	pl_list_get_v((ta)->task_menus->lists[0],(n))
#define TPMV(ta,n) 	pl_list_get_v((ta)->task_menus->lists[1],(n))
#define TGMV(ta,n) 	pl_list_get_v((ta)->task_menus->lists[2],(n))

/* PRIVATE ROUTINES */
void 		estim_make_trials	(_PRtask);
_PRtrial estim_get_trial	(_PRtask, int);
void 		estim_set_trial	(_PRtrial);

/* 
** TASK MGS
**		("estim")
*/

struct _TAtask_struct gl_estim_task[] = {

	/* name */
	"estim",

	/* 
	** Menus 
	*/
	{
	/* 
	** General Timing Parameters (likely to change for each monkey)
	*/
	{ "estim-Timing",		1 },
   		{ "pretgt",      	0, 1.0 },
   		{ "wait4sacon",      	0, 1.0 },
   		{ "wait4hit",   	0, 1.0 },
   		{ "delay2rew",   	0, 1.0 },
		{NULL},
	/* 
	** Specific Task parameters (likely to change for each exp)
	*/
	{ "estim-Property",		1 },
		{ "Trial_blocks",		0, 1.0 },	/* Trial_blocks: # of repetition of a full set of trials */
	   	{ "Angle_o",      	0, 1.0 },
   		{ "Angle_step",     	0, 1.0 },
	   	{ "Angle_n",      	0, 1.0 },
		{ "ecc",			0, 1.0},
		{ "targetwin",		0, 1.0 },
		{ "asymRew",		0, 1.0 },
   		{ "reward",      	0, 1.0 },
   		{ "flagRepeat",     	0, 1.0 },
		{ "Drop_codes",		1,	1.0 },
		{NULL},
	/*
	**	2 'standard' graphics objects:
	**		0 - fixation
	**		1 - target
	*/
	{ "graphics",		NUM_GR_MENUS },
		TU_XYRT_LIST,
	   	{ DX_DIAM,			1, 0.1 },
   		{ DX_CLUT,			0, 1.0 },
	   	{ NULL },

		/* END OF MENUS */
		{ NULL }
	},

	/* 
	** Graphics objects, by name
	*/
	{  { "dXtarget",	2 }, 
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
	&estim_make_trials,
	&estim_get_trial,
	&estim_set_trial,
};


/* PUBLIC ROUTINE: estim_make_trials
**
*/
void estim_make_trials(_PRtask task)
{
	int ang_n, *angs, rewcont_n, *rewcont;
	 
	/* 
	** Make array of angles and reward contingencies
	**	rewcont = 0: big reward angle_o, small reward angle_o+180
	**	rewcont = 1: big reward angle_o+180, small reward angle_o
	**	rewcont = 2: equal reward (avg of big and small reward)
	*/
	if (TPMV(task, "asymRew"))	{
		rewcont_n = 2;
		rewcont = SAFE_ZALLOC(long,2);
		rewcont[0] = 0;
		rewcont[1] = 1;
		ang_n = 2;
		angs  = toy_seriesi(ang_n, TPMV(task, "Angle_o"), 180);
	} else {
		rewcont_n = 1;
		rewcont = SAFE_ZALLOC(long, rewcont_n);
		rewcont[0] = 2;
		ang_n = toy_boundi(TPMV(task, "Angle_n"), 1, 1000);
		angs  = toy_seriesi(ang_n, TPMV(task, "Angle_o"), TPMV(task, "Angle_step"));
	}
	
		/* 
		** Make trial set for angle-reward contingency pairs
		*/
		tu_make_trials2D(task, "rewcont", rewcont_n, rewcont, 1.0, 	"angle", ang_n, angs, 1.0);

	/* 
	** Free stuff
	*/
	SAFE_FREE(rewcont);
	SAFE_FREE(angs);
}

/* PUBLIC ROUTINE: estim_get_trial
**
** Returns:
**		_PRtrial if trials remain
**		NULL otherwise
*/
_PRtrial estim_get_trial(_PRtask task, int reset_flag)	
{
	int trial_blocks = TPMV(task, "Trial_blocks");
	int asymRew = TPMV(task, "asymRew");
	if (asymRew)	{
		return(tu_get_random_altRows(task, trial_blocks, reset_flag));
	} else {
		return(tu_get_random(task, trial_blocks, reset_flag));
	}

}
/* PUBLIC ROUTINE: estim_set_trial
**
**	Note that here we mostly access groups/lists/properties
**	 by INDEX, not NAME, so be careful if anything
**	 above changes.
*/
void estim_set_trial(_PRtrial trial)
{
	_PLlist tgm = trial->task->task_menus->lists[2];

	/* Make working copies of values in 
	**		graphics menu 
	*/
	pl_list_set_v2w(tgm);

	/* Conditionally fill-in angle ("r")
	** 	for graphics object 1 (target)
	*/
	/* LD note: in the menu, T must be NULLI? */
printf("estim trial rewcont = %d angle = %d Ecc = %d \n", PL_L2PV(trial->list, 0), PL_L2PV(trial->list, 1), TPMV(trial->task, "ecc")  );
		PL_CSET( PL_L2PWS(tgm, 5, 1), PL_L2PV(trial->list, 1) );
		PL_CSET( PL_L2PWS(tgm, 4, 1), TPMV(trial->task, "ecc") );	
	
	/* compute x, y positions of
	**		fixation point (obj 0)
	**		target (obj 1)
 	*/
	/* LD note: this function only uses the working copy */
	tu_compute_xyrt(tgm);

	/* Set x, y positions in 
	**		actual dXtarget objects
	*/
	/* LD note: 0-(TU_XYRT_LEN-1) are from the TU_XYRT_LIST
	**	TU_XYRT_LEN refers to the DIAM property in the graphics menu
	**	TU_XYRT_LEN+1 refers to the CLUT property in the graphics menu
	** dx_set_by_indexIV usage
	** 	Arguments: flag, 
	**		graphics class index, e.g., in the current task, only one dXtarget class, index=0;
	**		# of objects in the group. If <0, the routine automatically figures it out
	**		NULL: to separate the previous arguments from the following list of values
	**		the next several rows of inputs are in the general format: 
	**			property index, value[0], value[1], ... 
	**			e.g., the inputs below are using the property fields set for dXtarget struct 
	**				1: DX_X, 2: DX_Y ... etc
	**		
	*/		

/* printf("actual X = %d, Y = %d", PL_L2PWS(tgm, 0, 1),PL_L2PWS(tgm, 1, 1));	*/
	dx_set_by_indexIV(DXF_NODRAW, 0, -1, NULL, 
		1, PL_L2PWS(tgm, 0, 0), 		PL_L2PWS(tgm, 0, 1),
		2, PL_L2PWS(tgm, 1, 0), 		PL_L2PWS(tgm, 1, 1),
		3, PL_L2PWS(tgm, TU_XYRT_LEN, 0), 	PL_L2PWS(tgm, TU_XYRT_LEN, 1),
		4, PL_L2PWS(tgm, TU_XYRT_LEN+1, 0), PL_L2PWS(tgm, TU_XYRT_LEN+1, 1),
		ENDI);

	/* Conditionally drop codes */
	if(TPMV(trial->task, "Drop_codes") != 0)

		ec_send_code_tagged(I_REWCONTCD, 7000 + PL_L2PV(trial->list, 0));	/* send tagged ecode with reward contingency */

		pl_ecodes_by_nameV(dx_get_object(DX_TARGET), 7000,
			DX_X,		0, 	I_FIXXCD,
			DX_Y,		0, 	I_FIXYCD,
			DX_CLUT,	0, 	I_FIXLCD,
			DX_X,		1, 	I_TRG1XCD,
			DX_Y,		1, 	I_TRG1YCD,
			DX_CLUT,	1, 	I_TRG1LCD,
			NULL);

}

	
