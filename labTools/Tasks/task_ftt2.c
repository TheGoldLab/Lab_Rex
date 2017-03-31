/*
** task_ftt2.c
**
** task "fixation-target-target".c, #2 ... 
**	 	standard fixation + 2-target tasks
**			(e.g., overlap and memory saccades)
*/

#include "../toys.h"
#include "../task.h"
#include "../task_utilities.h"

/* PRIVATE MACROS/CONSTANTS */
#define NUM_GR_MENUS 3
#define TI 				"setup"
#define TIMI(ta,n,i)	pl_list_get_vi(ta->task_menus->lists[0], (n), (i))
#define MAX_BLOCKS	5

/* PRIVATE DATA STRUCTURES */

/* PRIVATE ROUTINES */
void 		ftt2_make_trials	(_PRtask);
_PRtrial	ftt2_get_trial		(_PRtask, int);
void 		ftt2_set_trial		(_PRtrial);

/* PRIVATE VARIABLES */

/* 
** TASK: FTT2	("Fixation-Target-Target")
*/
struct _TAtask_struct gl_ftt2_task[] = {

	/* name */
	"ftt2",

	/* 
	** Menus 
	*/
	{
		/* 
		** Task info
		*/
		{ TI, MAX_BLOCKS },
			{ "Min_block_length",	0, 1.0 },
			{ "Mean_block_length",	0, 1.0 },
			{ "Max_block_length",	0, 1.0 }, 
			{ "T1prob",					0, 1.0 }, 
			{ "T2prob",					0, 1.0 }, 
			{ "Diminish_utility",	0, 1.0 }, 
        { NULL },

		/*
		**	3 'standard' graphics objects:
		**		0 - fixation
		**		1 - target #1
		**		2 - target #2
		*/
		{ "graphics",		NUM_GR_MENUS },
			{ DX_X,			0, 0.1 }, 
			{ DX_Y,			0, 0.1 }, 
			{ DX_DIAM,		0, 0.1 },
			{ DX_CLUT,		0, 1.0 },
			{ NULL },

		/*
		**	2 'beep' sound objects:
		**		0 - at fp off
		**		1 - occasionally during fixation
		*/
		{ "beeps",			2 },
			{DX_FREQ,		0, 1.0 },
			{DX_DUR,			0, 1.0 },
			{DX_GAIN,		0, 1.0 },
			{ NULL },

		/* END OF MENUS */
		{ NULL }
	},

	/* 
	** Snow-dots objects, by name
	*/
	{  { DX_TARGET,	3 }, 
		{ DX_BEEP, 		2 },
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
	&ftt2_make_trials,
	&ftt2_get_trial,
	&ftt2_set_trial,
};

/* PUBLIC ROUTINE: ftt2_make_trials
**
*/
void ftt2_make_trials(_PRtask task)
{
   int block_is[MAX_BLOCKS],num_blocks,i,j,k;

	/* first check for valid blocks */
	for(i=0,num_blocks=0;i<MAX_BLOCKS;i++)
		if(TIMI(task,"T1prob",i) >= 0 &&
			TIMI(task,"Min_block_length",i) > 0)
			block_is[num_blocks++] = i;

	/* now (conditionally) make emptry matrix of trials */
	if(!num_blocks)
		pr_task_clear(task);
	else
		pr_task_add_trials2D(task, num_blocks, 2);

	/* now add list of 2 probabilities per block */
	for(k=0,i=0;i<num_blocks;i++) {

		/* T1 prob */
		task->trials[k]->id = task->id*100+k;
		task->trials[k++]->list = 
			pl_list_initV("2dtc", 0, 1, 
				"block",  i, 1.0,
				"T1prob", TIMI(task,"T1prob",block_is[i]), 0.1,
				NULL);

		/* T2 prob */
		task->trials[k]->id = task->id*100+k;
		task->trials[k++]->list = 
			pl_list_initV("2dtc", 0, 1, 
				"block",  i, 1.0,
				"T2prob", TIMI(task,"T2prob",block_is[i]), 0.1,
				NULL);
	}

	/* Set up user data: block counter */
	SAFE_FREE(task->user_data);
	task->user_data = SAFE_ZALLOC(int, 1);
	*((int *) (task->user_data)) = 0;
}

/* PUBLIC ROUTINE: ftt2_get_trial
**
**	Really picking block, then always returning trial 0
**
** Returns:
**		_PRtrial if trials remain
**		NULL otherwise
*/
_PRtrial ftt2_get_trial(_PRtask task, int reset_flag)
{
	int this_block, min_length, mean_length, max_length, 
			*block_length = (int *) (task->user_data);

	/* If no blocks, do nothing */
	if(task->trials_length == 0)
		return(NULL);

	/* Check if we need to make a new block */
	if(task->trialPs && *block_length) {

		/* decrement counter if correct/null choice */
		if(task->rec->pmf->last_score >= 0)
			(*block_length)--;

	} else {
	
		/* none exists .. make it */	
		if(!task->trialPs) {
			this_block = TOY_RAND(task->trials_rows);
			pr_task_make_trialP_row(task, this_block, 1);

		} else {
			/* make sure that we don't repeat the same block */
			this_block = TOY_RAND(task->trials_rows-1);
			if(this_block >= pl_list_get_v(task->trialPs[0]->list, "block"))
				this_block++;

			/* make trial pointers for the new block */
			pr_task_make_trialP_row(task, this_block, 1);
		}

		/* start counter */
		min_length  = TOY_MAX(0, TIMI(task, "Min_block_length",  this_block));
		max_length  = TOY_MAX(min_length, TIMI(task, "Max_block_length",  
								this_block));
		mean_length = TOY_MIN(max_length, TOY_MAX(min_length, 
								TIMI(task, "Mean_block_length", this_block)));

		*block_length = min_length==max_length?min_length:
				toy_exp2(min_length, max_length, mean_length-min_length);
	}

	/* Always return the first trial in the block */
	task->trialPs_index = 0;
	return(task->trialPs[task->trialPs_index]);
}

/* PUBLIC ROUTINE: ftt2_set_trial
**
*/
void ftt2_set_trial(_PRtrial trial)
{
	int	  this_block = pl_list_get_v(trial->list, "block");
	_PLlist tgm = trial->task->task_menus->lists[1],
			  tbm = trial->task->task_menus->lists[2];

	printf("setting trial for block %d, %d trials remain\n", 
			this_block, *((int *) (trial->task->user_data)));

	/* Set graphic object properties .. use real values (not working copies) for
	* 		3 indexed objects: 		0=fp, 1=t1, 2=t2
	* 		4 indexed properties:  1=x, 2=y, 3=diam, 4=clut
	*/
	dx_set_by_indexIV(DXF_NODRAW, 0, -1, NULL,
				1, PL_L2PVS(tgm, 0, 0), PL_L2PVS(tgm, 0, 1), PL_L2PVS(tgm, 0, 2),
				2, PL_L2PVS(tgm, 1, 0), PL_L2PVS(tgm, 1, 1), PL_L2PVS(tgm, 1, 2),
				3, PL_L2PVS(tgm, 2, 0), PL_L2PVS(tgm, 2, 1), PL_L2PVS(tgm, 2, 2),
				4, PL_L2PVS(tgm, 3, 0), PL_L2PVS(tgm, 3, 1), PL_L2PVS(tgm, 3, 2),
				ENDI);

	/* Set beep object properties .. use real values (not working copies) for
	* 		2 indexed objects: 	beep0, beep1
	* 		3 indexed properties:  1=frequency, 2=duration, 3=gain
	*/
	dx_set_by_indexIV(DXF_NODRAW, 1, -1, NULL,
				0, PL_L2PVS(tbm, 0, 0), PL_L2PVS(tbm, 0, 1),
				1, PL_L2PVS(tbm, 1, 0), PL_L2PVS(tbm, 1, 1),
				2, PL_L2PVS(tbm, 2, 0), PL_L2PVS(tbm, 2, 1),
				ENDI);	

	/* Always drop codes
	** First fp/target propertes
	*/
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

	/* Now T1/T2 probabilities + diminish_utility value (block condition) */
	ec_send_code_tagged(I_PRTP1, 	 7000 + TIMI(trial->task, "T1prob", this_block));
	ec_send_code_tagged(I_PRTP2, 	 7000 + TIMI(trial->task, "T2prob", this_block));
	ec_send_code_tagged(I_REWCONTCD, 7000 + TIMI(trial->task, "Diminish_utility", this_block));
}
