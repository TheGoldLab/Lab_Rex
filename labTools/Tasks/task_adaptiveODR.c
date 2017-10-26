/*
** task_adaptiveODR.c
**
** task "fixation-target-target".c ... 
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
	"adODR",

	/* 
	** Menus 
	*/
	{
		/* 
		** Task info
		*/
		{ TI, 				1 },
         { "T1_ID_Hazard"     0, 0.1 }, 
         { "T1_ED_Hazard"     0, 0.1 }, 
         { "T1_kappa"         0, 0.1 }, 
         { "T2_ID_Hazard"     0, 0.1 }, 
         { "T2_ED_Hazard"     0, 0.1 }, 
         { "T2_kappa"         0, 0.1 }, 
         { "Start_T1_T2"      0, 0.1 }, 
         { "Drop_codes",      1, 1.0 },
      { NULL },

		/*
		**	3 'standard' graphics objects:
		**		0 - fixation
		**		1 - target #1
      **    2 - target #2
		*/
		{ "graphics",		NUM_OBJECTS },
         TU_XYRT_LIST,
      	{ DX_DIAM,			0, 0.1 },
         { DX_CLUT,			0, 1.0 },
         { DX_SHAPE,			0, 1.0 },
      { NULL },

		/* END OF MENUS */
		{ NULL }
	},

	/* 
	** Graphics objects, by name
	*/
	{  { DX_TARGET,	NUM_OBJECTS }, 
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
   int vals[2] = {0,1};

	/*
	** Just add two trial types, one per target
	*/
	tu_make_trials1D(task, "target", 2, vals, 0.1);
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
valtype val, hazard;
    
    /* Check if trialP array exists...
     * if not, make it with just the two trials and randomly pick one
     */
    if(!task->trialPs) {
       val = TIMV(task,"Start_T1_T2");
       
       /* make trials if they don't exist */
       pr_task_make_trialP_all(task, 1);
       
       /* check Task menu for starting target:
       * 1=T1, 2=T2, otherwise randomize
       */
      if(val==1 || val==2)
         task->trialPs_index = val-1;
      else
         task->trialPs_index = TOY_RAND(2);
    }
    
    /* Get & test current target-specific hazard */
    hazard = task->trialPs_index ? TIMV(task, "T2_ED_Hazard") : TIMV(task, "T1_ED_Hazard");
    if(hazard>0 && TOY_RCMP(hazard))
        task->trialPs_index = task->trialPs_index == 0 ? 1 : 0;
        
    /* return correct trial */
    return(task->trialPs(task->trialPs_index));
}

/* PUBLIC ROUTINE: adODR_set_trial
**
*/
void adODR_set_trial(_PRtrial trial)
{   
   _PRtask  task = trial->task;
	_PLlist  tim  = trial->task->task_menus->lists[0],
            tgm  = trial->task->task_menus->lists[1];
   valtype  hazard;
   double   kappa;
   register int i;
   
   /* array of indices of target objects (fp, t1, t2) in
   ** both the graphics menu and dXtargets objects list
   */
   int ti[] = {0,1,2};

   /* Need to access and udpdate both trials */
   for(i=0;i<task->trials_length;i++) {

      /* get appropriate variables from menu */
      if(i==0) {
         hazard = TIMV(task, "T1_ID_Hazard");
         kappa  = TIMV(task, "T1_ID_kappa")/10.0;
      } else {
         hazard = TIMV(task, "T2_ID_Hazard");
         kappa  = TIMV(task, "T2_ID_kappa")/10.0;
      }

      /* Store current generative in current trial */
      if((PL_L2PV(task->trials[i]->list,0)==NULLI) ||
         TOY_RCMP(hazard)) {
         
         /* get new generative mean */
         PL_L2PW(task->trials[i]->list,0) = TOY_RAND(360);
      }
      
      /* Compute actual target location from generative mean
      ** and variance and store it as theta (the "kT"-th property of 
      ** the graphics menu) of the appropriate target (the 1st or 2nd
      ** object)
      */
      PL_L2PVS(tgm,kT,i+1) = toy_vonMisesRand(
         PL_L2PW(task->trials[i]->list,0), kappa);
	}
         
	/* compute xys from the working copies of values in the graphics menu */
	pl_list_set_v2w(tgm);
	tu_compute_xyrt(tgm);

	/* Copy "working copy" values of key target parameters
	**		to graphics objects and automatically 
	**		send rSet command ...
	**
	**	First dXtargets, all at once ...
	*/
	dx_setl_by_nameIV(0, 1, 
		tgm, 'w',   NUM_OBJECTS, ti, 
		"dXtarget", NUM_OBJECTS, ti,
		DX_X,       NULL, 
		DX_Y,       NULL,
		DX_DIAM,    NULL,
		DX_CLUT,    NULL,
      DX_SHAPE,   NULL,
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
		ec_send_code_tagged(I_T1KAPPA, BASE + TIMV(task, "T1_kappa"));
		ec_send_code_tagged(I_T2IDH,   BASE + TIMV(task, "T2_ID_Hazard"));
		ec_send_code_tagged(I_T2EDH,   BASE + TIMV(task, "T2_ED_Hazard"));
		ec_send_code_tagged(I_T2KAPPA, BASE + TIMV(task, "T2_kappa"));
	}
}
