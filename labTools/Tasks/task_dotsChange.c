/* 
** task_dotsChange.c
**
** 2013/11/21 yl: inherited from task_dotsy for change-detectin task
**
** basic task: monkey must detect a change in a feature of the
**             dot stimulus (e.g., coherence)
**             once detected, must saccade to the relevant target for that trial
**             
*/

/*
** task_dotsy.c
**
**	 2afc task for yl's mt/lip stim experiments
**  
**  basic task: association rule for direction/saccade governed by 
**     MT dir + offset/LIP RF
**
**  MT stim block: Stim_flag = 1;
**    random trials w/ following properties:
**      1. signed coh <= Max_coh2stim  && smode == 1
**      2. signed coh <= Max_coh2stim  && smode == 0
**      3. signed coh >  Max_coh2stim  && smode == 0     
**
**  LIP stim block: Stim_flag = 2;
**    block of randomized Stim_block_len stim trial
**    followed by a single random non-stim trial
**    repeated until all trials are fulfilled 
**    e.g., if Stim_block_len = 3
**    pointer array: SSSNSSSNSSSN
**      where S = stim, N = nonstim
**
**    ie. there will be num_cohs*Stim_block_len*num_blocks stim trials
**                      num_cohs*               num_blocks nonstim trials
**	  If Stim_block_len == 0 or NULLI, only stim trials
**	  will be used.
**
**
**  2011/03/17 yl from task_dotsj.c
**  2011/05/11 yl fixed ecode issue w/ t1_x
**  2011/05/19 yl LIP stim now supports all-stim blocks
**  2011/05/24 yl implements stim_flag == 0 => no-stim, randomized
**				    LIP stim trials: just increments pointer after every trial
**  2012/02/05 yl adds "Rew_prob" item to menu. 
*/

#include "../toys.h"
#include "../task.h"
#include "../task_utilities.h"

/* ===== PRIVATE MACROS/CONSTANTS ===== */
#define NUM_GR_MENUS 6
#define TI 				"setup"
#define TIMV(ta,n)	pl_list_get_v((ta)->task_menus->lists[0], (n))
#define GETMVI(ta,i,k) PL_L2PV((ta)->task_menus->lists[i],k)


/* PRIVATE DATA STRUCTURES */

/* ===== PRIVATE ROUTINES ===== */
/* standard for every task file */
void     dotsChange_make_trials  (_PRtask);
_PRtrial dotsChange_get_trial    (_PRtask, int);
void     dotsChange_set_trial    (_PRtrial);

/* PRIVATE VARIABLES */
enum MENU_INDEX {
	TI_MENU,
	GRAPHICS_MENU,
	DOTS_MENU
};

/* 
** TASK: DOTS
**		1 dXdots object
**		3 dXtarget objects (fp, 2 targets)
*/
struct _TAtask_struct gl_dotsChange_task[] = {

	/* name */
	"dotsChange",

	/* menus */
	{
		/* 
		** Task info: 'setup' menu
		*/
		{ TI, 				1 },
		{ "Trial_order",	0, 		1.0 },	/* 0 = in order, 1 = randomize*/
		{ "Trial_blocks",	1,		1.0 },	/* # trials per cond */
		{ "FP_CLUT0",	5,	1.0}, 	/* initial FP */
		{ "FP_CLUT1", 1,	1.0},	/* cued FP */
		{ "ProT_r",			0,		1.0 }, /* relevant target ... dist from fp */
		{ "ProT_t",			0,		1.0 },	/* relevant target ... angle from left */
		{ "Show_one(0)/both(1)",		0,		1.0 }, /* if true, the irrelevant target is also shown */
		{ "Pro(0)/anti(1)/both(2)",	0,	1.0 }, /* whether relevant target is specified by T1_r/t, 180deg opposite, or both */
		{ "Coh(0)/sp(1)/dir(2)",	0, 1.0}, /* specifies the feature that changes */
		{ "Pre(0)/post(1)",		0,		0.1 }, /* whether value change is before or after the changepoint */
		{ "Change_val0",		1000,		0.1 }, /* post-change value */
		{ "Change_val1",		NULLI,	0.1 }, /* post-change value */
		{ "Change_val2",		NULLI,	0.1 }, /* post-change value */
		{ "Change_val3",		NULLI,	0.1 },	/* post-change value */
		{ "Dots_x",		0,		0.1 },	/* dots position ... x */
		{ "Dots_y",		0,		0.1 },	/* dots position ... y */
		{ "Dots_diam",		100,	0.1 },	/* dots diameter */
		{ "Drop_codes",	1,		1.0 },
		{ NULL },

		/*
		**	Six 'standard' graphics menu
		**		0 - fixation
		**		1 - dots aperture
		**		2 - target centroid
		**		3 - target 1
		**		4 - target 2
		**		5 - dots direction
		*/
		{ "graphics",	 NUM_GR_MENUS },
			TU_XYRT_LIST,
			{ DX_DIAM,      11, 0.1 },
			{ DX_CLUT,   	12, 1.0 },
		{ NULL },

		/*
		** One dots object menu
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
	&dotsChange_make_trials,
	&dotsChange_get_trial,
	&dotsChange_set_trial,
};

/* PUBLIC ROUTINE: dotsChange_make_trials
**
*/
void dotsChange_make_trials(_PRtask task)
{
	/* trial type: specifies what the relevant target is
	** (T1), (change_val) **
	*/
	/* for now, always make it pro */
	int *T1 = NULL, T1_n = 1;			/* array of T1s */
	int proAntiBoth;						/* from the menu */
	int val0, val1, val2, val3;			/* change vals from menu */
	int *change_vals = NULL, vals_n;	/* array of change values */
	int i;									/* counter */
	
	printf("dotsChange_make_trials start\n");
	
	switch(TIMV(task, "Pro(0)/anti(1)/both(2)"))
	{
		case 0:
			T1_n = 1; T1 = SAFE_ZALLOC(long,T1_n);
			T1[0] = 1; break;
		case 1:
			T1_n = 1; T1 = SAFE_ZALLOC(long,T1_n);
			T1[0] = -1; break;
		case 2:
			T1_n = 2; T1 = SAFE_ZALLOC(long,T1_n);
			T1[0] = 1; T1[1] = -1; break;
	}
	
	/* change vals */
	val0 = TIMV(task, "Change_val0");
	val1 = TIMV(task, "Change_val1");
	val2 = TIMV(task, "Change_val2");
	val3 = TIMV(task, "Change_val3");
	
	printf("val0 %d\n", val0);
	
	if(val3 != NULLI) vals_n=4;
	else if(val2 != NULLI) vals_n=3;
	else if(val1 != NULLI) vals_n=2;
	else if(val0 != NULLI) vals_n=1;
	else vals_n = 1;
	
	printf("vals_n = %d\n", vals_n);
	
	/* allocate change_vals */
	change_vals = SAFE_ZALLOC(long, vals_n);
	
	for(i = 0; i < vals_n; i++) {
		change_vals[i] = i;
	}
/*	printf("coh_n %d\n", coh_n); */

	/*
	** utility to make trial set for T1/vals pairs
	*/
	tu_make_trials2D(task,
			"T1",	T1_n, T1, 1.0,
			"vals", vals_n,   change_vals,   0.1);

	/* 
	** Free stuff
	*/
	SAFE_FREE(T1); SAFE_FREE(change_vals);
	
	
	printf("dotsChange_make_trials end\n");
}

/* PUBLIC ROUTINE: dotsChange_get_trial
**
** Returns:
**		_PRtrial if trials remain
**		NULL otherwise
*/
_PRtrial dotsChange_get_trial(_PRtask task, int reset_flag)
{	/* standard get_trial routine */
	int trial_blocks = TIMV(task, "Trial_blocks");


	printf("dotsChange_get_trial start\n");
	
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
	
	
	printf("dotsChange_get_trial end\n");
}

/* PUBLIC ROUTINE: dotsChange_set_trial
**
**	Note that here we mostly access groups/lists/properties
**	 by INDEX, not NAME, so be careful if anything
**	 above changes.
*/
void dotsChange_set_trial(_PRtrial trial)
{
	_PLlist 	tim = trial->task->task_menus->lists[0],
				tgm = trial->task->task_menus->lists[1],
				tdm = trial->task->task_menus->lists[2];
	/* tim = setup menu
	** tgm = graphics menu
	** tdm = dots men
	*/

	int 		
			    /* the following are arrays of indices 
				**		of target objects (fp, t1, t2) in
				**		the task graphics menu and the
				**		dXtargets objects, respectively	
				*/
				tti[] = {0, 3, 4},	
				gti[] = {0, 1, 2},
				gdi[] = {0};

	/* current trial's parameters */
	int T1, change_valk;		/* trial parameters... from trial pointer */
	int change_val;			/* actual change value */
	int pre_post;				/* whether change_val refers to value before or after */
	int t1ang, t2ang;			/* the two targets */
	int dots_x, dots_y, dots_diam;
	
	
	printf("dotsChange_set_trial start\n");
	
	/* default here is to show single target that is same as T1_r/t */
	
	/* get the current trial's T1 */
	T1 = PL_L2PV(trial->list,0);  /* the first element */
	
	printf("T1 = %d\n",T1);
	
	/* get the current trial's change_val */
	change_valk = PL_L2PV(trial->list,1);
	switch(change_valk){
		case 0:
			change_val = TIMV(trial->task, "Change_val0");
			break;
		case 1:
			change_val = TIMV(trial->task, "Change_val1");
			break;
		case 2:
			change_val = TIMV(trial->task, "Change_val2");
			break;
		case 3:
			change_val = TIMV(trial->task, "Change_val3");
			break;
	}

	/*	printf("scoh %d, dots_ang %d \n", scoh, dots_ang); */
	
	/* make working copies of the task object menu values */
	pl_list_set_v2w(tgm);
	pl_list_set_v2w(tdm);
	
	/*
	** conditionally set graphics/dots menus
	*/
	
	/* things independent of T1 *****/
	/* graphics menu */
	PL_CSET( PL_L2PWS(tgm, kX, 1),	TIMV(trial->task, "Dots_x") );		/* dots:	1, 0. */
	PL_CSET( PL_L2PWS(tgm, kY, 1),	TIMV(trial->task, "Dots_y") );		/* dots:	1, 1.  */
	PL_CSET( PL_L2PWS(tgm, kT, 2),	TIMV(trial->task, "ProT_t") );		/* ctr: 2, 5. */
	PL_CSET( PL_L2PWS(tgm, kR, 3),	TIMV(trial->task, "ProT_r") );		/* trg1:	3, 4. */
	PL_CSET( PL_L2PWS(tgm, kR, 4),	TIMV(trial->task, "ProT_r") );		/* trg2:	4, 4. */
	
	/* things that depend on T1 ***/
	if(T1==1) {
		t1ang = 0; t2ang = 180;
	} else {
		t1ang = 180; t2ang = 0;
	}
		
	/* graphics menu again */
	PL_CSET( PL_L2PWS(tgm, kT, 3),	t1ang);		/* trg1: 3, 5. t */
	PL_CSET( PL_L2PWS(tgm, kT, 4),	t2ang);		/* trg2: 4, 5. t */
	
	/* debugging stuff */
/*	printf("dots_ang %d, t1ang %d, t2ang %d\n", dots_ang, t1ang, t2ang); */


	/* debugging stuff added by YL */
/*	printf("before compute: WS tgm,0,1 %d\n", PL_L2PWS(tgm,0,1));
**	printf("before compute: VS tgm,0,1 %d\n", PL_L2PVS(tgm,0,1));
*/	


	/* compute xys */
	tu_compute_xyrt(tgm);


/*	printf(" after compute: WS tgm,0,1 %d\n", PL_L2PWS(tgm,0,1));
**	printf(" after compute: VS tgm,0,1 %d\n", PL_L2PVS(tgm,0,1));
*/

	/* Set x, y positions in 
	**		actual dXtarget objects
	**	Note: the indices here are based on the graphics objects
	**		fixation point	(obj 0)
	**		target 1		(obj 1)
	**		target 2		(obj 2)
	*/
	dx_set_by_indexIV(DXF_NODRAW, 0, 3, gti, 
		1, PL_L2PWS(tgm, 0, 0), 		PL_L2PWS(tgm, 0, 3), 		PL_L2PWS(tgm, 0, 4), 
		2, PL_L2PWS(tgm, 1, 0), 		PL_L2PWS(tgm, 1, 3), 		PL_L2PWS(tgm, 1, 4), 
		3, PL_L2PWS(tgm, TU_XYRT_LEN, 0), 	PL_L2PWS(tgm, TU_XYRT_LEN, 3), PL_L2PWS(tgm, TU_XYRT_LEN, 4),
		4, PL_L2PWS(tgm, TU_XYRT_LEN+1, 0), PL_L2PWS(tgm, TU_XYRT_LEN+1, 3), PL_L2PWS(tgm, TU_XYRT_LEN+1, 4),
		ENDI);


	
	/* Second, set properties of the dXdots object */
	dx_set_by_nameIV(DXF_D1, DX_DOTS, 1, gdi,
			DX_X,			PL_L2PWS(tgm, 0, 1),
			DX_Y,			PL_L2PWS(tgm, 1, 1),
			DX_DIAM,		PL_L2PWS(tgm, TU_XYRT_LEN, 1),
			DX_CLUT,		PL_L2PWS(tgm, TU_XYRT_LEN+1, 1),
			DX_COH, 		PL_L2PW(tdm, 2),
			DX_DIR, 		PL_L2PW(tdm, 3),
 			DX_SPEED,		PL_L2PW(tdm, 1),
 			DX_SEED,		tu_get_seed(PL_L2PW(tdm, 0), PL_L2PW(tdm,2), PL_L2PW(tdm,3)) ,	
			NULL);



	/* Drop codes */
	if(TIMV(trial->task, "Drop_codes") != 0){

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
			DX_X,		0,	I_STXCD,
			DX_Y,		0,	I_STYCD,
			DX_DIAM,	0,	I_STDIACD,
			DX_COH,		0, 	I_COHCD,
			DX_DIR,		0, 	I_DOTDIRCD,
			DX_SPEED,	0,	I_SPDCD,
			DX_SEED,	0, 	I_DTVARCD,
			NULL);
	}
	
	
	printf("dotsChange_set_trial end\n");

}