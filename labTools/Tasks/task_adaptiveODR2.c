/*
** task_adaptiveODR2.c
**
** task "fixation-target-target".c ... "
**	 	standard fixation + 2-target tasks
**			(e.g., overlap and memory saccades)
 *
 * Modified version of the adaptiveODR task to be more like the
 * Triangles task from Glaze et al 2015
*/

#include "../toys.h"
#include "../task.h"
#include "../task_utilities.h"

/* PRIVATE MACROS/CONSTANTS */
#define NUM_TRIALS  	2
#define NUM_OBJECTS  5
#define TI           "setup"
#define TIMV(ta,n) 	pl_list_get_v((ta)->task_menus->lists[0],(n))
#define WC_INIT		-1
#define EC_BASE		7000
#define ADIST(a1,a2)	((abs((a1)-(a2)) <= 180) ? abs((a1)-(a2)) : (360 - abs((a1)-(a2))))

/* PRIVATE DATA STRUCTURES */

/* PRIVATE ROUTINES */
void        adODR2_make_trials	(_PRtask);
_PRtrial    adODR2_get_trial   (_PRtask, int);
void        adODR2_set_trial   (_PRtrial);

/* PRIVATE VARIABLES */
int gl_mus[2];

/* 
** TASK: FT
**		("Fixation-Target")
*/
struct _TAtask_struct gl_adODR2_task[] = {

	/* name */
	"adaptODR2",

	/* 
	** Menus 
	*/
	{
		/* 
		** Task info
		*/
      { TI, 				1 },
         { "Hazard",          0, 0.1},
         { "Sample_std",      0, 0.1},
         { "Sample_cutoff",   1, 1.0}, 
			{ "Fixation_task",	0, 0.1 },
         { "Drop_codes",		1, 1.0 },
      { NULL },

 		/*
		**	5 'standard' graphics objects:
		**		0 - fixation
		**		1 - target #1
      **    2 - target #2
		**		3 - sample
      **    4 - annulus
		*/
		{ "graphics",		NUM_OBJECTS},    
         TU_XYRT_LIST,
      	{ DX_DIAM,			0, 0.1 },
         { DX_CLUT,			0, 1.0 },
         { DX_SHAPE,			0, 1.0 },
      { NULL },

		/* END OF MENUS */
		{ NULL }
	},

	/* 
	** Graphics objects, by name... add 2 for "mean"
	**	targets
	*/
	{  { DX_TARGET,	NUM_OBJECTS+3 },   /*KAS 6/15 added another object for ring*/
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
	&adODR2_make_trials,
	&adODR2_get_trial,
	&adODR2_set_trial,
};

/* PUBLIC ROUTINE: adODR2_make_trials
**
*/
void adODR2_make_trials(_PRtask task)
{
   int i, vals[2] = {0,1};

	printf("Starting Make Trials\n");
	/*
	** Add two trial types, one per target
	*/
	tu_make_trials1D(task, "target", 2, vals, 0.1);

	/* set working copies to a flag that indicates
	** to set trial that the target theta mean needs
	**	to be initialized
	*/
	for(i=0;i<NUM_TRIALS;i++)
{
		gl_mus[i] = WC_INIT;
		}
			printf("Finish Make Trials\n");
}

/* PUBLIC ROUTINE: adODR2_get_trial
**
** Use given Hazard rates to choose next trial
**
** Returns:
**		_PRtrial if trials remain
**		NULL otherwise
*/
_PRtrial adODR2_get_trial(_PRtask task, int reset_flag)
{
	valtype hazard;
	
    
	printf("Starting Get Trials\n");
    
	/* Check if trialP array exists...
	* if not, make it with just the two trials and randomly pick one
	*/
	if(!task->trialPs) {

		/* make trials if they don't exist */
		pr_task_make_trialP_all(task, 1);
       
      /* Randomly pick first target */
      task->trialPs_index = TOY_RAND(2);      
	}
   
	/* Use current hazard to determine if we are switching targets */
	hazard = TIMV(task, "Hazard");
	if(hazard>0 && TOY_RCMP(hazard)) 
{
			printf("Switching Target\n");

		task->trialPs_index = task->trialPs_index==0 ? 1 : 0;

} else {
			printf("NOT Switching Target\n");

}

	/* return correct trial */
	printf("Finishing Get Trials\n");
	return(task->trialPs[task->trialPs_index]);
		
}

/* PUBLIC ROUTINE: adODR2_set_trial
**
** Test Kyra 08/13/18
*/
void adODR2_set_trial(_PRtrial trial)
{      
	printf("Starting set Trials\n");
   _PRtask  task = trial->task;
	_PLlist  tim  = trial->task->task_menus->lists[0],
            tgm  = trial->task->task_menus->lists[1];
      
   /* array of indices of target objects (fp, t1, t2) in
   ** both the graphics menu and dXtargets objects list
   */
   int ti[] = {0,1,2,3,4}; 
   int t1_mean;
   int t2_mean;
   int mean_diff;
   int theta_mean;
   int theta_std;
   int sample_mean;

   int sample_t1diff;
   int sample_t2diff;


   /* first copy real values into working copies in graphics menu */
	pl_list_set_v2w(tgm);

	printf("Computing Sample location\n");
   /* Compute sample location */
   t1_mean = pl_list_get_vi(tgm,DX_T,1);
   t2_mean = pl_list_get_vi(tgm,DX_T,2);
   mean_diff = abs(t1_mean - t2_mean);
   if(mean_diff > 180)
     mean_diff = 360 - mean_diff;
   
   theta_mean = pl_list_get_vi(tgm,DX_T,task->trialPs_index+1);
   theta_std  = TIMV(task, "Sample_std")/10.0;

/*Previously sample mean was calculated here*/

   
   if(TIMV(task, "Sample_cutoff") > 0) {

      do {
         /* Loop until found sample between two targets */
   sample_mean = toy_randCircularNormal(theta_mean, theta_std);
         sample_t1diff = abs(t1_mean - sample_mean);
         if(sample_t1diff > 180)
            sample_t1diff = 360 - sample_t1diff;
         sample_t2diff = abs(t2_mean - sample_mean);
         if(sample_t2diff > 180)
            sample_t2diff = 360 - sample_t2diff;
          

      
      } while(sample_t1diff>mean_diff || sample_t2diff>mean_diff);
   }
      
      	printf("Setting Sample Angle\n");
   /* set the sample theta */
   PL_L2PWS(tgm,kT,3) = sample_mean;
   
   /* Compute target and sample locations */   
   tu_compute_xyrt(tgm);
   
	/* Copy "working copy" values of key target parameters
	**		to graphics objects and automatically 
	**		send rSet command ...
	**
	**	First dXtargets, all at once ...
	*/
	dx_setl_by_nameIV(0,
		tgm, 'w',   NUM_OBJECTS, ti,   /*added +1 to Num_objects KAS 6/15*/
		"dXtarget", NUM_OBJECTS, ti,
		DX_X,       NULL, 
		DX_Y,       NULL,
		DX_DIAM,    NULL,
		DX_CLUT,    NULL,
		DX_SHAPE,   NULL,
		NULL);

	/* Conditionally drop codes ... spm can figure out from the cluts
   ** which one is the active target
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
			DX_X,		3, 	I_TRG3XCD,
			DX_Y,		3, 	I_TRG3YCD,
			DX_CLUT,	3, 	I_TRG3LCD,
			NULL);
         
      /* DROP trial-specific codes */
		ec_send_code_tagged(I_T1IDH,   EC_BASE + TIMV(task, "Hazard"));
		ec_send_code_tagged(I_T1SIGMA, EC_BASE + TIMV(task, "Sample_std"));
			printf("Finish Set Trials\n");
	}
}
