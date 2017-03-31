/*
** TASK.H
**
** "Task.h" ... defines named task 
**		prototypes. See Task.c for more details.
**
**	This file contai
*/

#ifndef TASK_H_
#define TASK_H_

#include <stdio.h>
#include "paradigm_rec.h"
#include "property_list.h"
#include "../hdr/rexHdr.h"

/* PUBLIC DATA STRUCTURES */

#define TA_MAX_LISTS 100

typedef struct _TAtask_struct  	*_TAtask;
typedef struct _TAtasks_struct  	*_TAtasks;

struct _TAufn_struct {
	char	*name;
	PR_VV	user_fn;
};

struct _TAtask_struct {
	
	char		*type;

	struct	_PLinit_struct menus[TA_MAX_LISTS];
	struct	_PLinit_struct graphics[TA_MAX_LISTS];
	struct	_TAufn_struct 	user_fns[5];

	PR_VTA	make_trials_fn;
	PR_TRTAI	get_trial_fn;
	PR_VTR	set_trial_fn;
	PR_VTA	clear_user_data_fn;
};

struct _TAtasks_struct {
	char		*type;
	_TAtask	 task;
};

/* USER DATA STRUCTURES
**
** 1. _PRRuser_data_struct from task_probRew.c
**			added 5/6/10 by jig & mn
**
** 2. _exploreruser_data_struct from task_explorer.c
**			added 13-11-14 by dlb
*/
typedef struct _PRRuser_data_struct *_PRRuser_data;
struct _PRRuser_data_struct {
   int    num_targets;
   double *angles;
   double *xs;
   double *ys;

	int	 block_count;
	int	 block_index;
	int	 block_menu_index;
	int	 chose_t0;
};

/*
**
typedef struct _exploreruser_data_struct *_exploreruser_data;
struct _exploreruser_data_struct {
   int    num_targets;
   double *angles;
   double *xs;
   double *ys;

	int	 block_count;
	int	 block_index;
	int	 block_menu_index;
	int	 chose_t0;
};
**
*/

/* PUBLIC ROUTINE PROTOTYPES */

_PRtask ta_task_by_type(char *, _PLgroup, USER_FUNC *);

#endif /* PTASK_H_ */
