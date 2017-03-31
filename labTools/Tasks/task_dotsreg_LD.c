/*
** task_dotsreg_LD.c
**
** Regular dots task,
**	Long Ding, modified from task_dotsrt_LD20070221.c 3-13-2007
**	
*/

#include "../toys.h"
#include "../task.h"
#include "../task_utilities.h"

/* PRIVATE MACROS/CONSTANTS */
#define NUM_GR_MENUS 6
#define TTMV(ta,n) 	pl_list_get_v((ta)->task_menus->lists[0],(n))	/* timing */
#define TPMV(ta,n) 	pl_list_get_v((ta)->task_menus->lists[1],(n))	/* property */
#define TGMV(ta,n) 	pl_list_get_v((ta)->task_menus->lists[2],(n))	/* graphics */

/* PRIVATE ROUTINES */
void 		dotsreg_make_trials	(_PRtask);
_PRtrial dotsreg_get_trial	(_PRtask, int);
void 		dotsreg_set_trial	(_PRtrial);

/* 
** TASK DOTSREG
**		("dotsreg")
*/

struct _TAtask_struct gl_dotsreg_task[] = {

	/* name */
	"dotsreg",

	/* 
	** Menus 
	*/
	{
	/* 
	** General Timing Parameters (likely to change for each monkey)
	*/
	{ "dotsreg-Timing",		1 },
   		{ "preTgts",      	0, 1.0 },
			{ "exp_time", 1000, 1.0 },
			{ "min_time", 200, 1.0 },
			{ "max_time", 1500, 1.0 },
			{ "mean_time", 600, 1.0 },
			{ "override", 1200, 1.0 },
			{ "override_random", 200, 1.0 },
   		{ "wait4sacon",      	0, 1.0 },
   		{ "wait4hit",   	0, 1.0 },
   		{ "delay2rew",   	0, 1.0 },
   		{ "errorFB",   	0, 1.0 },
         { "inter_block",      1000, 1.0 },
		{NULL},
	/* 
	** Specific Task parameters (likely to change for each exp)
	*/
	{ "dotsreg-Property",		1 },
			{ "Trial_order",		0, 1.0 },
			{ "Trial_blocks",		0, 1.0 },	/* Trial_blocks: # of repetition of a full set of trials */
	   	{ "Angle_o",      	0, 1.0 },
	   	{ "Coherence_lo", 	0, 1.0 },
   		{ "Coherence_hi", 	0, 1.0 },
			{ "tgtecc",				0, 1.0},
			{ "targetwin",			0, 1.0 },
			{ "asymRew",			0, 1.0 },
   		{ "bigreward",      	0, 1.0 },
   		{ "smallreward",		0, 1.0 },	
			{ "reward_num",		1, 1.0 }, /* LD 9-17-07 values -3 and up, neg values refer to specific reward schedule, see paradigm_rec.c for details */
			{ "flagRepeat",     	0, 1.0 },
			{ "Drop_codes",		1,	1.0 },
			{NULL},
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
	   	{ DX_SEED,		 0, 1.0 },
   		{ DX_SPEED,		 0, 0.1 },
	   	{ DX_COH,    	 0, 0.1 },
   		{ DX_DIR,		 0, 1.0 },
	   	{ NULL },

		/* END OF MENUS */
		{ NULL }
	},

	/* 
	** Graphics objects, by name
	*/
	{  {DX_TARGET,	3 }, 
		{DX_DOTS,		1 }, 
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
	&dotsreg_make_trials,
	&dotsreg_get_trial,
	&dotsreg_set_trial,
};


/* PUBLIC ROUTINE: dotsreg_make_trials
**
*/
void dotsreg_make_trials(_PRtask task)
{
  	static int coh_list[] = {0, 32, 64, 128, 256, 512, 999};
// for more:  static int coh_list[] = {0, 8, 16, 32, 64, 128, 256, 512, 999};
	int  	num_coh = sizeof(coh_list)/sizeof(int),
					 *cohs=NULL, *rewcont, coh_n,
					 coh_lo, coh_hi, rewcont_n;
	int i,  k,  *cohs_angle=NULL, cohs_angle_n;
	 
	/* 
	** coding strategy: because the current _PRtask struct only have 
	** 2-D capability, the strategy here is to code angle0+coh as 
	** positive coh values, and -angle0+coh as negative coh values.
	** This combination will serve as the columns. The reward contingency 
	** will serve as the rows. So the current trials list will not carry
	** actual target angle information. This should be obtained by reading
	** the menu in set_trials routine. Also to make sure coh=0 can be 
	** associated with both directions, min coh is 1 here, but will be set 
	** to zero in set_trials routine.
	*/

	/* Figure out which coherences we'll use.
   	** Set using "Coherence_lo" and "Coherence_hi"
	**    in the task_menu struct. Note that
	**    positive values establish the range of
	**    coherence chosen from the static coh_list;
	**    a negative Coherence_lo means use just
	**    the two values (Coherence_lo and Coherence_hi).
	*/
	coh_lo = TPMV(task, "Coherence_lo");
	coh_hi = TPMV(task, "Coherence_hi");

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
   	register ilo = -1, ihi = -1;

      while(++ilo < num_coh-1 && coh_list[ilo] < coh_lo) ;
      while(++ihi < num_coh-1 && coh_list[ihi] < coh_hi) ;

		coh_n = ihi - ilo + 1;
		cohs  = SAFE_ZALLOC(long, coh_n);
		for(i=0;i<coh_n;i++,ilo++)
			cohs[i] = coh_list[ilo];
   }
	
	cohs_angle_n = coh_n*2;
	cohs_angle = SAFE_ZALLOC(long, cohs_angle_n);
	for (i=0; i<coh_n; i++) {
		cohs_angle[2*i] = cohs[i];
		cohs_angle[2*i+1] = -cohs[i];
	}
	
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
	} else {
		rewcont_n = 1;
		rewcont = SAFE_ZALLOC(long,1);
		rewcont[0] = 2;
	}
		
	/* 
	** Make trial set for angle+coherence/reward contingency combinations
	*/
	tu_make_trials2D(task, "rewcont", rewcont_n, rewcont, 1.0, 
					"cohs_angle", cohs_angle_n, cohs_angle, 1.0);

	/* 
	** Free stuff
	*/
	SAFE_FREE(rewcont);
	SAFE_FREE(cohs_angle);
}

/* PUBLIC ROUTINE: dotsreg_get_trial
**
** Returns:
**		_PRtrial if trials remain
**		NULL otherwise
*/
_PRtrial dotsreg_get_trial(_PRtask task, int reset_flag)	
{
	int trial_blocks = TPMV(task, "Trial_blocks");
	
		return(tu_get_random_altRows(task, trial_blocks, reset_flag));
/*
	Commented out by Long 3-15-2007. 
	if (TPMV(task, "asymRew"))	{
		return(tu_get_random_altRows(task, trial_blocks, reset_flag));
	} else {
		return(tu_get_random(task, trial_blocks, reset_flag));
	}
*/
}
/* PUBLIC ROUTINE: dotsreg_set_trial
**
**	Note that here we mostly access groups/lists/properties
**	 by INDEX, not NAME, so be careful if anything
**	 above changes.
*/
void dotsreg_set_trial(_PRtrial trial)
{
	/*
	** set properties for regular targets 3 (correct) & 4 (other)
	**
	*/
	_PLlist 	tgm = trial->task->task_menus->lists[2];
	_PLlist	tdm = trial->task->task_menus->lists[3];
	int angle = TPMV(trial->task, "Angle_o"), ang;
	int cohv;
	int dxtarget_indices[3] = {0, 1, 2};
	int dxdots_indices[1] = {0};
	int temp = 0;
	
	/* First, set properties for the dXtarget objects */

	/* Make working copies of values in 
	**		graphics menu 
	*/
	pl_list_set_v2w(tgm);

	/* Conditionally fill-in angle ("r")
	** 	for graphics object 3 (target)
	**			object 4 (other)
	*/
	cohv = PL_L2PV(trial->list, 1);
	ang = angle;
	if (cohv<0 || ( cohv == 0 && rand()%2) ) 
		ang = angle+180;
	cohv = abs(cohv);
	
printf("dotRT trial rewcont = %d angle = %d coh = %d Ecc = %d\n", 
	PL_L2PV(trial->list, 0), ang, cohv, TPMV(trial->task, "tgtecc")  );

	PL_CSET( PL_L2PWS(tgm, 5, 3), ang);
	PL_CSET( PL_L2PWS(tgm, 4, 3),  TPMV(trial->task, "tgtecc") );
	PL_CSET( PL_L2PWS(tgm, 5, 4), ang+180);
	PL_CSET( PL_L2PWS(tgm, 4, 4),  TPMV(trial->task, "tgtecc") );	

	/* compute x, y positions of
	**	Note: the indices are based on the graphics menu
	**		fixation point (obj 0)
	**		target (obj 3)
	**		other (obj 4)
 	*/
	tu_compute_xyrt(tgm);

	/* Set x, y positions in 
	**		actual dXtarget objects
	**	Note: the indices here are based on the graphics objects
	**		fixation point (obj 0)
	**		target (obj 1)
	**		other (obj 2)
	*/
	dx_set_by_indexIV(DXF_NODRAW, 0, 3, dxtarget_indices, 
		1, PL_L2PWS(tgm, 0, 0), 		PL_L2PWS(tgm, 0, 3), 		PL_L2PWS(tgm, 0, 4), 
		2, PL_L2PWS(tgm, 1, 0), 		PL_L2PWS(tgm, 1, 3), 		PL_L2PWS(tgm, 1, 4), 
		3, PL_L2PWS(tgm, TU_XYRT_LEN, 0), 	PL_L2PWS(tgm, TU_XYRT_LEN, 3), PL_L2PWS(tgm, TU_XYRT_LEN, 4),
		4, PL_L2PWS(tgm, TU_XYRT_LEN+1, 0), PL_L2PWS(tgm, TU_XYRT_LEN+1, 3), PL_L2PWS(tgm, TU_XYRT_LEN+1, 4),
		ENDI);

	/* Second, set properties of the dXdots object */

	dx_set_by_nameIV(DXF_D1, DX_DOTS, 1, dxdots_indices,
			DX_X,		0,
			DX_Y, 		0,
			DX_DIAM,		PL_L2PWS(tgm, TU_XYRT_LEN, 1),
			DX_CLUT,		PL_L2PWS(tgm, TU_XYRT_LEN+1, 1),
			DX_COH, 		cohv,
			DX_DIR, 		ang,
 			DX_SPEED,  	PL_L2PVS(tdm, 1 ,0),
 			DX_SEED,    tu_get_seed(PL_L2PVS(tdm, 0, 0), cohv, ang),	
			NULL);

	/* Conditionally drop codes */
	if(TPMV(trial->task, "Drop_codes") != 0) {

		ec_send_code_tagged(I_REWCONTCD, 7000 + PL_L2PV(trial->list, 0));	/* send tagged ecode with reward contingency */

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
			DX_COH,		0, 	I_COHCD,
			DX_DIR,		0, 	I_DOTDIRCD,
			DX_SEED,		0, 	I_DTVARCD,
			NULL);
	}
}

	
