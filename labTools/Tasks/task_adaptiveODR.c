/*
** task_adaptiveODR.c
**
** task "fixation-target-target".c ... "
**	 	standard fixation + 2-target tasks
**			(e.g., overlap and memory saccades)
*/

#include "../toys.h"
#include "../task.h"
#include "../task_utilities.h"

/* PRIVATE MACROS/CONSTANTS */
#define NUM_OBJECTS  3
#define TI           "setup"
#define TIMV(ta,n) 	pl_list_get_v((ta)->task_menus->lists[0],(n))
#define WC_INIT		-1

/* PRIVATE DATA STRUCTURES */

/* PRIVATE ROUTINES */
void        adODR_make_trials	(_PRtask);
_PRtrial    adODR_get_trial   (_PRtask, int);
void        adODR_set_trial   (_PRtrial);

/* PRIVATE VARIABLES */

/* 
** TASK: FT
**		("Fixation-Target")
*/
struct _TAtask_struct gl_adODR_task[] = {

	/* name */
	"adaptODR",

	/* 
	** Menus 
	*/
	{
		/* 
		** Task info
		*/
		{ TI, 				1 },
         { "T1_ID_Hazard",    0, 0.1 }, 
         { "T1_ED_Hazard",    0, 0.1 }, 
         { "T1_sigma",        0, 0.1 }, 
         { "T2_ID_Hazard",    0, 0.1 }, 
         { "T2_ED_Hazard",    0, 0.1 }, 
         { "T2_sigma",        0, 0.1 }, 
         { "Start_T1_T2",     0, 0.1 }, 
			{ "Fixation_task",	0, 0.1 },
         { "Drop_codes",		1, 1.0 },
      { NULL },

		/*
		**	5 'standard' graphics objects:
		**		0 - fixation
		**		1 - target #1
      **    2 - target #2
		**		3 - target #1 generative mean
      **    4 - target #2 generative mean
		*/
		{ "graphics",		NUM_OBJECTS+2},
         TU_XYRT_LIST,
      	{ DX_DIAM,			0, 0.1 },
         { DX_CLUT,			0, 1.0 },
      { NULL },

		/* END OF MENUS */
		{ NULL }
	},

	/* 
	** Graphics objects, by name... add 2 for "mean"
	**	targets
	*/
	{  { DX_TARGET,	NUM_OBJECTS+2 }, 
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
	&adODR_make_trials,
	&adODR_get_trial,
	&adODR_set_trial,
};

/* PUBLIC ROUTINE: adODR_make_trials
**
*/
void adODR_make_trials(_PRtask task)
{
   int i, vals[2] = {0,1};

	/*
	** Add two trial types, one per target
	*/
	tu_make_trials1D(task, "target", 2, vals, 0.1);

	/* set working copies to a flag that indicates
	** to set trial that the target theta mean needs
	**	to be initialized
	*/
	for(i=0;i<2;i++)
		PL_L2PW(task->trials[i]->list,0) = WC_INIT;
}

/* PUBLIC ROUTINE: adODR_get_trial
**
** Use given Hazard rates to choose next trial
**
** Returns:
**		_PRtrial if trials remain
**		NULL otherwise
*/
_PRtrial adODR_get_trial(_PRtask task, int reset_flag)
{
	valtype hazard;
    
	/* Check if trialP array exists...
	* if not, make it with just the two trials and randomly pick one
	*/
	if(!task->trialPs) {

		/* make trials if they don't exist */
		pr_task_make_trialP_all(task, 1);
       
		/* check Task menu for starting target
		* 	1=T1, 2=T2, otherwise randomize
		*/
		switch(TIMV(task, "Start_T1_T2")) {
			case 1:
				task->trialPs_index = 0;
				break;
			case 2:
				task->trialPs_index = 1;
				break;
			default:	
				task->trialPs_index = TOY_RAND(2);
				break;
		}
	}
    
	/* Test current target-specific hazard */
	hazard = task->trialPs_index ?
		TIMV(task, "T2_ED_Hazard") : TIMV(task, "T1_ED_Hazard");
	if(hazard>0 && TOY_RCMP(hazard))
		task->trialPs_index = task->trialPs_index == 0 ? 1 : 0;
        
    /* return correct trial */
    return(task->trialPs[task->trialPs_index]);
}

/* PUBLIC ROUTINE: adODR_set_trial
**
*/
void adODR_set_trial(_PRtrial trial)
{   
   _PRtask  task = trial->task;
	_PLlist  tim  = trial->task->task_menus->lists[0],
            tgm  = trial->task->task_menus->lists[1];
   valtype  hazard, sigma;
   register int i;
	int		xs[2], ys[2];
	static 	int mus[2];
   
   /* array of indices of target objects (fp, t1, t2) in
   ** both the graphics menu and dXtargets objects list
   */
   int ti[] = {0,1,2}, ti2[]={3,4};

	/* first copy real values into working copies in graphics menu */
	pl_list_set_v2w(tgm);

   /* Need to access and udpdate both trials */
   for(i=0;i<task->trials_length;i++) {

      /* get appropriate variables from menu */
      if(i==0) {
         hazard = TIMV(task, "T1_ID_Hazard");
         sigma  = TIMV(task, "T1_sigma");
      } else {
         hazard = TIMV(task, "T2_ID_Hazard");
         sigma  = TIMV(task, "T2_sigma");
      }

      /* Possibly get new generative mean and store in 
		** a local, static variable
		*/
      if((PL_L2PW(task->trials[i]->list,0)==WC_INIT) ||
         TOY_RCMP(hazard)) {
        	mus[i] = TOY_RAND(360);
      	PL_L2PW(task->trials[i]->list,0)=mus[i];
      }
      
		/* Compute x,y location of mean target */
		xs[i] = TOY_RT_TO_X(0, pl_list_get_vi(tgm,DX_R,i+1), mus[i]);
		ys[i] = TOY_RT_TO_Y(0, pl_list_get_vi(tgm,DX_R,i+1), mus[i]);

      /* Compute actual target location from generative mean
      ** and variance and store it as theta (kT) of the appropriate
      ** appropriate target (the 1st or 2nd object)
      */
      PL_L2PWS(tgm,kT,i+1) = 
			toy_randCircularNormal(mus[i], sigma/10.0);

		printf("Target %d, mean=%.2d, sigma=%.2f, theta=%.2f, x=%d, y=%d\n",
			i+1, mus[i], (double) (sigma)/10.0, (double) PL_L2PWS(tgm,kT,i+1),
			xs[i], ys[i]);
	}
  
	/* compute xys from the working copies of values in the graphics menu */
	tu_compute_xyrt(tgm);

	/* make working copy Fixation Point CLUT equal to CLUT
	** of acive target
	*/
	pl_list_set_wi(tgm, DX_CLUT, 0, 
		pl_list_get_vi(tgm, DX_CLUT, PL_L2PV(trial->list,0)+1));

	/* Copy "working copy" values of key target parameters
	**		to graphics objects and automatically 
	**		send rSet command ...
	**
	**	First dXtargets, all at once ...
	*/
	dx_setl_by_nameIV(0,
		tgm, 'w',   NUM_OBJECTS, ti, 
		"dXtarget", NUM_OBJECTS, ti,
		DX_X,       NULL, 
		DX_Y,       NULL,
		DX_DIAM,    NULL,
		DX_CLUT,    NULL,
		NULL);

	/* update hidden generative mean targets */
	dx_set_by_nameIV(0, DX_TARGET, 2, ti2,
		DX_X, xs[0], xs[1], 
		DX_Y, ys[0], ys[1], 
		NULL);

	/* Conditionally drop codes ... spm can figure out from the cluts
   * which one is the active target
   */
	if(TIMV(trial->task, "Drop_codes") != 0) {

      /* automatically drop codes related to three target objects */
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
         
      /* DROP trial-specific codes */
#define BASE 7000
		ec_send_code_tagged(I_T1IDH,   BASE + TIMV(task, "T1_ID_Hazard"));
		ec_send_code_tagged(I_T1EDH,   BASE + TIMV(task, "T1_ED_Hazard"));
		ec_send_code_tagged(I_T1SIGMA, BASE + TIMV(task, "T1_sigma"));
		ec_send_code_tagged(I_T1MEANT, BASE + mus[0]);
		ec_send_code_tagged(I_T2IDH,   BASE + TIMV(task, "T2_ID_Hazard"));
		ec_send_code_tagged(I_T2EDH,   BASE + TIMV(task, "T2_ED_Hazard"));
		ec_send_code_tagged(I_T2SIGMA, BASE + TIMV(task, "T2_sigma"));
		ec_send_code_tagged(I_T2MEANT, BASE + mus[1]);
	}
}
