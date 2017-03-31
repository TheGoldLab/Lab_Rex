/*
** task_dotsrg.c
**
** 2-alternative forced-choice dots task with extra stuff for red-green
**
*/

#include "../toys.h"
#include "../task.h"
#include "../task_utilities.h"
#include "../paradigm_rec.h"

/* GLOBAL VARIABLES */
static int	target1_angle=0;	/* one for each target */
static int	target2_angle=0;	/* one for each target */

/* PRIVATE MACROS/CONSTANTS */
#define NUM_GR_MENUS 6
#define TI 				"setup"			/* RG setup */
#define TT 				"T2_timing"		/* timing */
#define TIMV(ta,n)	pl_list_get_v((ta)->task_menus->lists[0], (n))
#define TTMV(ta,n)	pl_list_get_v((ta)->task_menus->lists[1], (n))

/* PRIVATE ROUTINES */
void 		dotsrg_make_trials	(_PRtask);
_PRtrial dotsrg_get_trial		(_PRtask, int);
void 		dotsrg_set_trial		(_PRtrial);

/* PRIVATE VARIABLES */

/* 
** TASK: DOTSRG
**		1 dXdots object
**		3 dXtarget objects (fp, 2 targets)
*/
struct _TAtask_struct gl_dotsrg_task[] = {

	/* name */
	"dotsrg",

	/* menus */
	{
		/* 
		** Task info
		*/
		{ TI, 					1 },
		{ "Trial_order",			0, 1.0 },
		{ "Trial_blocks",		0, 1.0 },
    		{ "Angle_o",      	0, 1.0 },
   		{ "Angle_step",     	0, 1.0 },
   		{ "Angle_n",      	0, 1.0 },
   		{ "Coherence_lo", 	0, 1.0 },
   		{ "Coherence_hi", 	0, 1.0 },
		{ "Drop_codes",		1, 1.0 },
		{ "Target_offset",	0, 1.0 },
		{ "Target_amplitude", 	0, 1.0 },
		{ "Dangle",          	0, 1.0 },
		{ "Dangle_limit", 	0, 1.0 },
   		{ NULL },

	/* menus */
        	/*
        	** Task2 timing
        	*/
        	{ TT,                                   1 },
		{ "Flag",   	   0, 1.0 },
		{ "Dots_Total",    0, 1.0 },
        	{ "T_on",      	   0, 1.0 },
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
	&dotsrg_make_trials,
	&dotsrg_get_trial,
	&dotsrg_set_trial,
};

/* PUBLIC ROUTINE: dotsrg_make_trials
**
*/
void dotsrg_make_trials(_PRtask task)
{
   static   int coh_list[] = {0, 32, 64, 128, 256, 512, 999};
   int  			 num_coh = sizeof(coh_list)/sizeof(int),
					 *cohs=NULL, *angs, coh_n,
					 coh_lo, coh_hi, ang_n;

   /* Figure out which coherences we'll use.
   ** Set using "Coherence_lo" and "Coherence_hi"
   **    in the task_menu struct. Note that
   **    positive values establish the range of
   **    coherence chosen from the static coh_list;
   **    a negative Coherence_lo means use just
   **    the two values (Coherence_lo and Coherence_hi).
   */
	coh_lo = TIMV(task, "Coherence_lo");
	coh_hi = TIMV(task, "Coherence_hi");

   if(coh_hi <= coh_lo || (coh_lo < 0 && coh_hi > 0)) {

		/* send just one value (abs value of coh_lo) */
		coh_n   = 1;
		cohs 	  = SAFE_ZALLOC(long, 1);
		cohs[0] = abs(coh_lo); 

   } else if(coh_lo < 0 && coh_hi <= 0) {

		/* both lo and hi are negative, use abs values */
		coh_n   = 2;
		cohs 	  = SAFE_ZALLOC(int, 2);
		cohs[0] = -coh_lo; 
		cohs[1] = -coh_hi; 

	} else {

		/* Otherwise find sequence between lo and hi
		**		from coh_list, defined above
		*/
   	register int i, ilo = -1, ihi = -1;

      while(++ilo < num_coh-1 && coh_list[ilo] < coh_lo) ;
      while(++ihi < num_coh-1 && coh_list[ihi] < coh_hi) ;

		coh_n = ihi - ilo + 1;
		cohs  = SAFE_ZALLOC(long, coh_n);
		for(i=0;i<coh_n;i++,ilo++)
			cohs[i] = coh_list[ilo];
   }

	/* 
	** Make array of angles
	*/
	ang_n = toy_boundi(TIMV(task, "Angle_n"), 1, 1000);
	angs  = toy_seriesi(ang_n, 
					TIMV(task, "Angle_o"), 
					TIMV(task, "Angle_step"));

	/*
	** utility to make trial set for coh/dir pairs
	*/
	tu_make_trials2D(task, 
			DX_COH,	coh_n, cohs, 0.1,
			"angle",	ang_n, angs, 0.1);

	/* 
	** Free stuff
	*/
	SAFE_FREE(cohs);
	SAFE_FREE(angs);
}

/* PUBLIC ROUTINE: dotsrg_get_trial
**
** Returns:
**		_PRtrial if trials remain
**		NULL otherwise
*/
_PRtrial dotsrg_get_trial(_PRtask task, int reset_flag)
{
		int trial_blocks = TIMV(task, "Trial_blocks");

		return(tu_get_random(task, trial_blocks, reset_flag));
}

/* PUBLIC ROUTINE: dotsrg_set_trial
**
**	Note that here we mostly access groups/lists/properties
**	 by INDEX, not NAME, so be careful if anything
**	 above changes.
*/
void dotsrg_set_trial(_PRtrial trial)
{
	/* Properties of target objects */
	_PLlist 	tim = trial->task->task_menus->lists[0];
	_PLlist	tgm = trial->task->task_menus->lists[2];
	_PLlist	tdm = trial->task->task_menus->lists[3];
	_PRrecord rec = pr_get_rec();
	
	int dangle = TIMV(trial->task, "Dangle");
	int dangle_l = TIMV(trial->task, "Dangle_limit");
	int toff = TIMV(trial->task, "Target_offset");
	int ang2use, coh2use;
	int t1ang, t2ang;
	int dang=0;
	int *tang;
	int radius = TIMV(trial->task, "Target_amplitude");
	
				/* the following are arrays of indices 
				**		of target objects (fp, t1, t2) in
				**		the task graphics menu and the
				**		dXtargets objects, respectively	
				*/
				int targi[3] = {0, 1, 2};
				int dotsi[1] = {0};

	/* make working copies of the task object menu values */
	pl_list_set_v2w(tgm);

	/* Set properties for graphics object 3 (green) and 4 (red) */
	coh2use = PL_L2PV(trial->list, 0);
	ang2use = PL_L2PV(trial->list, 1);
	
	if(pl_list_get_w(rec->paradigm_menu, "Trial_repeats")==0) { 
		while(dang!=1) {
			tang=tu_compute_dangle(ang2use, toff, dangle, radius, dangle_l);
			dang=tang[2];
		}
		t1ang=tang[0];
		t2ang=tang[1];	
	
		/* save the angles for future use if needed */
		target1_angle = t1ang;
		target2_angle = t2ang;
	
	} else { 					
		t1ang = target1_angle;	
		t2ang = target2_angle; 
	}

	/* Fill the values for object 3 & 4 */
	PL_CSET( PL_L2PWS(tgm, 5, 3), t1ang);
	PL_CSET( PL_L2PWS(tgm, 5, 4), t2ang);	
	
	PL_CSET( PL_L2PWS(tgm, 8, 3), 0);
	PL_CSET( PL_L2PWS(tgm, 8, 4), 0);
	
	PL_CSET( PL_L2PWS(tgm, 4, 3), radius);
	PL_CSET( PL_L2PWS(tgm, 4, 4), radius);
	
	printf("Current radius: %d\n", PL_L2PWS(tgm, 4, 4));

	/* compute xys */
	tu_compute_xyrt(tgm);

	printf("Current angle: %d coherence: %d radius: %d\n", ang2use, coh2use, radius);

	/* Set x,y positions in dXtarget objects */
	dx_set_by_indexIV(DXF_NODRAW, 0, 3, targi, 
		1, PL_L2PWS(tgm, 0, 0), 		PL_L2PWS(tgm, 0, 3), 		PL_L2PWS(tgm, 0, 4), 
		2, PL_L2PWS(tgm, 1, 0), 		PL_L2PWS(tgm, 1, 3), 		PL_L2PWS(tgm, 1, 4), 
		3, PL_L2PWS(tgm, TU_XYRT_LEN, 0), 	PL_L2PWS(tgm, TU_XYRT_LEN, 3), PL_L2PWS(tgm, TU_XYRT_LEN, 4),
		4, PL_L2PWS(tgm, TU_XYRT_LEN+1, 0), PL_L2PWS(tgm, TU_XYRT_LEN+1, 3), PL_L2PWS(tgm, TU_XYRT_LEN+1, 4),
		ENDI);

	/* set dXdots object properties */
	dx_set_by_nameIV(DXF_D1, DX_DOTS, 1, dotsi,
			DX_X,		0,
			DX_Y, 		0,
			DX_DIAM,	PL_L2PWS(tgm, TU_XYRT_LEN, 1),
			DX_CLUT,	PL_L2PWS(tgm, TU_XYRT_LEN+1, 1),
			DX_COH, 	coh2use,
			DX_DIR, 	ang2use,
 			DX_SEED, 	tu_get_seed(PL_L2PVS(tdm, 0, 0), coh2use, ang2use),	
			DX_SPEED,  	PL_L2PVS(tdm, 1, 0),
 			NULL);
 			
 	/* Conditionally drop codes */
	if(TIMV(trial->task, "Drop_codes") != 0) {
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
			DX_SEED,	0, 	I_DTVARCD,
			NULL);
	}
}
