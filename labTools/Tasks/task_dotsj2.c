/* 
** task_dotsj2.c 
**
** Created from task_dotsj.c by TD on 2013-06-24
** 
** 2013-06-24, 25
**  - Created a new menu variable DR0/RF1/SZ3/SP4 to do different tests
**    -- this variable was sent to plexon as an ecode in 785yltd.d
**       (can be used to select trials in onine analysis)
**
**  - Define grid not by step size but by total grid lengths
**    -- Created new menu variables X_len and Y_len
**
**  - Other new menu varialbs: size_lo/size_hi, speed_lo/speed_hi
**  - The highest coh is 1000, not 999
**
** 2013-10-07 TD
**  - possible size values tested: 0 20 30 46 70 106 120
**    -- 320 is too big. Even rex doesn't properly communicate with mac for 320 deg diam...
**    -- these values are even sampling in log space between [20 120].
** 
*/


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
#define TGM_DOTS 1
#define TDM_SPD  1
#define TDM_COH  2
#define TDM_DIR  3
#define kDIAM    10
/* PRIVATE DATA STRUCTURES */

/* PRIVATE ROUTINES */
void     dotsj2_make_trials	(_PRtask);
_PRtrial dotsj2_get_trial		(_PRtask, int);
void 		dotsj2_set_trial		(_PRtrial);
/* test speficit make-trial subsoutines */
void dotsj2_make_trials_DR(_PRtask);
void dotsj2_make_trials_RF(_PRtask);
void dotsj2_make_trials_SZ(_PRtask);
void dotsj2_make_trials_SP(_PRtask);
void dotj2_make_trials_subroutine(_PRtask, int*, int, int, int, int, char*); // used for direction tuning, size tuning, and speed tuning

/* test speficit set-trial subsoutines */
void dotsj2_set_trials_DR(_PLlist, _PLlist, _PRtrial);
void dotsj2_set_trials_RF(_PLlist, _PLlist, _PRtrial);
void dotsj2_set_trials_SZ(_PLlist, _PLlist, _PRtrial);
void dotsj2_set_trials_SP(_PLlist, _PLlist, _PRtrial);

/* GLOVAL VARIABLES */
char TestName[4][20] = { "Direction Tuning", "RF Mapping", "Size Tuning",  "Speed Tuning"};

/* 
** TASK: DOTS
**		1 dXdots object
**		3 dXtarget objects (fp, 2 targets)
*/
struct _TAtask_struct gl_dotsj2_task[] = {

	/* name */
	"dotsj2",

	/* menus */
	{
		/* 
		** Task info
		*/
		{ TI, 					1 },
			{ "Trial_order",		0, 1.0 },
			{ "Trial_blocks",		1, 1.0 },
			{ "DR0/RF1/SZ2/SP3",	0, 1.0 },
			{ "Angle_o",			0, 1.0 },
			{ "Angle_step",			0, 1.0 },
			{ "Angle_n",			0, 1.0 },
			{ "Coherence_lo",		0, 0.1 },
			{ "Coherence_hi",		0, 0.1 },
			{ "X_len",				10, 0.1 },
			{ "Y_len",				10, 0.1 },
			{ "X_o",				0, 0.1 },
			{ "X_n",				1, 1.0 },
			{ "Y_o",				0, 0.1 },
			{ "Y_n",				1, 1.0 },
            { "Diam_lo",			0, 0.1 },
            { "Diam_hi", 			0, 0.1 },
            { "Speed_lo",			0, 0.1 },
            { "Speed_hi",			0, 0.1 },
			{ "Drop_codes",			1,	1.0 },
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
	&dotsj2_make_trials,
	&dotsj2_get_trial,
	&dotsj2_set_trial,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* PUBLIC ROUTINE: dotsj2_make_trials
**
*/
void dotsj2_make_trials(_PRtask task)
{
	int test_switch = TIMV(task, "DR0/RF1/SZ2/SP3"); 

   //printf("DEBUG: make_trials() is entered.\n");	
   switch(test_switch)
	{
	  case 0:
		dotsj2_make_trials_DR(task);
		break;
	  case 1:
		dotsj2_make_trials_RF(task);
		break;
	  case 2:
		dotsj2_make_trials_SZ(task);
		break;
	  case 3:
		dotsj2_make_trials_SP(task);
		break;
	  default:
		dotsj2_make_trials_DR(task);
		break;
	}
}



//////////////////////////////////////////////
// Test specific "make trials" sub routines //
//////////////////////////////////////////////
// TD, 2013-06-25
/*
** direction tuning
*/
void dotsj2_make_trials_DR(_PRtask task)
{
	static int	var_list[] = {0, 32, 64, 128, 256, 512, 1000};
	const  int  num_var    = sizeof(var_list)/sizeof(var_list[0]); // the size cannot be retrieved inside the function to which the array is passed.
 	int var_lo = TIMV(task, "Coherence_lo");
	int var_hi = TIMV(task, "Coherence_hi");
	int flagAngle = 1; // setup different angles according to Setup Task Menu
   char name[10];
   strcpy(name,"coherence"); 
	
   /* Figure out which coherences we'll use.
   ** Set using "Coherence_lo" and "Coherence_hi"
   **    in the task_menu struct. Note that
   **    positive values establish the range of
   **    coherence chosen from the static coh_list;
   **    a negative Coherence_lo means use just
   **    the two values (Coherence_lo and Coherence_hi).
   */
	
	// Common subroutine
	dotj2_make_trials_subroutine(task, var_list, num_var, var_lo, var_hi, flagAngle, name);
}


/*
** RF mapping
*/
///////////////////////////////////
void dotsj2_make_trials_RF(_PRtask task)
{
	int				xl,yl,x0, xn, y0, yn,
					*xs=NULL, *ys = NULL;
	int 			x_step, y_step;
	register int  	i;
	
/*	printf("spatial mapping routine.\n"); */
	
	xl = TIMV(task, "X_len");
	yl = TIMV(task, "Y_len");
	x0 = TIMV(task, "X_o");
	xn = TIMV(task, "X_n");
	y0 = TIMV(task, "Y_o");
	yn = TIMV(task, "Y_n");
	
	/* step */
	x_step = (int) ((double)(xl)/(double)(xn-1) + 0.5);
	y_step = (int) ((double)(yl)/(double)(yn-1) + 0.5);
		
	/* Make array of x pos */
	xs = SAFE_ZALLOC(long, xn);
	for(i=0;i<xn;i++)
		xs[i] = x0 + i*x_step;
	
	/* Make array of y pos */
	ys = SAFE_ZALLOC(long, yn);
	for(i=0;i<yn;i++)
		ys[i] = y0 + i*y_step;
		
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

/*
** Size tuning
*/
///////////////////////////////////
void dotsj2_make_trials_SZ(_PRtask task)
{
	/* DeAngelis and Uka, 2002, J Neurophysiol
	used diameters: 0,   1, 2, 4, 8, 16, 32 deg
	and      speed: 0, 0.5, 1, 2, 4,  8, 16, 32 deg/s
	
	optimal size distribution, 5 - 30 deg
	pref speed distribution,   1 - 30 deg/s
	(often the size of 5 deg and the speed of 6 deg/s are used in Dots task.)
	*/
	
	static int	var_list[] = {0, 20, 30, 46, 70, 106, 160}; // updated 2013-10-07
	const  int  num_var    = sizeof(var_list)/sizeof(var_list[0]);
	int var_lo = TIMV(task, "Diam_lo");
	int var_hi = TIMV(task, "Diam_hi");
	int flagAngle = 0; // Just use a single direction (Dots menu "direction")
   char name[10];
   strcpy(name,"size");
	
   /* Figure out which coherences we'll use.
   ** Set using "Coherence_lo" and "Coherence_hi"
   **    in the task_menu struct. Note that
   **    positive values establish the range of
   **    coherence chosen from the static var_list;
   **    a negative Coherence_lo means use just
   **    the two values (Coherence_lo and Coherence_hi).
   */
	
	// Common subroutine
	dotj2_make_trials_subroutine(task, var_list, num_var, var_lo, var_hi, flagAngle, name);
}


/*
** Speed tuning
*/
///////////////////////////////////
void dotsj2_make_trials_SP(_PRtask task){
	/* DeAngelis and Uka, 2002, J Neurophysiol
	used diameters: 0,   1, 2, 4, 8, 16, 32 deg
	and      speed: 0, 0.5, 1, 2, 4,  8, 16, 32 deg/s
	
	optimal size distribution, 5 - 30 deg
	pref speed distribution,   1 - 30 deg/s
	(often the size of 5 deg and the speed of 6 deg/s are used in Dots task.)
	*/
	
	static int	var_list[] = {0, 5, 10, 20, 40, 80, 160, 320};
	const  int  num_var    = sizeof(var_list)/sizeof(var_list[0]);
   int var_lo = TIMV(task, "Speed_lo");
	int var_hi = TIMV(task, "Speed_hi");
	int flagAngle = 0; // Just use a single direction (Dots menu "direction")
   char name[10];
   strcpy(name,"speed");

   /* Figure out which coherences we'll use.
   ** Set using "Coherence_lo" and "Coherence_hi"
   **    in the task_menu struct. Note that
   **    positive values establish the range of
   **    coherence chosen from the static var_list;
   **    a negative Coherence_lo means use just
   **    the two values (Coherence_lo and Coherence_hi).
   */
	
	// Common subroutine
	dotj2_make_trials_subroutine(task, var_list, num_var, var_lo, var_hi, flagAngle,name);
}


/*
** Common process for direction, size, and speed tuning tests.
**		var_list, the list of parameter values tested
**		var_lo and var_hi, only the values within the range [var_lo var_hi] are selected.
**		flagAngle, setup different angles as the second stimulus parameter. 1 for direction tuning, 0 for size/speed tuning 
**
**		TD, 2013-06-26
*/
void dotj2_make_trials_subroutine(_PRtask task, int var_list[], int num_var, int var_lo, int var_hi, int flagAngle, char name[])
{
	int		 *vars=NULL, *angle=NULL, var_n, ang_n;
   int       j;
   //printf("DEBUG: make_trials_subroutine() is entered.\n");

	
	if(var_hi <= var_lo || (var_lo < 0 && var_hi >= 0)) {
      //printf("DEBUG: the abs value of _lo is used\n");

		/* send just one value (abs value of var_lo) */
		var_n	= 1;
		vars 	= SAFE_ZALLOC(long, 1);
		vars[0] = abs(var_lo); 

   } else if(var_lo < 0 && var_hi < 0) {
      //printf("DEBUG: the abs values of _lo and _hi are used\n");

		/* both lo and hi are negative, use abs values */
		var_n	= 2;
		vars	= SAFE_ZALLOC(int, 2);
		vars[0] = -var_lo; 
		vars[1] = -var_hi; 

	} else {

		/* Otherwise find sequence between lo and hi
		**		from var_list, defined above
		*/
		register int i, ilo = -1, ihi = -1;
		while(++ilo < num_var-1 && var_list[ilo] < var_lo) ;
		while(++ihi < num_var-1 && var_list[ihi] < var_hi) ;
	
    /*printf("DEBUG: ihi=%d, ilo=%d\n",ihi,ilo);
      printf("DEBUG: num_var=%d\n",num_var);
      for(j=0;j<7;j++)
 		{
      	printf("DEBUG: var_list[%d]=%d\n",j,var_list[j]);
      } */

		var_n = ihi - ilo + 1;
		vars  = SAFE_ZALLOC(long, var_n);
		for(i=0;i<var_n;i++,ilo++)
			vars[i] = var_list[ilo];
	}

	/* 
	** Make array of angles
	*/
	if(flagAngle){
		ang_n  = toy_boundi(TIMV(task, "Angle_n"), 1, 1000);
		angle  = toy_seriesi(ang_n, TIMV(task, "Angle_o"), TIMV(task, "Angle_step"));
	}else{
		ang_n    = 1;
      angle    = SAFE_ZALLOC(int,ang_n); 
		angle[0] = 0; // this value will not be used actually; Instead Dots Task Menu "direction" will be used. 
	}
	/*
	** utility to make trial set for coh/dir pairs
	*/
	/* TD, 2013-06-25
		PL list with the name 2dtc is created wiht the parameter names DX_COH (defined as "coherence") and "angle".
			My guess is as follows:
			task->trials[k]->list->(property: parameter name1)->value
			task->trials[k]->list->(property: parameter name2)->value
	*/
	tu_make_trials2D(task, 
			name,	  var_n, vars,  0.1,
			"angle",ang_n, angle, 0.1);

	/* 
	** Free stuff
	*/
	SAFE_FREE(vars);
	SAFE_FREE(angle);
}
//////////////////// Task specific make_trial subroutiens end here ////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* PUBLIC ROUTINE: dotsj2_get_trial
**
** Returns:
**		_PRtrial if trials remain
**		NULL otherwise
*/
_PRtrial dotsj2_get_trial(_PRtask task, int reset_flag)
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


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* PUBLIC ROUTINE: dotsj2_set_trial
**
**	Note that here we mostly access groups/lists/properties
**	 by INDEX, not NAME, so be careful if anything
**	 above changes.
*/
void dotsj2_set_trial(_PRtrial trial)
{
	_PLlist 	tim = trial->task->task_menus->lists[0], // setup menu
				tgm = trial->task->task_menus->lists[1], // graphics menu: Menu Number 1 is dots. 0,x; 1,y; 10, diam
				tdm = trial->task->task_menus->lists[2]; // dots menu: 0, seed; 1, speed; 2, coherence; 3 direction

	int 		/* the following are arrays of indices 
				**		of target objects (fp, t1, t2) in
				**		the task graphics menu and the
				**		dXtargets objects, respectively	
				*/
				tti[] = {0, 3, 4},	
				gti[] = {0, 1, 2},
				gdi[] = {0};

	int			t1ang, t2ang,  /* dot direction, target 1 and 2 directions */
				test_switch;  /* 0, direction; 1, RF mapping; 2, Size; 3, Speed*/
	double		ddir;
	
	/* make working copies of the task object menu values */
	pl_list_set_v2w(tgm);
	pl_list_set_v2w(tdm);
	
	/* figure out the task */
	test_switch  = TIMV(trial->task, "DR0/RF1/SZ2/SP3"); // access via name is safer.
	//test_switch  = PL_L2PV(tim, 2);		/* get DR0/RF1/SZ2/SP3 */
    if(test_switch>-1 && test_switch<4){
        printf("%s\n",&TestName[test_switch][0]);
    }else{   
        printf("%s\n",&TestName[0][0]);
    }
	
	///////////////////////////////////////////////////
	//////////// Test specific procedures /////////////
	///////////////////////////////////////////////////
	// TD, 2013-06-25
	/*	0. Dir tuning
			0-1. copy 2D table vavlues to dots-menu coherence and direction
			0-2. conditionally set x/y using joy
		1. RF mapping
			1-1. copy 2D table values to graphics menu kX and kY
		2. Size tuning
			2-1. copy 2D table values to graphics menu diameter (menu number 1)
		3. Speed tuning
			2-1. copy 2D table values to dots menu speed */
	switch(test_switch)
	{
	  case 0: /* Dir Tunign */
		dotsj2_set_trials_DR(tgm, tdm, trial);
		break;
	  case 1: /* RF mapping*/
		dotsj2_set_trials_RF(tgm, tdm, trial);
		break;
	  case 2: /* Size tuning */
		dotsj2_set_trials_SZ(tgm, tdm, trial);
		break;
	  case 3: /* Speed tuning */
		dotsj2_set_trials_SP(tgm, tdm, trial);
		break;
	  default: /* Otherwise */
		dotsj2_set_trials_DR(tgm, tdm, trial);
		break;
	}

	/* Also need to set the property 't' of the 'dots direction' (obj 5 in tgm)
	**
	** This is only to take advantage of the 'wrt' function.
	** Set 'wrt' of the targets to that of the dots direction will 
	** align the target 'planets' to the axis of the dots stimulus. 
	** Actually, this is quite irrelevant for the Fine Discrimination task.
	*/
	/* Copy value from tdm(3) (dots menu, number 3, direction) to tgm(kT,5). kT is 5, defined in task_utiliteis.h*/
	PL_CSET(PL_L2PWS(tgm,kT,5),	PL_L2PW(tdm,TDM_DIR)); // TD, just leave this here. Not sure why graphics menu number 5 is relevant here. 2013-006-25

	/* Check parameters */
	printf("----------------------\n");
	printf("(x,y) = (%d, %d)\n",  PL_L2PWS(tgm,kX,TGM_DOTS),PL_L2PWS(tgm,kY,TGM_DOTS));
	printf("Dir %d, Coh %d\n",    PL_L2PW(tdm,TDM_DIR),		PL_L2PW(tdm,TDM_COH));
	printf("Speed %d, Size %d\n", PL_L2PW(tdm,TDM_SPD),		PL_L2PWS(tgm,kDIAM,TGM_DOTS));
	printf("----------------------\n");
	
	/* Conditionally copy values of trial/task for targets
	**	 properties to working copies of task menus:
	**	3. Angle 	 (of ob2)  from trial (property 1)
	**	4. Vertex	 (of ob2) = ob1
	*/
	/* compute target 1 and 2 direction */
	ddir    = cos(PL_L2PW(tdm, 3)*3.141592654/180);
	if(ddir>0)	{ t1ang=0; t2ang=180;}
	else		{ t1ang=180; t2ang=0;}
	// TD, here setting the graphics menu "t" variable for target 1 and target 2.
	PL_CSET( PL_L2PWS(tgm,5,tti[1]),   t1ang);
	PL_CSET( PL_L2PWS(tgm,5,tti[2]),   t2ang);
	
	/* debugging stuff added by YL */
/*	printf("before compute: WS tgm,0,1 %d\n", PL_L2PWS(tgm,0,1));
	printf("before compute: VS tgm,0,1 %d\n", PL_L2PVS(tgm,0,1)); */

	/* compute xys */
	tu_compute_xyrt(tgm);

/*	printf(" after compute: WS tgm,0,1 %d\n", PL_L2PWS(tgm,0,1));
	printf(" after compute: VS tgm,0,1 %d\n", PL_L2PVS(tgm,0,1)); */

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
		3, PL_L2PWS(tgm, TU_XYRT_LEN, 0),	PL_L2PWS(tgm, TU_XYRT_LEN, 3),	PL_L2PWS(tgm, TU_XYRT_LEN, 4),
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


//////////////////////////////////////////////
// Test specific "set trials" sub routines //
//////////////////////////////////////////////
// Copying values from 2D tables (setted up in make_trial()) to dots and/or graphics menu working copies
// TD, 2013-06-25
/*
** direction tuning
*/
void dotsj2_set_trials_DR(_PLlist tgm, _PLlist tdm, _PRtrial trial)
{
		PL_L2PW(tdm,TDM_COH) = PL_L2PV(trial->list, 0); // TD, Copy 2D table's 0th variable ("coherence") to dots menu 2nd variable, coherence
		PL_L2PW(tdm,TDM_DIR) = PL_L2PV(trial->list, 1); // TD, Copy 2D table's 1st variable ("angle")     to dots menu 3rd variable, direction
		/*
		** If either X/Y for obj 1 (dots) is NULL => use joyh/joyv
		** pl_list_set_v2w(tgm) should have taken care of the default case
		*/
		if (PL_L2PVS(tgm,kX,TGM_DOTS) == NULLI | PL_L2PVS(tgm,kY,TGM_DOTS) == NULLI)
		{
			PL_L2PWS(tgm, kX, TGM_DOTS) = joyh/10;
			PL_L2PWS(tgm, kY, TGM_DOTS) = joyv/10;
		} 
}

/*
** RF mapping
*/
void dotsj2_set_trials_RF(_PLlist tgm, _PLlist tdm, _PRtrial trial)
{
		PL_L2PWS(tgm,kX,TGM_DOTS) = PL_L2PV(trial->list,0); // set graphics menu 1 (dots) x with the 2D table's first variable 
		PL_L2PWS(tgm,kY,TGM_DOTS) = PL_L2PV(trial->list,1); // set graphics menu 1 (dots) y with the 2D table's second variable
}

/*
** Size tuning
*/
void dotsj2_set_trials_SZ(_PLlist tgm, _PLlist tdm, _PRtrial trial)
{
	PL_L2PWS(tgm,kDIAM,TGM_DOTS) = PL_L2PV(trial->list,0); // set graphics menu 1 (dots) diameter with the 2D table's first variable 
}

/*
** Speed tuning
*/
void dotsj2_set_trials_SP(_PLlist tgm, _PLlist tdm, _PRtrial trial)
{
	PL_L2PW(tdm,TDM_SPD) = PL_L2PV(trial->list,0); // set dots menu speed with the 2D table's first variable 
}
