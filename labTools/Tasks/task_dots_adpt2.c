/*
** task_dots_adpt2.c
   Created from task_dots_adpt.c by TD on 2014-02-17

   PREF/NULL switch and PREF/NEAR-PREF switch
*/

/*
 ** task_dots_adpt.c
 **
 **	Adaptation to motion-direction change
 ** The task is a passive fixation
 ** 
 ** Created by T.D. from task_dots.c on 2014-01-29
 **
 ** 2014/02/06 yl: create Angle_delta menu item.  Stim will be Angle_0 or Angle_0+Angle_delta
 */

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
#define TDM_COH 2
#define TDM_DIR 3 

/* PRIVATE DATA STRUCTURES */

/* PRIVATE ROUTINES */
void 		dots_adpt2_make_trials	(_PRtask);
_PRtrial dots_adpt2_get_trials		(_PRtask, int);
void 		dots_adpt2_set_trials		(_PRtrial);


/* PRIVATE VARIABLES */

/* 
** TASK: DOTS
**		1 dXdots object
**		3 dXtarget objects (fp, 2 targets)
*/
struct _TAtask_struct gl_dots_adpt2_task[] = {

	/* name */
	"dots_adpt2",

	/* menus */
	{
		/* 
		** Task info
		*/
		{ TI, 					1 },
		  { "Trial_order",		1, 1.0 },
		  { "Trial_blocks",		1, 1.0 },
        { "Angle_o",      	0, 1.0 },
        { "Angle_d1",	  180, 1.0 },
        { "Angle_d2",	   30, 1.0 },
        { "setCoh", 	    10101, 0.1 },
        { "FP_CLUT0", 	    5, 1.0},  /* initial FP */
        { "FP_CLUT1", 	    1, 1.0},  /* second  FP */
        { "Drop_codes",  1, 0.1 },
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
   	{ DX_DIAM,      11, 0.1 },
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
	&dots_adpt2_make_trials,
	&dots_adpt2_get_trials,
	&dots_adpt2_set_trials,
};

/* PUBLIC ROUTINE: dots_adpt2_make_trials
**
*/
void dots_adpt2_make_trials(_PRtask task)
{
   static   int coh_list[] = {1, 32, 64, 128, 256, 512, 999}; // 1 is fake to get online performance correctly. Later it'll be converted to 0. 
   int  			 num_coh = sizeof(coh_list)/sizeof(int),
					 *cohs=NULL, coh_n;  // coh_n, num of selected coherences
   int i, j, setCoh, *flagCoh;
	int *rates, rates_n;                 // rate of motion-dir change
	int *cohs_angle=NULL, cohs_angle_n;  // signed coherence
   int flagPosCoh = 0;

    /* Figure out which coherences we'll use based on a menu variable, setCoh. */
    setCoh = TIMV(task, "setCoh"); // mask for filtering out particular coherences from coh_list
    // e.g. to get [64,256,999], setCoh should be [0010101]. These binary correspond to the inclusion of coherences [0,32,64,128,256,512,999]
    // the first binary should be 1
    if(setCoh<0){
		setCoh = -setCoh;
		flagPosCoh = 1;
    }

    coh_n = 0;
    flagCoh = SAFE_ZALLOC(int,num_coh);
    for (i=0; i<num_coh; i++){
        flagCoh[i]     = setCoh % 10; // get the last digit
        setCoh         = (setCoh - flagCoh[i])/10;
        coh_n          =  coh_n  + flagCoh[i]; // count num of selected coherences
    }
    cohs = SAFE_ZALLOC(long, coh_n);
    j = 0; 
    for(i=0; i<num_coh; i++){
        if(flagCoh[i]){
            cohs[j] = coh_list[num_coh-i-1]; // cohs, organized from high to low
            j++;
        }
    }
    
   // absolute coh to signed coh, which will be used for trial type table
   // If setCoh < 0, prepare only positive coherences (2014-02-18) 
   if(flagPosCoh==0){ 
 	  cohs_angle_n = coh_n*2;
		cohs_angle   = SAFE_ZALLOC(long, cohs_angle_n);
		for (i=0; i<coh_n; i++) {
			cohs_angle[2*i]   =  cohs[i];
			cohs_angle[2*i+1] = -cohs[i];
 	     printf("DEBUG: cohs_angle %d, %d\n",cohs_angle[2*i],cohs_angle[2*i+1]);
 	   }
	}else{	
 	   cohs_angle_n = coh_n;
	   cohs_angle   = SAFE_ZALLOC(long, cohs_angle_n);
		for (i=0; i<coh_n; i++) {
			cohs_angle[i]   =  cohs[i];
	      printf("DEBUG: cohs_angle %d\n",cohs_angle[i]);
	    }
	}
	/* 
	** Make array of signed rates
   **  positive, switch between angle_o and (angle_o + angle_d1)
   **  negative, switch between angle_o and (angle_o + angle_d2)
	*/
	rates_n = 8; // slow, mid, fast, very fast. Pos/Neg for Null/Near-Pref  
   rates   = SAFE_ZALLOC(long, rates_n);
	for(i=0;i<rates_n/2;i++) {
		rates[2*i]   =   i+1;
      rates[2*i+1] = -(i+1); 
	} 

	/*
	** utility to make trial set for coh/dir pairs
	*/
	tu_make_trials2D(task,
                 "rates",      rates_n,      rates,      1.0,
				     "cohs_angle", cohs_angle_n, cohs_angle, 1.0);

	/* 
	** Free stuff
	*/
	SAFE_FREE(cohs);
	SAFE_FREE(cohs_angle);
	SAFE_FREE(rates);
   SAFE_FREE(flagCoh);
}


/* PUBLIC ROUTINE: dots_adpt2_get_trials
**
** Returns:
**		_PRtrial if trials remain
**		NULL otherwise
*/
_PRtrial dots_adpt2_get_trials(_PRtask task, int reset_flag)
{
	int trial_blocks = TIMV(task, "Trial_blocks");  
	switch(TIMV(task, "Trial_order")) {

   printf("DEBUG: dots_adpt2_get_trials, check 0");
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


/* PUBLIC ROUTINE: dots_adpt2_set_trials
**
**	Note that here we mostly access groups/lists/properties
**	 by INDEX, not NAME, so be careful if anything
**	 above changes.
**
** TD modified this function based on task_dotsj2.c
*/
void dots_adpt2_set_trials(_PRtrial trial)
{
	_PLlist 	tim = trial->task->task_menus->lists[0], // setup menu
				tgm = trial->task->task_menus->lists[1], // graphics menu: Menu Number 1 is dots. 0,x; 1,y; 10, diam 
                tdm = trial->task->task_menus->lists[2]; // dots menu: 0, seed; 1, speed; 2, coherence; 3 direction

	int			/* the following are arrays of indices
				**		of target objects (fp, t1, t2) in
				**		the task graphics menu and the
				**		dXtargets objects, respectively	
				*/
				tti[] = {0, 3, 4},	
				gti[] = {0, 1, 2},
            gdi[] = {0};

	int			t1ang, t2ang;  /* dot direction, target 1 and 2 directions */
   int         angle = TIMV(trial->task, "Angle_o"),angle_delta = TIMV(trial->task,"Angle_delta");
   int ang, cohv;
   double		ddir;
    
   printf("DEBUG: dots_adpt2_set_trials, check 0\n");
	/* make working copies of the task object menu values */
	pl_list_set_v2w(tgm);
    pl_list_set_v2w(tdm);
    
    // Copy coh from the 2D table (setted up in make_trial) to dots menu working copy
    /* signed coh to absolute coh */
	cohv = PL_L2PV(trial->list, 1); // signed coh
	ang = angle;
    if (cohv<0)
		ang = angle+angle_delta; // null
	cohv = abs(cohv);
    if (cohv==1)
        cohv = 0;
    
    PL_L2PW(tdm,TDM_COH) = cohv; // TD, Copy absolute coherence to dots menu 2nd variable, coherence
    PL_L2PW(tdm,TDM_DIR) = ang;  // TD, Copy angle              to dots menu 3rd variable, direction

    PL_CSET(PL_L2PWS(tgm,kT,5),	PL_L2PW(tdm,TDM_DIR)); // TD, just leave this here. Not sure why graphics menu number 5 is relevant here. 2014-01-29

    /*
	/* Error check object_1, object_2 values .. note
	**		that these are indices of values in the
	**		"Graphics" menu, above.
	
	if(ob1 < 0 || ob1 >= NUM_GR_MENUS)
		ob1 = 0;	/* ob1 default fp
	if(ob2 < 0 || ob2 >= NUM_GR_MENUS)
		ob2 = 3;	/* ob2 default t1

	/* Conditionally copy values of trial/task
	**	 properties to working copies of task menus:
	**	1. rate of change    (of dots)  from trial (property 0)
	**	2. signed coherence  (of dots)  from trial (property 1)
	**	3. Vertex	 (of ob2) = ob1
	
	PL_CSET( PL_L2PW( tdm,2), 	 	PL_L2PV(trial->list, 0));
	PL_CSET( PL_L2PWS(tgm,4,ob2),   PL_L2PV(trial->list, 1));
	PL_CSET( PL_L2PWS(tgm,8,ob2), ob1); */
  
	/* compute xys */
	tu_compute_xyrt(tgm); // not sure if this is necessary in the passive fixation dots. TD, 2014-01-29

    // the following part was just copied from task_dotsj2.c
	/* Set x, y positions in
     **		actual dXtarget objects
     **	Note: the indices here are based on the graphics objects
     **		fixation point	(obj 0)
     **		target 1		(obj 1)
     **		target 2		(obj 2)
     */
	dx_set_by_indexIV(DXF_NODRAW, 0, 3, gti,
                      1, PL_L2PWS(tgm, 0, 0),				PL_L2PWS(tgm, 0, 3),			PL_L2PWS(tgm, 0, 4),
                      2, PL_L2PWS(tgm, 1, 0),				PL_L2PWS(tgm, 1, 3),			PL_L2PWS(tgm, 1, 4),
                      3, PL_L2PWS(tgm, TU_XYRT_LEN, 0),	    PL_L2PWS(tgm, TU_XYRT_LEN,   3),PL_L2PWS(tgm, TU_XYRT_LEN,   4),
                      4, PL_L2PWS(tgm, TU_XYRT_LEN+1, 0),	PL_L2PWS(tgm, TU_XYRT_LEN+1, 3),PL_L2PWS(tgm, TU_XYRT_LEN+1, 4),
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
    
	/* Conditionally drop codes */
	if(TIMV(trial->task, "Drop_codes") != 0){
        ec_send_code_tagged(I_RATE, 7000 + PL_L2PV(trial->list, 0));	/* send tagged ecode for motion-change rate */
        
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
}



