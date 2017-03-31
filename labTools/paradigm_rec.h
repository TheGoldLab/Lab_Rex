/*
** PARADIGM_REC.H
**
** Header file that contains data structures 
**		and public routine prototypes
**		for setting up the hierarchical data structures
**		for a REX paradgim:
**											rec
**			task							task ...
**	trial trial trial ...	 trial trial trial ...
**
** TD 2015-05-18 
**		To give addtional reward pulse(s) shorter than the first, added:
** 	pr_set_reward2, pr_beep_reward2, pr_give_reward2, pr_give_beep2
*/

#ifndef PARADIGM_REC_H_
#define PARADIGM_REC_H_

/* INCLUDES */
#include <stdarg.h>
#include <stdio.h>
#include <sys/types.h>

#include "../hdr/rexHdr.h"
#include "property_list.h"
#include "dotsX.h"

/* DATA STRUCTURES */

/* these are the data structures defined in this file
**
** _PRpmf			-- performance data
** _PRtrial			-- info needed for single trial (display presentation)
** _PRtask			-- collection of trials and graphics objects
** _PRrecord		-- the big kahuna. everything needed for an experiment
**
*/
typedef struct _PRpmf_struct 				*_PRpmf;
typedef struct _PRtrial_struct			*_PRtrial;
typedef struct _PRtask_struct				*_PRtask;
typedef struct _PRrecord_struct			*_PRrecord;

/* And function pointers... these are
**		named for their input arguments
*/
typedef void		(*PR_VV)		(void);
typedef void 		(*PR_VTA)   (_PRtask);
typedef _PRtrial 	(*PR_TRTAI) (_PRtask,  int);
typedef void 		(*PR_VTR)   (_PRtrial);

/* _PRpmf_struct
**
**	For performance data
*/
struct _PRpmf_struct {

	/* Keep track of performance, using
	** 	a group of pre-defined outcome 
	**		tags. Each of these variables
	**		is a counter, initialized to 0.
	*/
	int			started;			/* all scores 			*/
	int			finished;		/* score >= 0 			*/
	int			rewarded;		/* reward > 0 			*/
	int			rewards;			/* total # rewards	*/
	int			correct;			/* score =  1 			*/
	int			error;			/* score =  0 			*/
	int			no_choice;		/* score = -1 			*/
	int			broken_fix;		/* score = -2 			*/

	/* Trying to make this structure more
	**		general... here we can keep track of
	**		n alternatives (array of counters, as
	**		above)
	*/
	int		  *scores;
	int			scores_length;

	/* Keep track of recent scores:
	**		- last_score is most recent score
	**		- run_length is number of times in 
	**			a row "current" occurred
	**		- last_reward is reward (e.g., # beeps) for most
	**			recent trial
	*/
	int			last_score;
	int			run_length;
	int			last_reward;

	/* Possibly keep track of history of score/reward.
	**		These are treated as circular buffers.
	*/
	int			history_length;
	int			history_index;
	int		  *score_history;
	int		  *reward_history;

   /* jig & ld added 3/29/07
   ** Keep track of RTs
   */
   int         rt_mean;
   int         rt_median;
   int        *rts;
   int         rt_len;

};

/* _PRtrial_struct
**
*/
struct _PRtrial_struct {

	int			id;
	int			repetitions;	/* # of times trial is presented in block */

	void			*user_data;		/* task-specific stuff  */

	_PLlist		list;				/* the properties used to make the trial */
	_PRpmf		pmf;				/* performance data */
	_PRtask		task;				/* ptr back to task */

};

/* _PRtask_struct
**
**	A collection of trials and graphics
**	objects
*/
struct _PRtask_struct {

	int			id;
	char			*name;
	char			*type;

	_PLgroup		task_menus;		/* task info */
	_PRpmf		pmf;				/* task-specific performance */
	void		   *user_data;		/* task-specific data */

	/* Pointer to graphics group ... this is really
	**	stored in the _PRrecord, because different tasks
	**	can share the same graphics group
	*/
	_PLgroup		graphics;
	
	/* pointer back to record 
	*/
	_PRrecord	rec;				

	/* Array of trials
	**
   ** This can be a one- or two-dimensional
	**		array. Two-dimensional arrays
	**		are stored in the standard convention:
	**		row-by-row. Thus our typical convention
	**		is to store the primary dependent
	**		variable (PDV) -- that is, the one you'd
	**		use to generate a tuning curve or
	**		psychometric function, like coherence --
	**		varies along the rows, and other variables
	**		like direction vary along the columns...
	**		so that for a particular value of
	**		the PDV, it is easy to access the
	**		block of trials to choose from.
	*/
	_PRtrial	  *trials;
	int			trials_length;
	int			trials_rows;
	int			trials_columns;
	int			trials_index;

	/* Array of pointers to trials
	**
	** These are the trials in order they will
	**	occur
	*/
	_PRtrial	  *trialPs;
	int			trialPs_length;
	int			trialPs_index;

	/* _PRtask  methods:
	**		- set_task_fn ... makes the array of trials
	**			for the task
	**		- get_trial_fn ... gets the next trial
	**		- set_trial_fn ... makes the graphics, etc, for the trial
	**		- clear_user_data_fn ... clear whatever user data you've added
	*/ 
	PR_VTA		make_trials_fn;
	PR_TRTAI		get_trial_fn;
	PR_VTR		set_trial_fn;
	PR_VTA		clear_user_data_fn;
};

/* _PRrecord_struct
**
**	The big kahuna, stores all the tasks, which 
** store all the objects and trials.
*/
struct _PRrecord_struct {

	/* Rec-specific info
	**	pmf				... performance data for the whole rec
	**	paradigm_menu 	... menu for specifying paradigm behavior 
	**							(e.g., total trials, task proportions, etc).
	**	prefs_menu 		... user preferences menu.
	**	rt_vars			... list of "real-time variables" for rex
	*/	
	_PRpmf	  	 pmf;
	_PLlist		 paradigm_menu;
	_PLlist		 prefs_menu;
	_PLlist		 clut_menu;
	_PLlist		 IP_menu;
	_PLlist		 dio_menu;
	_PLlist		 ecodes_menu;
	_PLlist		 rt_vars;

	/* dx graphics struct. This holds the groups
	**		corresponding to the graphics objects for
	**		a task. Note that it's here because different
	**		tasks can share the same graphics group. Also
	**		note that this is a copy of a pointer also stored
	**		as a global variable in dotsX.c, which allows
	**		graphics calls to be made without explicit
	**		reference to this variable; see dotsX.[ch] for
	**		details.
	*/ 
	_DXgraphics	 dx; 

	/* Array of tasks */ 
	_PRtask	  	*tasks;
	int			 tasks_length;
	int			 task_index;		/* current task */

	/* pointer to current trial 
	*/
	_PRtrial		 trialP;
};

/* PUBLIC CONSTANTS/MACROS  */

#define DIV(n) pl_list_get_v(gl_rec->dio_menu, 	 	 (n))
	/* remember fancy TTL bits are 1-based */
#define PR_DIO_ID(dv) 	Dio_id(PCDIO_DIO,4,1<<(dv-1))
#define PR_DIO_ON(n) 	{valtype dv;      \
  			if((dv=DIV(n))>=0) {dio_on(PR_DIO_ID(dv)); ec_send_dio(dv);}}
#define PR_DIO_OFF(n) 	{valtype dv;      \
         if((dv=DIV(n))>=0) dio_off(PR_DIO_ID(dv));}
#define PR_MAX_MENUS	50
#define PR_TRIAL(ta)	(((ta) && (ta)->trialPs && (ta)->trialPs_index >= 0 \
								&& (ta)->trialPs_index < (ta)->trialPs_length) ? \
								(ta)->trialPs[(ta)->trialPs_index] : NULL)
#define TR_TASK_MENU(tr,ind) 	((tr)->task->task_menus->lists[(ind)])
#define TR_DX_OBJ(tr, ind)		((tr)->task->graphics->lists[(ind)])			

enum scores {
	kNoFix = -3,
	kBrFix,
	kNC,
	kError,
	kCorrect
};

/* PUBLIC ROUTINE PROTOTYPES */
_PRrecord	pr_initV							(int, int, MENU *, _PLlist, 
														RTVAR *, _PLlist, USER_FUNC *, ...);
void			pr_cleanup						(void);
void			pr_setup							(void);
void			pr_print							(void);
void			pr_print_pmf					(void);

_PRrecord	pr_get_rec						(void);
void			pr_add_ufunc					(USER_FUNC *, char *, PR_VV);
int			pr_get_task_index				(void);
valtype		pr_get_task_menu_value		(char *, char *, int);
valtype		pr_get_trial_property		(char *, int);
valtype		pr_get_pref						(char *);
void			pr_set_rtvar					(char *, valtype);
void			pr_set_clut						(void);

int			pr_make_trials					(void);
int			pr_get_trial					(void);
int			pr_set_trial					(void);

int			pr_start_trial					(void);
int			pr_score_trial					(long, long, long);
int			pr_score_trialPRC				(long, long, long, long, long, long);
int         pr_score_trialRT           (long, long, long, long);
int			pr_set_reward					(int, long, long, int, long, long);
int			pr_set_reward2					(int, long, long, long, int, long, long, long);
int			pr_beep_reward					(void);
int			pr_beep_reward2				(void);
int			pr_give_reward					(void);
int			pr_give_reward2				(void);
int			pr_give_beep					(void);
int			pr_give_beep2					(void);
int			pr_finish_trial				(void);
int			pr_toggle_file					(int);

	/* RECORD 		*/
_PRrecord	pr_rec_init						(int, int);
void			pr_rec_add_task				(_PRrecord, _PRtask, int);
int			pr_rec_get_task_first		(_PRrecord);
int			pr_rec_get_task_random		(_PRrecord);
void			pr_rec_print					(_PRrecord);
void			pr_rec_clear					(_PRrecord, int);
void			pr_rec_free						(_PRrecord);

	/* TASK 			*/
_PRtask		pr_task_init					(void);
void			pr_task_add_trials1D			(_PRtask, int);
void			pr_task_add_trials2D			(_PRtask, int, int);
void			pr_task_make_trialP_all		(_PRtask, int);
void			pr_task_make_trialP_allR	(_PRtask, int);
void			pr_task_make_trialP_row		(_PRtask, int, int);
void			pr_task_make_trialP_rowR	(_PRtask, int, int);
void			pr_task_make_trialP_column	(_PRtask, int, int);
void			pr_task_make_trialP_columnR(_PRtask, int, int);
void			pr_task_randomize_trialP	(_PRtask);
void			pr_task_swap_trialP			(_PRtask);
void			pr_task_print_pmf				(_PRtask);
void			pr_task_print					(_PRtask);
void			pr_task_clear					(_PRtask);
void			pr_task_free					(_PRtask);

	/* TRIAL 		*/
_PRtrial		pr_trial_init					(void);
void        pr_trial_set_pmf_scoreRT   (_PRtrial, int, long);
void			pr_trial_set_pmf_score		(_PRtrial, int);
void			pr_trial_set_pmf_reward		(_PRtrial, int);
void			pr_trial_print_pmf			(_PRtrial);
void			pr_trial_print					(_PRtrial);
void			pr_trial_free					(_PRtrial);

	/* PMF 			*/
_PRpmf		pr_pmf_init						(int, int);
_PRpmf		pr_pmf_copy						(_PRpmf);
void			pr_pmf_set_score				(_PRpmf, int);
void			pr_pmf_set_reward				(_PRpmf, int);
void			pr_pmf_print					(_PRpmf);
void			pr_pmf_clear					(_PRpmf);
void			pr_pmf_free						(_PRpmf);

#endif /* PARADIGM_REC_H_ */
