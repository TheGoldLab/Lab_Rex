/*
** task_dotsj.c
**
**	Standard 2-alternative forced-choice
**		dots task for Jeff's MT LIP fine discrimintion experiment
**     also for Yin's motion-saccade tasks (can be used for mapping MT RFs)
**  
**  Modified from task_dots.c on 12/08/08
**  2011/05/11 yl - fixed ecode issue w/ t1_x
**  2011/05/17 yl - use joyh/v for dots_x/y if either menu value is NULL
**  2011/11/06 yl - add ability to create x/y grid for spatial mapping
*/

#include "../toys.h"
#include "../task.h"
#include "../task_utilities.h"

/* PRIVATE MACROS/CONSTANTS */
#define NUM_GR_MENUS 6
#define TI 				"setup"
#define TIMV(ta,n)	pl_list_get_v((ta)->task_menus->lists[0], (n))

/* PRIVATE DATA STRUCTURES */

/* PRIVATE ROUTINES */
void 		dotsj_make_trials	(_PRtask);
_PRtrial dotsj_get_trial		(_PRtask, int);
void 		dotsj_set_trial		(_PRtrial);

/* PRIVATE VARIABLES */

/* 
** TASK: DOTS
**		1 dXdots object
**		3 dXtarget objects (fp, 2 targets)
*/
struct _TAtask_struct gl_dotsj_task[] = {

	/* name */
	"dotsj",

	/* menus */
	{
		/* 
		** Task info
		*/
		{ TI, 					1 },
			{ "Trial_order",		0, 1.0 },
			{ "Trial_blocks",		1, 1.0 },
			{ "Dir0/Sp1",			0, 1.0 },
			{ "Angle_o",      	0, 1.0 },
			{ "Angle_step",     	0, 1.0 },
			{ "Angle_n",      	0, 1.0 },
			{ "Coherence_lo", 	0, 0.1 },
			{ "Coherence_hi", 	0, 0.1 },
			{ "Sp_step",			20, 0.1 },
			{ "X_o",				0, 0.1 },
			{ "X_n",				1, 1.0 },
			{ "Y_o",				0, 0.1 },
			{ "Y_n",				1, 1.0 },
			{ "Drop_codes",		1,	1.0 },
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
	&dotsj_make_trials,
	&dotsj_get_trial,
	&dotsj_set_trial,
};

/* PUBLIC ROUTINE: dotsj_make_trials
**
*/
void dotsj_make_trials(_PRtask task)
{
   static   int coh_list[] = {0, 32, 64, 128, 256, 512, 999};
   int  			task_switch = TIMV(task, "Dir0/Sp1"), 
   					num_coh = sizeof(coh_list)/sizeof(int),
					 *cohs=NULL, *angs, coh_n,
					 coh_lo, coh_hi, ang_n,
					 sp_step, x0, xn, y0, yn,
					 *xs=NULL, *ys = NULL;

	if (task_switch == 0)
	{
	/*  
	** dir tuning
	*/
	
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
	} else {
	register int  i;
	
	
	/*
	** spatial RF mapping **
	*/
	
/*	printf("spatial mapping routine.\n"); */
	
	sp_step = TIMV(task, "Sp_step");
	x0 = TIMV(task, "X_o");
	xn = TIMV(task, "X_n");
	y0 = TIMV(task, "Y_o");
	yn = TIMV(task, "Y_n");
	
	
	/* Make array of x pos */
	xs = SAFE_ZALLOC(long, xn);
	for(i=0;i<xn;i++)
		xs[i] = x0 + i*sp_step;
	
	/* Make array of y pos */
	ys = SAFE_ZALLOC(long, yn);
	for(i=0;i<yn;i++)
		ys[i] = y0 + i*sp_step;
		
	/*
	** make trial set for x/y pairs
	*/
	tu_make_trials2D(task,
		"x", xn, xs, 0.1,
		"y", yn, ys, 0.1);
	
	
	/* 
	** Free stuff
	*/
	SAFE_FREE(xs);
	SAFE_FREE(ys);
	}
}

/* PUBLIC ROUTINE: dotsj_get_trial
**
** Returns:
**		_PRtrial if trials remain
**		NULL otherwise
*/
_PRtrial dotsj_get_trial(_PRtask task, int reset_flag)
{
	int trial_blocks = TIMV(task, "Trial_blocks");

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
}

/* PUBLIC ROUTINE: dotsj_set_trial
**
**	Note that here we mostly access groups/lists/properties
**	 by INDEX, not NAME, so be careful if anything
**	 above changes.
*/
void dotsj_set_trial(_PRtrial trial)
{
	_PLlist 	tim = trial->task->task_menus->lists[0],
				tgm = trial->task->task_menus->lists[1],
				tdm = trial->task->task_menus->lists[2];

	int 		
			    /* the following are arrays of indices 
				**		of target objects (fp, t1, t2) in
				**		the task graphics menu and the
				**		dXtargets objects, respectively	
				*/
				tti[] = {0, 3, 4},	
				gti[] = {0, 1, 2},
				gdi[] = {0};

	int       t1ang, t2ang, /* dot direction, target 1 and 2 directions */
			   task_switch;  /* whether dir tuning OR spatial mapping */
	double ddir;
	
	/* make working copies of the task object menu values */
	pl_list_set_v2w(tgm);
	pl_list_set_v2w(tdm);
	
	
	/* figure out the task */
	task_switch = PL_L2PV(tim, 2);		/* get Dir0/Sp1 */
	printf("task = %d\n", task_switch);
	
	/* Conditionally copy values of trial/task for dots
	**	 properties to working copies of task menus:
	**	1. Coherence (of dots) from trial (property 0)
	**  2. Dot direction
	*/
	if(task_switch == 0) {
		/* if dir tuning: pick from 2D list */
		PL_L2PW( tdm,2) =	PL_L2PV(trial->list, 0);
		PL_L2PW( tdm,3) =	PL_L2PV(trial->list, 1);
	} else {
		/* if sp mapping: use whatever's in the dots menu */
/*		PL_CSET( PL_L2PW( tdm,2),		PL_L2PV(tdm, 2));
		PL_CSET( PL_L2PW( tdm,3),		PL_L2PV(tdm, 3)); */
	}

	/* Also need to set the property 't' of the 'dots direction' (obj 5 in tgm) */
	/* This is only to take advantage of the 'wrt' function.
	**	Set 'wrt' of the targets to that of the dots direction will 
	** align the target 'planets' to the axis of the dots stimulus. 
	** Actually, this is quite irrelevant for the Fine Discrimination task.
	*/

	/* Copy value from tdm(3) to tgm(kT,5) */
	PL_CSET(	PL_L2PWS(tgm,kT,5),	PL_L2PW(tdm,3)	);		
	
	/* Set up x/y position for the dot stimulus */
	
	if(task_switch == 0) {
		/*
		** If either X/Y for obj 1 (dots) is NULL => use joyh/joyv
		** pl_list_set_v2w(tgm) should have taken care of the default case
		*/

		if (PL_L2PVS(tgm,kX,1) == NULLI |
	    	PL_L2PVS(tgm,kY,1) == NULLI)
		{
/*		printf("hi1\n");*/
			PL_L2PWS(tgm, kX, 1) = joyh/10;
			PL_L2PWS(tgm, kY, 1) = joyv/10;
		} 
		else 
		{
/*		printf("hi2\n");*/
/*		PL_CSET( PL_L2PWS(tgm, kX, 1), PL_L2PVS(tgm, kX,1));
		PL_CSET( PL_L2PWS(tgm, kY, 1), PL_L2PVS(tgm, kY,1));*/
		}
	} else {
		/* pick from the 2D list */
/*		printf("(x0,y0) = (%d,%d)\n",PL_L2PV(trial->list,0),
		                           PL_L2PV(trial->list,1));
	*/	                           
		PL_L2PWS(tgm,kX,1) = PL_L2PV(trial->list,0);
		PL_L2PWS(tgm,kY,1) = PL_L2PV(trial->list,1);
/*		PL_CSET( PL_L2PWS( tgm, kX,1), PL_L2PV(trial->list, 0));
		PL_CSET( PL_L2PWS( tgm, kY,1), PL_L2PV(trial->list, 1)); */
		printf("(x,y) = (%d,%d)\n", PL_L2PWS(tgm,kX,1),
		                           PL_L2PWS(tgm,kY,1));
	}	
	
	/* Conditionally copy values of trial/task for targets
	**	 properties to working copies of task menus:
	**	3. Angle 	 (of ob2)  from trial (property 1)
	**	4. Vertex	 (of ob2) = ob1
	*/
	/* compute target 1 and 2 direction */
	ddir    = cos(PL_L2PW(tdm, 3)*3.141592654/180);
	if(ddir>0) { t1ang=0; t2ang=180;}
	else         { t1ang=180; t2ang=0;}
	PL_CSET( PL_L2PWS(tgm,5,tti[1]),   t1ang);
	PL_CSET( PL_L2PWS(tgm,5,tti[2]),   t2ang);
	
	/* debugging stuff added by YL */
/*	printf("before compute: WS tgm,0,1 %d\n", PL_L2PWS(tgm,0,1));
	printf("before compute: VS tgm,0,1 %d\n", PL_L2PVS(tgm,0,1));
*/	


	/* compute xys */
	tu_compute_xyrt(tgm);


/*	printf(" after compute: WS tgm,0,1 %d\n", PL_L2PWS(tgm,0,1));
	printf(" after compute: VS tgm,0,1 %d\n", PL_L2PVS(tgm,0,1));
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



	/* Conditionally drop codes */
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

}
