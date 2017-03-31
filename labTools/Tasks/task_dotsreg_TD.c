/*
** task_dotsreg_LD.c
**
** Regular dots task,
**	Long Ding, modified from task_dotsrt_LD20070221.c 3-13-2007
**	
*/

/*
 task_dotreg_TD.c
 Created by Taka Doi from task_dotreg_LD.c on 2013-05-30
 for perceptual learning and speed-accuracy tradeoff in FEF-CD

 Greatly modified how to sample trials across different coh levels.
 See task_dotRT_TD.c for details.

 2014-04-29. TD. Reduced 3.2% from coherence set. 
 
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
void 		dotsregTD_make_trials	(_PRtask);
_PRtrial dotsregTD_get_trial	(_PRtask, int);
void 		dotsregTD_set_trial	(_PRtrial);

/* 
** TASK DOTSREG
**		("dotsregTD")
*/

struct _TAtask_struct gl_dotsregTD_task[] = {

	/* name */
	"dotsregTD",

	/* 
	** Menus 
	*/
	{
	/* 
	** General Timing Parameters (likely to change for each monkey)
	*/
	{ "dotsregTD-Timing",		1 },
			{ "Vexp_time", 1000, 1.0 },
			{ "Vmin_time", 200, 1.0 },
			{ "Vmax_time", 1500, 1.0 },
			{ "Vmean_time", 600, 1.0 },
			{ "Voverride", 1200, 1.0 },
			{ "Voverride_random", 200, 1.0 },
			{ "Dexp_time", 1000, 1.0 },
			{ "Dmin_time", 200, 1.0 },
			{ "Dmax_time", 1500, 1.0 },
			{ "Dmean_time", 600, 1.0 },
			{ "Doverride", 1200, 1.0 },
			{ "Doverride_random", 200, 1.0 },
   		{ "wait4sacon",      	0, 1.0 },
   		{ "wait4hit",   	0, 1.0 },
   		{ "delay2rew",   	0, 1.0 },
   		{ "errorTimeOut",   	0, 1.0 },
   		{ "brfixTimeOut",   	0, 1.0 },
   		{ "ncerrTimeOut",   	0, 1.0 },
		{NULL},
	/* 
	** Specific Task parameters (likely to change for each exp)
	*/
	{ "dotsregTD-Property",		1 },
			{ "Trial_order",		0, 1.0 },
             { "Angle_o",      	0, 1.0 },
			{ "tgtecc",				0, 1.0},
			{ "targetwin",			0, 1.0 },
            { "Reward",      	0, 1.0 },
			{ "flagRepeat",     	0, 1.0 },
			{ "Drop_codes",		1,	1.0 },
            { "Coh_W_High",			1,		1.0}, /* the below is added by TD on 2013-06-06 */
            { "Coh_W_Uni",			1,		1.0},
            { "Coh_Alpha",			1,		1.0},
            { "Coh_Beta",             1,		1.0},
            { "nTrialpBlock",			1,		1.0},
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
	&dotsregTD_make_trials,
	&dotsregTD_get_trial,
	&dotsregTD_set_trial,
};


/* PUBLIC ROUTINE: dotsregTD_make_trials
**
*/
void dotsregTD_make_trials(_PRtask task)
{   // 1 is later changed to 0. Use 1 to show % correct correctly in console window. TD
  	static int coh_list[] = {1, 64, 128, 256, 512, 999};
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
    	// cohs_angle = {0,0,64,-64,128,-128,....,999,-999}
    cohs_angle_n = num_coh*2;
	cohs_angle   = SAFE_ZALLOC(long, cohs_angle_n);
	for (i=0; i<num_coh; i++) {
		cohs_angle[2*i]   = +coh_list[i];
		cohs_angle[2*i+1] = -coh_list[i];
	}
	
	/*
     ** Make array of angles and reward contingencies
     **	rewcont = 0: big reward angle_o, small reward angle_o+180
     **	rewcont = 1: big reward angle_o+180, small reward angle_o
     **	rewcont = 2: equal reward
     */
    /* TD: removed switch and only uses rewcont[0] = 2.
     */
	rewcont_n  = 1;
	rewcont    = SAFE_ZALLOC(long,1);
	rewcont[0] = 2;
		
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

/* PUBLIC ROUTINE: dotsregTD_get_trial
**
** Returns:
**		_PRtrial if trials remain
**		NULL otherwise
*/
_PRtrial dotsregTD_get_trial(_PRtask task, int reset_flag)	
{
    /*
     ** All conditional operations based on num_by_coh and Trial_blocks are removed... (TD 2013-06-07)
     ** Instead of using Trial_blocks, now directly specify the number of trials per block. Then, the number
     ** of trials for each coherence is calcualted based on a probability density distribution.
     */
    return(td_get_random_altRows(task, reset_flag));
}



/* PUBLIC ROUTINE: dotsregTD_set_trial
**
**	Note that here we mostly access groups/lists/properties
**	 by INDEX, not NAME, so be careful if anything
**	 above changes.
*/
void dotsregTD_set_trial(_PRtrial trial)
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
    if (cohv<0)
		ang = angle+180;
	cohv = abs(cohv);
    /* change coh 1 to 0 */
    if (cohv==1)
        cohv = 0;
    
    
    printf("dotReg trial rewcont = %d angle = %d coh = %d Ecc = %d\n",
	
    PL_L2PV(trial->list, 0), ang, cohv, TPMV(trial->task, "tgtecc")  );

	PL_CSET( PL_L2PWS(tgm, 5, 3), ang);
	PL_CSET( PL_L2PWS(tgm, 4, 3), TPMV(trial->task, "tgtecc") );
	PL_CSET( PL_L2PWS(tgm, 5, 4), ang+180);
    PL_CSET( PL_L2PWS(tgm, 4, 4), TPMV(trial->task, "tgtecc") );
	

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
		3, PL_L2PWS(tgm, TU_XYRT_LEN,   0), PL_L2PWS(tgm, TU_XYRT_LEN,   3), PL_L2PWS(tgm, TU_XYRT_LEN,   4),
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
        // Probably, this line can be commented out with no harm. but just keep it here.
        // TD 2013-06-07
        
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

	
