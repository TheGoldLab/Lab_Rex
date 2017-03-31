/*
** TASK.C
**
** "Task.c" ... 
**		for setting up _PRtasks
**
**	IMPORTANT: This file contains the registry
**		of tasks (defined in task*.c files)
*/

/* ---- History ----
**
**	11/10/2016 task list simplified by jig... add as appropriate per rig
**
*/


#include "toys.h"
#include "task.h"

/* PUBLIC REGISTRIES */

/*
** Registry of tasks. These are typically
**		defined in Tasks/task*.c
*/	
extern struct _TAtask_struct gl_asl_task;
extern struct _TAtask_struct gl_fix_task;
extern struct _TAtask_struct gl_ft_task;
extern struct _TAtask_struct gl_dots_task;
extern struct _TAtask_struct gl_dotsardt_task;
extern struct _TAtask_struct gl_dotsreg_task;
extern struct _TAtask_struct gl_dotsrt_task;
extern struct _TAtask_struct gl_mgs_task;
extern struct _TAtask_struct gl_vgs_task;

struct _TAtasks_struct gl_tasks[] = {
   { "asl",       &gl_asl_task 	},
   { "fix",       &gl_fix_task 	},
   { "ft",        &gl_ft_task 	},
   { "dots",      &gl_dots_task },
   { "dotsardt",  &gl_dotsardt_task },
   { "dotsreg",   &gl_dotsreg_task },
   { "dotsrt",    &gl_dotsrt_task },
   { "mgs",       &gl_mgs_task 	},
   { "vgs",       &gl_vgs_task 	},
 };
  
/* PRIVATE MACROS/CONSTANTS */

/* PRIVATE DATA STRUCTURES */

/* PRIVATE VARIABLES */

/* PRIVATE ROUTINES */

/* PUBLIC MACROS  */

/* PUBLIC ROUTINES */

/* PUBLIC ROUTINE: ta_task_by_type
**
** Gets a named task
**
**	Arguments: char * type
**				  _PLgroup graphics ... possible group of
**							graphics objects to use (share)
**
**	Returns: the _PRtask
*/
_PRtask ta_task_by_type(char *type, _PLgroup graphics, USER_FUNC *ufuncs)
{	
	register int i;
	int index=-1, 
		num_tasks = sizeof(gl_tasks)/sizeof(struct _TAtasks_struct);
	_PRtask task;

	for(i=0;i<num_tasks;i++)
		if(!strcmp(type, gl_tasks[i].type)) {
			index = i;
			break;
		}

	/* NOT FOUND!!!! */
	if(index == -1) {
		printf("TASK <%s> NOT FOUND!!!\n", type);
		return(NULL);
	}

	/* set up the task */
	task = pr_task_init();

	/* set the type */
	task->type = TOY_CARBONCOPY(type);

	/* add task menus */
	task->task_menus = 
		pl_init_to_group(&(gl_tasks[index].task->menus));

	/* add graphics, if necessary */
	if(graphics)

		/* just use the given graphics group */
		task->graphics = graphics;

	else

		/* make it */
		task->graphics = 	
			dx_make_graphic(gl_tasks[index].task->graphics);

	/* add user funcs */
	i=-1;
	while(gl_tasks[index].task->user_fns[++i].name)
		pr_add_ufunc(ufuncs, 
			gl_tasks[index].task->user_fns[i].name,
			gl_tasks[index].task->user_fns[i].user_fn);

	/* add methods */
	task->make_trials_fn = gl_tasks[index].task->make_trials_fn;
	task->get_trial_fn 	= gl_tasks[index].task->get_trial_fn;
	task->set_trial_fn 	= gl_tasks[index].task->set_trial_fn;
   task->clear_user_data_fn   = gl_tasks[index].task->clear_user_data_fn;

	return(task);
}

