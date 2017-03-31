/*
** task_dots_rev.c
**   Created from task_dots_adpt2.c by JIG on 2015-10-30
**   Created from task_dots_adpt.c by TD on 2014-02-17
**
**  Uses a "reversing dots" stimulus that has two controllable
**      epochs within each trial. Each epoch is defined by a hazard
**      rate, duration, two coherences, and two directions
**
*/

#include "../toys.h"
#include "../task.h"
#include "../task_utilities.h"

/* PRIVATE MACROS/CONSTANTS */
#define NUM_GR_MENUS 6
#define TI 				"setup"
#define TDM_COH 2
#define TDM_DIR 3 

/* PRIVATE DATA STRUCTURES */

/* PRIVATE ROUTINES */
void		dots_rev_make_trials	(_PRtask);
_PRtrial	dots_rev_get_trial	(_PRtask, int);
void		dots_rev_set_trial	(_PRtrial);


/* PRIVATE VARIABLES */

/* 
** TASK: DOTS
**		1 dXdots object
**		3 dXtarget objects (fp, 2 targets)
*/
struct _TAtask_struct gl_dots_rev_task[] = {

	/* name */
	"dots_rev",

	/* menus */
	{
		/* 
		** Task info
		*/
		{ TI, 						1 },
        { "Trial_order",		1, 1.0 },
        { "Trial_blocks",		1, 1.0 },
        { "Epoch1_Dir1",   	0, 1.0 },
        { "Epoch1_Dir2",   	0, 1.0 },
        { "Epoch1_Coh1",   	0, 0.1 },
        { "Epoch1_Coh2",   	0, 0.1 },
        { "Epoch1_DurMin", 	0, 1.0 },
        { "Epoch1_DurMean",	0, 1.0 },
        { "Epoch1_DurMax",		0, 1.0 },
        { "Epoch1_Rate",     	0, 1.0 },
        { "Epoch1_RateFlag",	0, 1.0 }, 	// 0=Hazard; 1=Fixed
        { "Epoch2_Dir1",   	0, 1.0 },
        { "Epoch2_Dir2",   	0, 1.0 },
        { "Epoch2_Coh1",   	0, 0.1 },
        { "Epoch2_Coh2",   	0, 0.1 },
        { "Epoch2_DurMin", 	0, 1.0 },
        { "Epoch2_DurMean",	0, 1.0 },
        { "Epoch2_DurMax", 	0, 1.0 },
        { "Epoch2_Rate",   	0, 1.0 },
        { "Epoch2_RateFlag",	0, 1.0 }, 	// 0=Hazard; 1=Fixed
        { "Min_Switch_Dur",   10, 1.0},	// minimum time between any switch
        { "Max_Switch_Dur",   5000, 1.0},	// maximum time between any switch
        { "Drop_codes",    	1, 0.1 },
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
        { DX_DIAM,	11, 0.1 },
        { DX_CLUT,	12, 1.0 },
        { NULL },

		/*
		** One dots object
		*/
		{ "dots",		1 },
			{ DX_SEED,	0, 0.1 },
			{ DX_SPEED,	0, 0.1 },
			{ DX_COH,	0, 0.1 },
			{ DX_DIR,	0, 0.1 },
			{ NULL },

		/* END OF MENUS */
		{ NULL }
	},

	/* 
	** Graphics objects, by name
	*/
	{	{ "dXtarget",	3 },
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
	&dots_rev_make_trials,
	&dots_rev_get_trial,
	&dots_rev_set_trial,
};

/* PUBLIC ROUTINE: dots_rev_make_trials
**
**  Make 1D array of trials, just the two directons
**		from Epoch 2. Note that we don't know until the END of
**		the trial which was the final direction shown (which is what
**		the monkey should be using to make its choice), so we may
**		have to swap in the correct "current trial" at that point.
*/
void dots_rev_make_trials(_PRtask task)
{
	_PLlist 	tim = task->task_menus->lists[0]; // task info (setup) menu
	register int i;
	valtype  dirs[] = { 	pl_list_get_v(tim, "Epoch2_Dir1"),
								pl_list_get_v(tim, "Epoch2_Dir2")},
				cohs[] = { 	pl_list_get_v(tim, "Epoch2_Coh1"),
								pl_list_get_v(tim, "Epoch2_Coh2")};

	/* add the array of two (empty) trials, one per dir/coh 
	** 	pair from Epoch 2 */
	pr_task_add_trials1D(task, 2);

	/* For each, add a property list for dir/coh values from Epoch 2 */
	for(i=0;i<2;i++) {
		task->trials[i]->id = task->id*100+i;
		task->trials[i]->list = pl_list_initV("1dtc", 0, 1, 
			"dir", dirs[1], 0.1, 
			"coh", cohs[1], 0.1, 
			NULL);
	}
}

/* PUBLIC ROUTINE: dots_rev_get_trial
**
** Returns:
**		_PRtrial ... always choose the first one because
**              it doesn't really matter for now -- we don't know what type
**              we're in until the END of the trial (determined in state set)
*/
_PRtrial dots_rev_get_trial(_PRtask task, int reset_flag)
{
	/* possibly make trial pointers */
	if(task->trialPs == NULL)
		pr_task_make_trialP_all(task, 1);
	
	/* return the first one */
	return(task->trialPs[0]);
}

/* PUBLIC ROUTINE: dots_rev_set_trial
**
**	Note that here we mostly access groups/lists/properties
**	 by INDEX, not NAME, so be careful if anything
**	 above changes.
**
*/
void dots_rev_set_trial(_PRtrial trial)
{
	_PLlist 	tim = trial->task->task_menus->lists[0], // task info (setup) menu
				tgm = trial->task->task_menus->lists[1], // 6 task graphics menus (see above) 
				tdm = trial->task->task_menus->lists[2]; // task dots menu: 0, seed; 1, speed; 2, coherence; 3 direction
	int 		gti[] = {0, 1, 2},
 				gdi[] = {0};
    
	/* make working copies of the menus */
	pl_list_set_v2w(tim);
	pl_list_set_v2w(tgm);
	pl_list_set_v2w(tdm);

	/* compute xy positions of FP, targets, dots */
	tu_compute_xyrt(tgm);
    
	/* Set x (1), y (2), diam (3), clut (4) properties of three separate
	** dXtarget (graphics) objects with indices:
	**		fixation point	(obj 0)
	**		target 1		(obj 1)
	**		target 2		(obj 2)
	** Each line is:
	**     <PROPERTY INDEX>, <Obj 1 value>, <Obj 2 value>, <Obj 3 value>
	** PL_L2PWS(tgm,*,*) gets the value from the appropriate graphic menu;
	**     usage: PL_L2PWS(tgm, 1, 0) = Task Graphics Menu (tgm), 
	**             first property (DX_X), 0th list (fp)
	**     note that we could also use:
	**         pl_list_get_wi(tgm, DX_X, 0)
	*/
	dx_set_by_indexIV(DXF_NODRAW, // draw flag
		0,                      	// class index (dx_target, as defined above) 
		3,                      	// number of properties to set
		gti,                    	// array of object indices
		1, PL_L2PWS(tgm, 0, 0),					PL_L2PWS(tgm, 0, 3),			PL_L2PWS(tgm, 0, 4),
		2, PL_L2PWS(tgm, 1, 0),					PL_L2PWS(tgm, 1, 3),			PL_L2PWS(tgm, 1, 4),
		3, PL_L2PWS(tgm, TU_XYRT_LEN, 0),   PL_L2PWS(tgm, TU_XYRT_LEN,   3),PL_L2PWS(tgm, TU_XYRT_LEN,   4),
		4, PL_L2PWS(tgm, TU_XYRT_LEN+1, 0), PL_L2PWS(tgm, TU_XYRT_LEN+1, 3),PL_L2PWS(tgm, TU_XYRT_LEN+1, 4),
		ENDI);

	/* Set properties of the dXdots object */
	dx_set_by_nameIV(DXF_NODRAW, DX_DOTS, 1, gdi,
		DX_X,			PL_L2PWS(tgm, 0, 1),
		DX_Y,			PL_L2PWS(tgm, 1, 1),
		DX_DIAM,		PL_L2PWS(tgm, TU_XYRT_LEN, 1),
		DX_CLUT,		PL_L2PWS(tgm, TU_XYRT_LEN+1, 1),
		DX_COH, 		PL_L2PW (tdm, 2),
		DX_DIR, 		PL_L2PW (tdm, 3),
		DX_SPEED,	PL_L2PW (tdm, 1),
		DX_SEED,		tu_get_seed(PL_L2PW(tdm, 0), 
							pl_list_get_w(tim, "Epoch1_Coh1"), 
							pl_list_get_w(tim, "Epoch1_Dir1")),
		NULL);
    
	/* Conditionally send ecodes */
	if(pl_list_get_v(tim, "Drop_codes") != 0) {
        
		// Hazard rates
		ec_send_code_tagged(I_RATE1,     
			7000 + pl_list_get_v(tim, "Epoch1_Rate"));
		ec_send_code_tagged(I_RATEFLAG1, 
			7000 + pl_list_get_v(tim, "Epoch1_RateFlag"));
		ec_send_code_tagged(I_RATE2,     
			7000 + pl_list_get_v(tim, "Epoch2_Rate"));
		ec_send_code_tagged(I_RATEFLAG2, 
			7000 + pl_list_get_v(tim, "Epoch2_RateFlag"));
       
		// Target objects (fp, T1, T2)
		pl_ecodes_by_nameV(dx_get_object(DX_TARGET), 7000,
			DX_X,   	0, 	I_FIXXCD,
			DX_Y,   	0, 	I_FIXYCD,
			DX_CLUT,	0, 	I_FIXLCD,
			DX_X,   	1, 	I_TRG1XCD,
			DX_Y,   	1, 	I_TRG1YCD,
			DX_CLUT,	1, 	I_TRG1LCD,
			DX_X,   	2, 	I_TRG2XCD,
			DX_Y,   	2, 	I_TRG2YCD,
			DX_CLUT,	2, 	I_TRG2LCD,
			NULL);
        
		// Dots object
		pl_ecodes_by_nameV(dx_get_object(DX_DOTS), 7000,
			DX_X,		0,		I_STXCD,
			DX_Y,		0,		I_STYCD,
			DX_DIAM, 0,		I_STDIACD,
			DX_SPEED,0,		I_SPDCD,
			DX_SEED,	0,		I_DTVARCD,
			NULL); 

	} /* dropped ecodes */
}
