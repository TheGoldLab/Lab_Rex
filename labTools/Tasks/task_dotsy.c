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
void     dotsy_make_trials  (_PRtask);
_PRtrial dotsy_get_trial    (_PRtask, int);
void     dotsy_set_trial    (_PRtrial);

/* helper fxn for making trials
** yl_task_make_trialP_... makes an array of (possibly shuffled) trials
** yl_get_..._trial        grabs the next trial from the above array
*/
void     yl_task_make_trialP_MT  (_PRtask, int);        /* creates array in order - from pr_task_make_trialP_all */
_PRtrial yl_get_MT_trial         (_PRtask, int, int);	/* randomizes trials from yl_..._trialP_MT; from tu_get_random */

void     yl_task_make_trialP_LIP (_PRtask, int);        /* creates random stim trials + nonstim trials */
_PRtrial yl_get_LIP_trial        (_PRtask, int, int);   /* from tu_get_block */

/* helper fxn for displaying trials */
void     yl_task_print           (_PRtask, int);
void     yl_trial_print          (_PRtrial);				/* counts # of times that trial is used */
void     yl_trialPs_print        (_PRtrial *, int, int);/* prints all the trial IDs in order */



/* PRIVATE VARIABLES */
enum MENU_INDEX {
	TI_MENU,
	GRAPHICS_MENU,
	DOTS_MENU,
	COH_MENU
};

/* 
** TASK: DOTS
**		1 dXdots object
**		3 dXtarget objects (fp, 2 targets)
*/
struct _TAtask_struct gl_dotsy_task[] = {

	/* name */
	"dotsy",

	/* menus */
	{
		/* 
		** Task info: 'setup' menu
		*/
		{ TI, 				1 },
		{ "Trial_order",	0, 		1.0 },	/* 0 = randomize, 1 = in order*/
		{ "Trial_blocks",	1,		1.0 },	/* # trials per cond */
		{ "Angle_MT",		0,		1.0 },	/* angle of the MT cell */
		{ "Angle_offset",	90,		1.0 },	/* offset from MT tuning for dots */
		{ "Pos_x_MT",		0, 		0.1 }, /* ctr of MT RF */
		{ "Pos_y_MT",		0, 		0.1 },	
		{ "Pos_r_LIP",		0, 		0.1 },	/* LIP RF */
		{ "Pos_t_LIP",		0, 		1.0 },
		{ "Stim_flag", 	0, 		1.0 },	/* 1 = MT stim, 2 = LIP stim */
		{ "Max_coh2stim",	500,	0.1 },	/* for MT stim trials */
		{ "Stim_block_len"	,	10,		1.0 }, /* for LIP stim trials */	
		{ "Rew_prob",		NULLI,	0.1 }, /* used by 785yl */
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

		/*
		** Coherence menu
		*/
		{ "coherence", 1 },
		{ "Coherence_0",     1,	0.1 },
		{ "Coherence_1",   500,	0.1 },
		{ "Coherence_2",  1000,	0.1 },
		{ "Coherence_3", NULLI,	0.1 },
		{ "Coherence_4", NULLI,	0.1 },
		{ "Coherence_5", NULLI,	0.1 },
		{ "Coherence_6", NULLI,	0.1 },
		{ "Coherence_7", NULLI,	0.1 },
		{ "Coherence_8", NULLI,	0.1 },
		{ "Coherence_9", NULLI,	0.1 },
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
	&dotsy_make_trials,
	&dotsy_get_trial,
	&dotsy_set_trial,
};

/* PUBLIC ROUTINE: dotsy_make_trials
**
*/
void dotsy_make_trials(_PRtask task)
{
	/* convention: make coh = 1 signify 0% coherence so we can use signed coh
	**	will set to 0% coherence in set_trials
	** the angle of dots will be determined by the Angle_MT menu value
	**/ 
   	int *cohs = NULL, coh_n=0;												/* array of unsigned coherence */
   	int *signcohs = NULL, signcohs_n;									/* array of signed coherences */
   	int i;																				/* counter for later */
   	int sflag, *smodes = NULL, smodes_n;			/* from "Stim_flag", array of stim modes */

	/* get the coherences */
   	while(GETMVI(task,COH_MENU,coh_n) != NULLI && coh_n < 10) coh_n++;
   	signcohs_n = coh_n*2;
   	signcohs = SAFE_ZALLOC(long, signcohs_n);
	for(i = 0; i < coh_n; i++) {
		signcohs[2*i] = GETMVI(task,COH_MENU,i);
		signcohs[2*i+1] = -GETMVI(task,COH_MENU,i);
	}
	
/*	printf("eyeh %d\n", eyeh); */

/*	printf("coh_n %d\n", coh_n); */

	/* Make array of stim modes
	** sflag = 0: no stim 
	** sflag = 1: MT stim
	** sflag = 2: LIP stim
	** smode is either 0 or 1. 
	** During trial, we will grab from Stim_flag menu to determine what kind of stim
	*/ 
	sflag = TIMV(task, "Stim_flag");
	
	if(sflag == 0) {
		/* stim mode will consist only of 0s */
		smodes_n = 1;
		smodes = SAFE_ZALLOC(int, smodes_n); 
		smodes[0] = 0;
	} else {
		/* stim mode consists of {0, 1} */
		smodes_n = 2;
		smodes = SAFE_ZALLOC(int, smodes_n);
		smodes[0] = 0;
		smodes[1] = 1;
	} 	

	/*
	** utility to make trial set for scoh/smode pairs
	*/
	tu_make_trials2D(task,
			"scoh",	signcohs_n, signcohs, 0.1,
			"smode", smodes_n,   smodes,   1.0);

	/* 
	** Free stuff
	*/
	SAFE_FREE(cohs); SAFE_FREE(signcohs);
	SAFE_FREE(smodes);
/*	SAFE_FREE(angs);*/
}

/* PUBLIC ROUTINE: dotsy_get_trial
**
** Returns:
**		_PRtrial if trials remain
**		NULL otherwise
*/
_PRtrial dotsy_get_trial(_PRtask task, int reset_flag)
{
	int trial_blocks = TIMV(task, "Trial_blocks");
	int sflag = TIMV(task, "Stim_flag");
	int maxcoh = TIMV(task, "Max_coh2stim");
	
	switch(sflag) {

		/* No stim */
		case 0:
/*			printf("no stim\n");*/
			return(tu_get_random(task, trial_blocks, reset_flag));

		/* MT stim */
		case 1:

/*			printf("MT stim\n"); */
			return(yl_get_MT_trial(task, trial_blocks, reset_flag));

		/* LIP stim */
		case 2:

/*			printf("LIP stim\n"); */
			return(yl_get_LIP_trial(task, trial_blocks, reset_flag));

		/* REPEAT */
		default:

			if(task->trialPs && task->trialPs_index >= 0 &&
				task->trialPs_index < task->trialPs_length)
				return(task->trialPs[task->trialPs_index]);
			else
				return(NULL);
	}
}

/* PUBLIC ROUTINE: dotsy_set_trial
**
**	Note that here we mostly access groups/lists/properties
**	 by INDEX, not NAME, so be careful if anything
**	 above changes.
*/
void dotsy_set_trial(_PRtrial trial)
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
	int scoh;					/* signed coh */
	int mt_ang, offset_ang;	
	int dots_ang;				/* dots angle - to be computed */
	int t1ang, t2ang;			/* target 1 (correct), target 2 (incorrect) */
	
	/* get the current trial's signed coh (scoh) */
	scoh = PL_L2PV(trial->list,0);
	
	/* get current task's Angle_MT */
	mt_ang = TIMV(trial->task, "Angle_MT");
		
/*	printf("scoh %d, dots_ang %d \n", scoh, dots_ang); */
	
	/* make working copies of the task object menu values */
	pl_list_set_v2w(tgm);
	pl_list_set_v2w(tdm);
	
	
	
	/*
	** conditionally set graphics/dots menus
	*/
	
	/* things independent of scoh sign *****/
	/* graphics menu */
	PL_CSET( PL_L2PWS(tgm, kX, 1),	TIMV(trial->task, "Pos_x_MT") );		/* dots:	1, 0. x = Pos_x_MT */
	PL_CSET( PL_L2PWS(tgm, kY, 1),	TIMV(trial->task, "Pos_y_MT") );		/* dots:	1, 1. y = Pos_y_MT */
	PL_CSET( PL_L2PWS(tgm, kT, 2),	TIMV(trial->task, "Pos_t_LIP") );		/* ctr: 	2, 5. t = Pos_t_LIP */
	PL_CSET( PL_L2PWS(tgm, kR, 3),	TIMV(trial->task, "Pos_r_LIP") );		/* trg1:	3, 4. r = Pos_r_LIP */
	PL_CSET( PL_L2PWS(tgm, kR, 4),	TIMV(trial->task, "Pos_r_LIP") );		/* trg2:	4, 4. r = Pos_r_LIP */
	
	/* things that depend on scoh sign *****/
	offset_ang = TIMV(trial->task, "Angle_offset");
	if (scoh > 0) {
		dots_ang = mt_ang + offset_ang;
		t1ang = 0; t2ang = 180;
	} else {
		dots_ang = (mt_ang + offset_ang + 180) % 360;
		t1ang = 180; t2ang = 0;
	}
	
	/* debugging stuff */
/*	printf("dots_ang %d, t1ang %d, t2ang %d\n", dots_ang, t1ang, t2ang); */
	
	/* dots menu */
	if(abs(scoh) == 1) { scoh = 0; }				/*  make sure to recode +/- 1 as 0% coh */
	PL_CSET( PL_L2PW(tdm, 2),	abs(scoh) );		/* 2. coherence */
	PL_CSET( PL_L2PW(tdm, 3),	dots_ang );		/* 3. direction of dots */
	
	/* graphics menu again */
	PL_CSET( PL_L2PWS(tgm, kT, 3),	t1ang);		/* trg1: 3, 5. t */
	PL_CSET( PL_L2PWS(tgm, kT, 4),	t2ang);		/* trg2: 4, 5. t */
	
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

}

/* PRIVATE ROUTINE: yl_get_MT_trials()
**
** generates randomized trials for MT stim
** trials where coh < Max_coh2stim OR stim == 0
** this is modelled after tu_get_random
**
** Arguments:
**  task: current task (see paradigm_rec)
**  num_blocks: # reps of trials to use to generate the full block
**  reset_flag: if TRUE, set pointer back to start of trialPs & makes new block
*/
_PRtrial yl_get_MT_trial(_PRtask task, int num_blocks, int reset_flag) 
{
	int last_score = task->pmf ? task->pmf->last_score : 0;
	
	/* if a block has been made */
	if(task->trialPs) {
		/* check previous score ... */
		if(last_score < 0) {
			/* if fixation break or no choice
			** - make sure we repeat this condition
			** swap trial with later trial 
			*/
			pr_task_swap_trialP(task);
		} else if((++(task->trialPs_index)) == task->trialPs_length) {  /* try to increment the pointer */
     		/* reached the end, clear pointer array
			** so next time through it gets re-randomized
			*/
			SAFE_FREE(task->trialPs);
			task->trialPs_length = 0;
		}
	} else {   /* if no block has been made */
		/* this is the first time, so toggle reset_flag */
		reset_flag = 1;
	}

	/* check to make block */
	if(reset_flag && !task->trialPs) {
		yl_task_make_trialP_MT(task, num_blocks);   /* 1. just make copies of trials for the block */
		pr_task_randomize_trialP(task);				  /* 2. randomize pointers in the block */
		
		yl_task_print(task, 0);
	}

	/* check if there is a list of pointers */
	if(!task->trialPs)
		return(NULL);

	/* return pointer to the current trial */
	return(task->trialPs[task->trialPs_index]);
	
}

/* PRIVATE ROUTINE: yl_task_make_trialP_MT
**
** make an array of pointers to MT stim trials w/i task
** grabs all trials that satisfy the following criteria:
**   1. smode = 0,
**    OR
**   2. abs(scoh) < maxcoh
** gets randomized in yl_get_MT_trial
**
**
** modified from pr_task_make_trialP_all
** 
*/
void yl_task_make_trialP_MT(_PRtask task, int num_blocks)
{
	int maxcoh = TIMV(task, "Max_coh2stim");
	int trial_count = 0; /* counts # trials in trials array that fulfill criterion */
	register int i,j,k;
	int total;

	/* check for task */
	if(!task)
		return;

	/* free the current pointers */
	SAFE_FREE(task->trialPs);

	/* check for trials, blocks */
 	if(!task->trials || num_blocks < 1)
		return;

	/* count the # of trials in 'trials' array that fulfill criterion */
	for(i=0;i<task->trials_length;i++) {
		/* smode == 0 || coh <= maxcoh */
		int scoh = PL_L2PV(task->trials[i]->list,0);
		int smode = PL_L2PV(task->trials[i]->list,1);
		if(smode == 0 || abs(scoh) <= maxcoh) trial_count++;
	}	

	/* Allocate the array of pointers */
	total = trial_count * num_blocks;
	task->trialPs = SAFE_ZALLOC(_PRtrial, total);

	/* conditinally fill the array with repeated blocks of pointers 
	** to trials, in order
	*/
	for(k=0,i=0;i<num_blocks;i++)
		for(j=0;j<task->trials_length;j++) {
			int scoh = PL_L2PV(task->trials[j]->list,0);
			int smode = PL_L2PV(task->trials[j]->list,1);
			if(smode == 0 || abs(scoh) <=maxcoh)
				task->trialPs[k++] = task->trials[j];
		}

	task->trialPs_length = total;
	task->trialPs_index  = 0;		/* start at the first trial */
}

/* PRIVATE ROUTINE: yl_get_LIP_trial
**
** picks the next trial for LIP stim block
** the hard work is in yl_task_make_trialP_LIP
** 
** based on tu_get_block - but just picks the next trial, even if monkey fxbr or nochoice
*/
_PRtrial yl_get_LIP_trial(_PRtask task, int num_blocks, int reset_flag)
{
	int last_score = task->pmf ? task->pmf->last_score : 0;

	/* Conditionally make a single block...
	** This will also reset the index to 0
	*/
	if(task->trialPs == NULL) {
		yl_task_make_trialP_LIP(task, num_blocks);    /* make a list of the trials */
		yl_task_print(task, 0);
		
		/* check that it worked */
		if(task->trialPs == NULL)
			return(NULL);

	/* Else if subj made a choice - increment the pointer and check for
	** end of the array
	*/
	} else if(				/* last_score >= 0 && */
		++(task->trialPs_index) >= task->trialPs_length) {

		/* end of the line ... check for reset */
		if(reset_flag > 0)
			/* reset pointer back to the beginning of the array */	
			task->trialPs_index = 0;
		else
			/* no reset, end of the line */
			return(NULL);
	}

	return(task->trialPs[task->trialPs_index]);
}

/* PRIVATE ROUTINE: yl_task_make_trialP_LIP
**
** generates a list of trial pointers with:
**   randomized stim trials
**   punctuated by a random nonstim trial every Stim_block_len trials
**    ie. there will be num_cohs*Stim_block_len*num_blocks stim trials
**                      num_cohs*               num_blocks nonstim trials
**
**   block of randomized Stim_block_len stim trial
**   followed by a single random non-stim trial
**   repeated until all trials are fulfilled
**    e.g., if Stim_block_len = 3
**    pointer array: SSSNSSSNSSSN
**      where S = stim, N = nonstim
**
**  strategy:
**     1. build separate lists of Stim and Nstim arrays
**     2. randomize each of those separately
**     3. concatenate them together
**
**	2011/05/19 yl: if Stim_block_len = 0 or NULLI, use ONLY stim trials
*/
void yl_task_make_trialP_LIP(_PRtask task, int num_blocks)
{
	register int i,j,k;
	int stim_blen = TIMV(task,"Stim_block_len");
	int num_cohs = task->trials_rows;	/* # of coherences */
	int stim_len, nstim_len, total;
	bool stim_only = stim_blen==0 || stim_blen == NULLI;
	_PRtrial *dummyStimPs, *dummyNstimPs, *tp1;
	
	/* check for task */
	if(!task) 	return;

	/* free the current pointers */
	SAFE_FREE(task->trialPs);

	/* check for trials, blocks */
 	if(!task->trials || num_blocks < 1) return;

	/* allocate space for the two dummy pointer arrays */
	if(stim_only)
	{
		stim_len = num_cohs * num_blocks;
		nstim_len = 0;
	} else {
		stim_len =  num_cohs * num_blocks * stim_blen;
		nstim_len = num_cohs * num_blocks;
	}
	dummyStimPs = SAFE_ZALLOC(_PRtrial, stim_len);
	if(!stim_only)
		dummyNstimPs = SAFE_ZALLOC(_PRtrial, nstim_len);
	
	/* 
	** ==== fill the two arrays in order ====
	** 1. the nonstim trials - these would be in the 1st column 
	*/
	if(!stim_only)
	{
		for(k=0,i=0; i<num_blocks; i++) {
			/* for each coh */
			for(j=0; j<num_cohs; j++)
				/* grab from the 1st column */
			dummyNstimPs[k++] = task->trials[j*task->trials_columns+0];
		}
	}
		
	/* 
	** 2. the stim trials - these would be in the 2nd column 
	*/
	k=0;
	while(k<stim_len) {
		/* for each coh */
		for(j=0; j<num_cohs; j++)
			/* grab from the 2nd column */
			dummyStimPs[k++] = task->trials[j*task->trials_columns+1];
	} 
	
	/* 
	** ==== randomize w/i each set ====
	** I do this by swapping the dummy trial pointers w/ the task trial pointers
	*/
	/* 1. randomize for the nonstim trials */
	if(!stim_only)
	{	task->trialPs = dummyNstimPs;
		task->trialPs_length = nstim_len;
		pr_task_randomize_trialP(task);
		dummyNstimPs = task->trialPs; 
	}
	
	/* 2.randomize for stim trials */
	task->trialPs = dummyStimPs;
	task->trialPs_length = stim_len;
	pr_task_randomize_trialP(task);
	dummyStimPs = task->trialPs;		/* for stim_only blocks, this would be done */
	
/*	SAFE_FREE(task->trialPs);*/   /* <= DO I NEED TO DO THIS? I THINK I DELETE EVERYTHING */
	
	/* now we want to put those two arrays together */
	/* first - allocate space */
	total = nstim_len + stim_len;
	
	if(!stim_only)
	{
		tp1 = task->trialPs = SAFE_ZALLOC(_PRtrial, total);
	
		/* now - fill the array */
		j=0;
		for(i=0; i<stim_len; i++) {
			/* grab from the stim block */
			*tp1++ = dummyStimPs[i];
		
			/* every stim_blen-th trial ...
			** grab from the nstim block
			*/
			if((i+1)%stim_blen == 0)
				*tp1++ = dummyNstimPs[j++];
		}
	}
	/* set the trialP index */	
	task->trialPs_length = total;
	task->trialPs_index  = 0;
	
	/* don't forget to free the memory! */
	if(!stim_only)
	{	SAFE_FREE(dummyStimPs);
		SAFE_FREE(dummyNstimPs);
	}
	/* for stim-only trials, dummyStimPs *is* task->trialPs */
	
}

/* PRIVATE ROUTINE: yl_task_print
**
** print useful info about a task
** including # of times each trial type is repeated
**
** Parameters:
**  print_all: if TRUE, prints all the trial IDs in order
*/
void yl_task_print(_PRtask task, int print_all)
{
	register int i, num_trials, num_cols;
	
	if(!task) { printf(" ** NULL TASK ** \n"); return; }
	
	/* task info */
	printf("\n\n***** yl_task_print *****\n");
	printf("  ** TASK (%d,%s): %d (%dr x %dc; i=%d) trials; %d (i=%d) trialPs.\n",
		task->id, task->type, task->trials_length, 
		task->trials_rows, task->trials_columns, 
		task->trials_index, task->trialPs_length,
		task->trialPs_index);

	/* loop through the types of trials */
	for(i=0; i<task->trials_length;i++) {
		yl_trial_print(task->trials[i]);
	}
	
	if(TIMV(task, "Stim_flag")==2) num_cols =1+TIMV(task,"Stim_block_len");
	else num_cols = 10;
	
	/* conditionally print all trials */
	if(print_all) {
		yl_trialPs_print(task->trialPs, task->trialPs_length, num_cols);
	}
	
	printf("*************************\n\n");
}

/* PRIVATE ROUTINE: yl_trialPs_print
**
** print all the id's of array of trials in order
**
** Parameters:
**	trialPs = array of trials
**	len = length of array
**	num_cols = # of columns for print out
*/
void yl_trialPs_print(_PRtrial *trialPs, int len, int num_cols)
{	register int i;
	printf("\n");
	for(i=0;i<len;i++) {
		printf(" %d ", trialPs[i]->id);
		if((i+1)% num_cols == 0) {
			printf("\n");
		}
	}
	printf("\n");
}


/* PRIVATE ROUTINE: yl_trial_print
**
** prints info that trial
** includes count of that trial type in that block
** mod from pr_trial_print
**
*/
void yl_trial_print(_PRtrial trial)
{
	register int i;
	int num_trials = 0;	/* # reps for that trial */
		
	if(!trial) {
		printf("		 NULL TRIAL\n"); return;
	}

	/* loop through trialPs to count num_trials for this trial*/
	for(i=0; i<trial->task->trialPs_length; i++) {
		if(trial->task->trialPs[i] == trial) num_trials++;
	}
	
	printf("     TR (%2d) x%d: ", trial->id, num_trials);
	
	/* loop through the properties */
	if(trial->list && trial->list->properties_length) {
		/* print the property values */
		for(i=0;i<trial->list->properties_length;i++)
			printf("%s=%3d ", 
				trial->list->properties[i]->name,
				(int) (trial->list->properties[i]->values[0]));

		printf("\n");
	} else {
		printf("No properties\n");
	}
}


#/** PhEDIT attribute block
#-11:16777215
#0:12367:default:-3:-3:0
#12367:13512:monospace9:0:-1:0
#13512:15641:default:-3:-3:0
#15641:15666:monospace9:0:-1:0
#15666:22134:default:-3:-3:0
#**  PhEDIT attribute block ends (-0000234)**/
