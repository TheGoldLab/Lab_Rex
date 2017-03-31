/*
** task_grid.c
**
**		grid of dXtargets
*/

#include "../toys.h"
#include "../task.h"
#include "../task_utilities.h"

/* PRIVATE MACROS/CONSTANTS */
#define NUM_GR_MENUS 2
#define TI 				"setup"
#define TIMV(ta,n) 	pl_list_get_v((ta)->task_menus->lists[0],(n))

/* PRIVATE DATA STRUCTURES */

/* PRIVATE ROUTINES */
void 		gr_make_trials	(_PRtask);
_PRtrial gr_get_trial	(_PRtask, int);
void 		gr_set_trial	(_PRtrial);

/* PRIVATE VARIABLES */

/* 
** TASK: GR
**		("GRid-Target")
*/
struct _TAtask_struct gl_grid_task[] = {

	/* name */
	"grid",

	/* 
	** Menus 
	*/
	{
		/* 
		** Task info
		*/
		{ TI, 					1 },
		{ "Separation",		0, 1.0 },
   	{ NULL },
		{ NULL }},

	/* 
	** Graphics objects, by name
	*/
	{  { DX_TARGETS,	1 }, 
		{ NULL }},

	/* 
	**	Task methods
	*/
	&gr_make_trials,
	&gr_get_trial,
	&gr_set_trial,
};

/* PUBLIC ROUTINE: gr_make_trials
**
*/
void gr_make_trials(_PRtask task)
{
	int val = 0;

	/* 
	** Make 1 dummy trial
	*/
	tu_make_trials1D(task, "dummy", 1, &val, 0.1);
}

/* PUBLIC ROUTINE: gr_get_trial
**
** Returns:
**		_PRtrial if trials remain
**		NULL otherwise
*/
_PRtrial gr_get_trial(_PRtask task, int reset_flag)
{
	return(task->trials[0]);
}

/* PUBLIC ROUTINE: gr_set_trial
**
*/
void gr_set_trial(_PRtrial trial)
{
	char buf[256];
	int val = trial->task->task_menus->lists[0]->properties[0]->values[0];

	sprintf(buf, "rSet('dXtargets',1,'visible',true,'x',[-%d,0,%d,-%d,0,%d,-%d,0,%d],'y',[%d,%d,%d,0,0,0,-%d,-%d,-%d],'diameter',0.8,'color',2);draw_flag=1;",
		val,val,val,val,val,val,val,val,val,val,val,val);
	printf("buf is %s\n", buf);

	dx_send(buf);
}

