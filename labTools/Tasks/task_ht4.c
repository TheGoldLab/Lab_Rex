/*
 ** task_ht4.c
 **
 ** task "hazard-4 target"
 **		control change-point probability (hazard rate)
 **		for two pairs of visual targets
 */

#include "../toys.h"
#include "../task.h"
#include "../task_utilities.h"

/* PRIVATE MACROS/CONSTANTS */
#define NUM_TARGETS  2
#define MAX_HAZARDS  5
#define TI           "setup"
#define TIMV(ta,n) 	pl_list_get_v((ta)->task_menus->lists[0],(n))

/* PRIVATE DATA STRUCTURES */

/* PRIVATE ROUTINES */
void        ht4_make_trials   (_PRtask);
_PRtrial    ht4_get_trial     (_PRtask, int);
void        ht4_set_trial     (_PRtrial);

/* PRIVATE VARIABLES */

/*
 ** TASK: HT4
 **		("Hazard + 4 Targets")
 */
struct _TAtask_struct gl_ht4_task[] = {
   
   /* name */
   "ht4",
   
   /*
    ** Menus
    */
   {
      /*
       ** Task info
       */
      { TI, 					1 },
      { "ID_H1",			0, 0.1 },
      { "ID_H2",			0, 0.1 },
      { "ID_H3",			0, 0.1 },
      { "ID_H4",			0, 0.1 },
      { "ID_H5",  		0, 0.1 },
      { "ED_H",  			0, 0.1 },
      { "Y_max",			1, 0.1 },
      { "NC_repeat",		1, 0.1 },
      { "Drop_codes",	1, 1.0 },
      { NULL },
              
		/*
		**	3 graphics objects:
		**		0 - fixation
		**		1 - T1
		**    2 - TR2
		*/
      { "graphics",	NUM_TARGETS+1},
      { DX_X,			0, 0.1 },
      { DX_Y,			0, 0.1 },
      { DX_DIAM,     0, 0.1 },
      { DX_CLUT,		0, 1.0 },
      { NULL },
              
		/* END OF MENUS */
      { NULL }
   },
   
   /*
    ** Graphics objects: 3 dXtargets
	 **  	FP, T1, T2
    */
   {  { DX_TARGET,	NUM_TARGETS+1 },
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
   &ht4_make_trials,
   &ht4_get_trial,
   &ht4_set_trial,
};

/* PUBLIC ROUTINE: ht4_make_trials
 **
 */
void ht4_make_trials(_PRtask task)
{
	int i, this_hazard, hazards[MAX_HAZARDS],
			num_hazards=0, targets[]={0,1};
	char buf[256];

	/* seed the random number generator with the clock */
	TOY_SRAND;

	/* check for legit hazards */
	for(i=0;i<MAX_HAZARDS;i++) {
		sprintf(buf, "ID_H%d", i+1);
		this_hazard = TIMV(task, buf);
		if(this_hazard>=0 && this_hazard<=1000)
			hazards[num_hazards++] = this_hazard;
	}

	/* Add trials:
	*    Rows are hazards
	*    Columns are targets
	*/
	tu_make_trials2D(task,
		"hazard",	num_hazards, hazards, 0.1,
		"target",	NUM_TARGETS, targets, 0.1);
}

/* PUBLIC ROUTINE: ht4_get_trial
 **
 ** Use given Hazard rates to choose next trial
 **
 ** Returns:
 **		_PRtrial if trials remain
 **		NULL otherwise
 */
_PRtrial ht4_get_trial(_PRtask task, int reset_flag)
{
   int         hazard_index, target_index, 
               get_new_hazard=0, get_new_target=0;

   /* Check if trialP array exists...
    * if not, make it with just the two trials and randomly pick one
    */
   if(!task->trialPs) {
      
      /* make trials if they don't exist */
      pr_task_make_trialP_all(task, 1);
      
      /* radomly pick one to start with */
		task->trialPs_index = TOY_RAND(task->trialPs_length);

   } else if(!((TIMV(task, "NC_repeat")==1) && 
		(task->pmf->last_score<0))) {

		/* Otherwise if not a repeat trial, 
		 *		check for change points      
		 *
       * First test extra-dimensional shift 
		*/
      if((task->trials_rows>1) && (TOY_RCMP(TIMV(task, "ED_H")))) {

         /* need these, set below */
         get_new_hazard=1;
         get_new_target=1;
         
      } else if(TOY_RCMP(
              pl_list_get_v(task->trialPs[task->trialPs_index]->list, "hazard"))) {
         
         get_new_target=1;
      }
      
      if(get_new_target) {
         
         /* get row and column of current trial */
         hazard_index = 
				floor((double)task->trialPs_index/(double)task->trials_columns);
         target_index = 
				task->trialPs_index - (hazard_index * (task->trials_columns));

         /* maybe get new hazard */
         if(get_new_hazard) {
            int old_hi = hazard_index;
            
            /* randomly pick a DIFFERENT hazard */
            if((hazard_index = TOY_RAND(task->trials_rows-1)) == old_hi)
               hazard_index = hazard_index + 1;
         }
         
         /* now get new trial */
         if(get_new_hazard) {
            target_index = TOY_RAND(2);
         } else {
            target_index = 1-target_index;
         }

         /* now get the actual trial index from row/column */
         task->trialPs_index = hazard_index*task->trials_columns+target_index;
      }
   }
 
   /* return current trial */
   return(task->trialPs[task->trialPs_index]);
}

/* PUBLIC ROUTINE: ht4_set_trial
 **
 */
void ht4_set_trial(_PRtrial trial)
{
   _PRtask  	task 		= trial->task;
   _PLlist  	tim  		= task->task_menus->lists[0],
            	tgm  		= task->task_menus->lists[1];
   _PLproperty prop_y 	= pl_list_get_prop(tgm, DX_Y);
	valtype 		y_max 	= TIMV(task, "Y_max");
	int			hazard_index = floor((double)task->trialPs_index/(double)task->trials_columns),
   				ti[] 		= {0,1,2}; /* indices of target objects */
   register int i;
  
   /* first copy real values into working copies in graphics menu */
   pl_list_set_v2w(tgm);

   /* Compute target locations 
	 * loop through the targets, check to use y_max 
	*/
   for(i=0;i<NUM_TARGETS;i++) {
      if((prop_y->values_wc[i+1]==NULLI)) {
			prop_y->values_wc[i+1] = -y_max + 
				(hazard_index*y_max*2)/(MAX_HAZARDS-1);
		}
	}				

   /* Copy "working copy" values of key target parameters
    **		to graphics objects and automatically
    **		send rSet command ...
    **
    **	First dXtargets, all at once ...
    */
   dx_setl_by_nameIV(0,
           tgm, 'w',   NUM_TARGETS+1, ti,
           DX_TARGET,  NUM_TARGETS+1, ti,
           DX_X,       NULL,
           DX_Y,       NULL,
           DX_DIAM,    NULL,
           DX_CLUT,    NULL,
           NULL);

   /* Conditionally drop codes ... spm can figure out from the cluts
    * which one is the active target ... note that this uses some
    * strangely named codes because we're running out 
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
      ec_send_code_tagged(I_T1IDH, BASE + 
              pl_list_get_v(task->trialPs[task->trialPs_index]->list, "hazard"));              
      ec_send_code_tagged(I_T1MEANT, BASE + 
              pl_list_get_v(task->trialPs[task->trialPs_index]->list, "target"));              
      ec_send_code_tagged(I_T1EDH,   BASE + TIMV(task, "ED_H"));
   }
}
