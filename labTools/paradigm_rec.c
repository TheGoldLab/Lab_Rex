/*
** PARADIGM_REC.C
**
**	Routines, etc, for setting up rec, tasks 
** and trials to be used in a REX paradigm.
*/

/* Stim_off_bit was added to dio menu by TD (11/09/2011)
**
** pr_change_block() and current_block are added for estimPostSacTD.d (06/25/12 TD)
**

   TD 2013-06-12
   Reward_off_bit and Beep_off_bit are added to FancyTTL menu.
   These are dummy and just used to get the timing information via dio send code.
 
   TD 2015-05-18
   created pr_set_reward2() and pr_give_reward2() for a shorter additional reward pulse
*/

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/types.h>
#include <x86/inline.h>
#include "paradigm_rec.h"
#include "task.h"
#include "toys.h"
#include "../hdr/rexHdr.h"
#include "../sset/lcode.h"

/* PUBLIC MACROS */
#define PRV(n) pl_list_get_v(gl_rec->prefs_menu, 	 (n))
#define CLV(n) pl_list_get_v(gl_rec->clut_menu, 	 (n))
#define IPV(n) pl_list_get_v(gl_rec->IP_menu, 	 	 (n))
#define ECV(n) pl_list_get_v(gl_rec->ecodes_menu, 	 (n))
#define PMV(n) pl_list_get_v(gl_rec->paradigm_menu, (n))
#define PMW(n) pl_list_get_w(gl_rec->paradigm_menu, (n))

/* MACROS to access "Proportion_T*" properties in paradigm_menu;
**		regular and working copies
*/
#define PIND 4 /* index of first "proportion" arg */
#define TPV  PL_L2PV(gl_rec->paradigm_menu, PIND+gl_rec->task_index)
#define TPW  PL_L2PW(gl_rec->paradigm_menu, PIND+gl_rec->task_index)
#define RW   PL_L2PW(gl_rec->paradigm_menu, 3)

enum {
	kTaskBlock,
	kTaskRandom,
	kTaskRepeatTask,
	kTaskRepeatTrial
};

/* GLOBAL VARIABLES
**
**  ooooh. icky.
*/
static _PRrecord	gl_rec=NULL;
static VLIST 		*gl_vlistPs[PR_MAX_MENUS];
static int			gl_reward_count;
static int 			gl_beep_count;

/* PRIVATE GLOBALS */
static long gl_reward_times[2];
static long gl_beep_times[2];

// td 2015-05-18
// [0] initial pulse length, [1] inter pulse interval, [2] additional pulse length
static long gl_reward_times2[3];
static long gl_beep_times2[3];


/* PRIVATE ROUTINE PROTOTYPES
**
**	These are for automatic generation of
**		Rex menus
*/
void pr_list_to_menu(_PLlist, MENU *, int, const char *);
int  pr_agf(int call_cnt, MENU *mp, char *astr);
int  pr_maf(int flag, MENU *mp, char *astr, ME_RECUR *rp);

/* PUBLIC ROUTINES */

/* PUBLIC ROUTINE: pr_initV
**
** The Big Kahuna that initializes and sets up
**		the global gl_rec to be used
**		in a paradigm. V for va_args. 
**	This routine is typically called from the "autoinit"
**		routine in a spot (.d) file.
**
** Arguments:
**		pmf_scores   	... number of unique scores 
**		pmf_history  	... number of previous trials to store	
**		umenus			... array of MENUs
**		prefL  	 	 	... _PLlist definitions of preferences menu fields
**		rtvars			... array of RTVARs
**		rtvarsL  	 	... _PLlist definitions of rtvars
**		num tasks		... number of tasks, specifies remaining args
**		va_args 			... NULL-teriminated list of pairs:
**									task_type, num.
**									IMPORTANT: note that each task made
**										from a single "type,num" pair here
**										uses the SAME graphics group; different
**										pairs (even of the same task type) use
**										DIFFERENT graphics objects.	
**
** Returns: 
**		_PRrecord ... pointer to the _PRrecord_struct
*/
_PRrecord pr_initV(int pmf_scores, int pmf_history, 
		MENU  *umenus, _PLlist prefL, 
		RTVAR *rtvars, _PLlist rtvarsL, 
		USER_FUNC *ufuncs, ...)

{
	va_list ap;
	register int i,j;
	char buf[256], *type;
	int num, task_count = 0,
			/* CAREFUL -- we're starting the menu
			** 	count at "1" because we fill in
			**		the first ("Paradigm") entry later
			*/
 			menu_count = 1;
	_PRtask  task;
	_PLgroup graphics;
	_PLproperty pr, pg, pb;

	/* clear gl_vlistPs array */
	for(i=0;i<PR_MAX_MENUS;i++)
		gl_vlistPs[i] = NULL;
	
	/* Make the new rec */
	gl_rec = pr_rec_init(pmf_scores, pmf_history);

	/* CREATE PARADIGM MENU. 
	**		(Make menu only after all the tasks have been added.)
	**
	** Includes default fields:
	**
	**	Total_trials  ... limits number of trials
	**							(unused if <0). Note that
	**							CHANGING this value calls 
	**							pr_make_trials()
 	** Task_order    ... kTaskBlock = blockwise tasks, in order
 	**                   kTaskRandom = randomly interleave tasks
	**							kTaskRepeatTask = repeat previous task
	**							kTaskRepeatTrial = repeat previous trial
	** Trial_repeats ... Training function -- number of trials
	**							in a row required to be correct before
	**							selecting next trial
 	** Repetitions	  ... <0 = no end condition
	**							0 = stop after no trials returned
 	** 						>0 = repeat n times
	**							Note that CHANGING this value calls 
	**							pr_make_trials()
	** Proportion_T* ... proportion of trials from each task
	*/
	gl_rec->paradigm_menu = pl_list_initV("Paradigm", 0, 1,
		"Total_trials", 0, 				1.0,
		"Task_order", 	 kTaskBlock, 	1.0,
		"Trial_repeats",0,				1.0,
		"Repetitions",	 0, 				1.0,
		NULL);

	/* ADD THE TASKS one at a time
	**		Also add the task-specific menus,
	**		separated by a "separator"
	*/
	va_start(ap, ufuncs);
	while(type = va_arg(ap, char *)) {

		/* get the number (count) of this type of task to add */
		num = va_arg(ap, int);

		/* First task in a list creates a graphics object ...
		** 	all the rest share it. See below.
		*/
		graphics = NULL;

		/* get the task */
		while(num-- && (task = ta_task_by_type(type, graphics, ufuncs))) {

			/* Name using the index ... used for menu names, below */
			sprintf(buf, "T%d", task_count);
			task->name = TOY_CARBONCOPY(buf);

/*
**			printf("Adding task %s, type %s\n", task->name, type);
**			fflush(stdout);
*/
			/* Add it to the rec, using task_count as the id */ 
			pr_rec_add_task(gl_rec, task, task_count);

			/*  If this was the first time through,
			**  add the graphics group to the main list; otherwise
			**	 the call to ta_task_by_type should have given
			**  this task a pointer to the shared graphics group.
			*/
			if(!graphics) {
				graphics = task->graphics;
				dx_add_graphic(gl_rec->dx, graphics);
			}
			
			/* add a "Proportion_T*" field to the menu */
			sprintf(buf, "Proportion_%s(%s)", task->name, task->type);
			pl_list_append(gl_rec->paradigm_menu, buf, 0, 1.0);

			/* add the menus */
			if(task->task_menus) { 

				/* add the separator */
				umenus[menu_count].me_name  = TOY_CARBONCOPY("separator");
				umenus[menu_count++].me_vlp = NP;

				/* add the menu(s) */ 
				sprintf(buf, "%s%s_", task->name, task->type);
				for(i=0;i<task->task_menus->lists_length;i++)
					pr_list_to_menu(task->task_menus->lists[i], 
						umenus, menu_count++, buf);
			}

			/* increment the task counter */
			task_count++;
		}
	}

	/* cleanup va args */
	va_end(ap);

	/* Add user-supplied entries to the rec menu 
	** 	and make it a Rex menu. Note that this
	**		is the FIRST (index=0) menu. We put it
	**		here because above we added task 
	**		"proportions". Finally, add "pr_make_trials"
	**		as the access funtion when "Total_trials"	is updated.
	*/
	pr_list_to_menu(gl_rec->paradigm_menu, umenus, 0, NULL);
	(gl_vlistPs[0])[0].vl_accf = pr_make_trials;
	(gl_vlistPs[0])[0].vl_flag = ME_AFT;

	/* add a separator */
	umenus[menu_count].me_name  = TOY_CARBONCOPY("separator");
	umenus[menu_count++].me_vlp = NP;
	
	/* CREATE PREFS MENU
	**
	**	Reset_pmf	  		... 0 = do NOT reset pmf with new trials
	** 							 1 = DO reset pmf with new trials
	**	Monitor_width_cm	... Values need by dx_set_screen;
	**	View_dist_cm		... 	see pr_setup, below.
	**	Reward_time			...
	**	Show_corner			... put a small object in the corner of
	**									the screen during fixation
	*/
	gl_rec->prefs_menu = pl_list_initV("Preferences", 0, 1,
		"Reset_pmf",				0, 			1.0,
		"Constant_proportions",	0, 			1.0,
		"Monitor_width_cm",		37, 			1.0,
		"View_dist_cm",			60, 			1.0,
		"Reward_on_time",			250,			1.0,
		"Reward_off_time",		100,			1.0,
		"Reward_system_flag",	0,				1.0,
		"Show_corner",				0, 			1.0,
		"Dump_commands",			1,				1.0,
		NULL); 
	if(prefL)
		pl_list_cat(gl_rec->prefs_menu, prefL);
	pr_list_to_menu(gl_rec->prefs_menu, umenus, menu_count++, NULL);

	/* CREATE DIO (Fancy TTL) MENU 
	**
	**	Defines the bits on byte 4 (Fancy TTL panel)
	**	of the DIO. Also turn all bits 'off'
	**
   ** jig 11/11/2016: Here are a bunch of bits that need cleaning up:
	** 	"Reward_off_bit",		1, 		1.0,
	** 	"Beep_off_bit",		2, 		1.0,
	** 	"Stim_toggle_bit2",  4,       1.0,
	** 	"Stim_on_bit2",      7,       1.0,
	**
	**	from Taka:
	** 	Stim_off_bit was added by TD (11/09/2011)
   **  	This is dummy: Actual stim off is done w/ Stim_on_bit. But
   **  	the timing of stim off is sent to Plexon by using Stim_off_bit.
   */
	gl_rec->dio_menu = pl_list_initV("FancyTTL", 0, 1,
		"Reward_bit",			2, 			1.0,
		"Beep_bit",				3, 			1.0,
		"Stim_toggle_bit",	5, 			1.0,
		"Stim_on_bit",			6, 			1.0,
		"Stim_off_bit",   	7, 			1.0,
		"Plexon_toggle_bit",	8, 			1.0, 
		NULL); 
	pr_list_to_menu(gl_rec->dio_menu, umenus, menu_count++, NULL);
	dio_off(Dio_id(PCDIO_DIO, 4, 0xff));

	/* CREATE ECODES MENU 
	**
	**	Specifies certain ecodes that are dropped
	**	automatically by paradigm_rec routines
	*/
	gl_rec->ecodes_menu = pl_list_initV("Ecodes", 0, 1,
		"Start_code",			STARTCD, 	1.0,
		"Task_id_code",		I_TASKIDCD,	1.0,
		"Trial_id_code",		I_TRIALIDCD,1.0,
		"List_done_code", 	LISTDONECD, 1.0,
		"All_off_code", 		ALLOFFCD, 	1.0,
		"Broken_fixation",	BRFIXCD,		1.0,
		"No_choice",			NOCHCD,		1.0,
		"Correct",				CORRECTCD,	1.0,
		"Error",					WRONGCD,		1.0,
		NULL); 
	pr_list_to_menu(gl_rec->ecodes_menu, umenus, menu_count++, NULL);

	/* CREATE COLORS MENU
	**	 and add user function "pr_set_clut" to update
	**
	**	Each color is defined by the following values:
	**		index 	(0 ... PR_CLUT_SIZE) ... size is actually
	**						limited by the clut allocated in SCREEN
	**		r			(0 ... 255)	
	**		g			(0 ... 255)	
	**		b			(0 ... 255)	
	**		luminance(0 ... 1000)
	*/
#define PR_CLUT_SZ 16
	gl_rec->clut_menu = pl_list_initV("CLUT", 0, PR_CLUT_SZ,
		"r", 				 		0, 			1.0,
		"g", 				 		0, 			1.0,
		"b", 				 		0, 			1.0,
		"luminance", 		1000, 			1.0,
		NULL);
	pr = pl_list_get_prop(gl_rec->clut_menu, "r");
	pg = pl_list_get_prop(gl_rec->clut_menu, "g");
	pb = pl_list_get_prop(gl_rec->clut_menu, "b");
	i  = 0;
#define CSET(r,g,b) {pr->values[i]=(r);pg->values[i]=(g);pb->values[i]=(b);i++;}
	CSET(	 0,	  0,	  0);
	CSET(255,	255,	255);
	CSET(255,	  0,	  0);
	CSET(	 0,	255,	  0);
	CSET(	 0,	  0,	255);
	CSET(255,	255,	  0);
	CSET(255,	  0,	255);
	CSET(	 0,	255,	255);
	pr_list_to_menu(gl_rec->clut_menu, umenus, menu_count++, NULL);	
	pr_add_ufunc(ufuncs, "pr_set_clut", pr_set_clut);
	pr_add_ufunc(ufuncs, NS, NULL);

	/* CREATE IP MENU 
	**
	** Determines the connection from this machine	
	**	to the mac ... see udp.[ch] for details
	*/
	gl_rec->IP_menu = pl_list_initV("IP", 0, 1,
		"IP_f1",					192, 			1.0,
		"IP_f2",					168,			1.0,
		"IP_f3",					16,			1.0,
		"This_IP_f4",			252,			1.0,
		"Mac_IP_f4",			6,				1.0,
		"Port",					6665,			1.0,
		NULL); 
	pr_list_to_menu(gl_rec->IP_menu, umenus, menu_count++, NULL);

	/* terminate menu list with NULL */
	pr_list_to_menu(NULL, umenus, menu_count, NULL);

	/* FILL RTVARS MENU.
	**
	** Make Rex rtvars menu using pointers to
	** 	values in rec->pmf.
	**	Remember that the max name length is 20.
	*/
#define RTV(nm,ptr) { 						\
		strcpy(rtvars[i].rt_name, (nm)); \
	 	rtvars[i++].rt_var = (ptr); }
	i=0;
	RTV("Started", 	&gl_rec->pmf->started);
	RTV("Finished", 	&gl_rec->pmf->finished);
	RTV("Rewarded", 	&gl_rec->pmf->rewarded);
	RTV("Rewards",  	&gl_rec->pmf->rewards);
	RTV("Correct", 	&gl_rec->pmf->correct);
	RTV("Error", 		&gl_rec->pmf->error);
	RTV("No choice", 	&gl_rec->pmf->no_choice);
	RTV("Broken fix", &gl_rec->pmf->broken_fix);
	RTV("Score", 		&gl_rec->pmf->last_score);
	RTV("Run length", &gl_rec->pmf->run_length);

	/* Conditionally add user-supplied rt_vars */
	if(rtvarsL) {
		
		/* make a list from the user-supplied rt_vars */
		gl_rec->rt_vars = rtvarsL;

		/* add the entries of the list to the rtvars array */
		for(j=0;j<gl_rec->rt_vars->properties_length;j++) {
			sprintf(rtvars[i].rt_name, "%s", 
				gl_rec->rt_vars->properties[j]->name);
			rtvars[i++].rt_var  = &(gl_rec->rt_vars->properties[j]->values[0]);
		}
	}
	/* trailing empty string to finish the rtvars array */
	sprintf(rtvars[i].rt_name, "");

	/* return a pointer to the rec */
	return(gl_rec);
}

/* PUBLIC ROUTINE: pr_setup
**
** Setup routine, typically called ONCE from
**		rinit in a spot (.d) file.
** This is separate from pr_initV, above,
**		because that routine initializes all the
**		data structures & MENUs, whereas this routine
**		makes the tasks/trials and sends the 
**		appropriate commands to the dotsX
**		client AFTER all the MENU entries have 
**		(potentially) been updated by a root file.
**		
*/
void pr_setup(void)
{
	char buf1[32], buf2[32], buf[256];

	/* set up the UDP (QNX-MAC) connection */
	sprintf(buf1, "%d.%d.%d.%d",
		IPV("IP_f1"), IPV("IP_f2"), IPV("IP_f3"), IPV("This_IP_f4"));
	sprintf(buf2, "%d.%d.%d.%d",
		IPV("IP_f1"), IPV("IP_f2"), IPV("IP_f3"), IPV("Mac_IP_f4"));
	udp_open(buf1, buf2, IPV("Port"));

	/* set up dotsX graphics ("dx") struct */
	dx_setup(PRV("Monitor_width_cm"), PRV("View_dist_cm"));

	/* set the clut */
	pr_set_clut();

	/* make trials from the task definition */
	pr_make_trials();

	/* drop a code, baby */
	ec_send_code(HEADCD);
}

/* PUBLIC ROUTINE: pr_cleanup
**
*/
void pr_cleanup(void)
{

	/* free the global rec */
	pr_rec_free(gl_rec);

	/* close the udp connection */
	udp_close();
}

/* PUBLIC ROUTINE: pr_print
**
*/
void pr_print(void)
{
	pr_rec_print(gl_rec);
}

/* PUBLIC ROUTINE: pr_print_pmf
**
*/
void pr_print_pmf(void)
{	
	register int i;

	/* Rec pmf */
	printf("****PMF**** REC: ");
	pr_pmf_print(gl_rec->pmf);

	/* Task pmfs */
	for(i=0;i<gl_rec->tasks_length;i++)
		if(PL_L2PV(gl_rec->paradigm_menu, PIND+i))
			pr_task_print_pmf(gl_rec->tasks[i]);

	/* Trailing blank line */
	printf("\n");
}

/* PUBLIC ROUTINE: pr_get_rec
**
**	Just returns a copy of the global
**		gl_rec (_PRrecord)
*/
_PRrecord pr_get_rec(void)
{
	return(gl_rec);
}

/* PUBLIC ROUTINE: pr_add_ufunc
**
**	Adds user functions
*/
void pr_add_ufunc(USER_FUNC *ufuncs, char *name, PR_VV func)
{
	static int index = 0;
	register int i;

	/* check for NS (end of the line) */
	if(!strcmp(name, NS)) {
		sprintf(ufuncs[index].n_name, name);
		return;
	}
		
	/* Check that it doesn't already exist.. this
	**	can happen for task-specific user functions
	**	that come from tasks that are added multiple 
	**	times.
	*/
	for(i=0;i<index;i++)
		if(ufuncs[i].n_ptr == func)
			return;

	/* add it to the list */
	sprintf(ufuncs[index].n_name, name);
	sprintf(ufuncs[index].formatString, "");
	ufuncs[index++].n_ptr = func;
}

/* PUBLIC ROUTINE: pr_get_task_index
**
**	Just returns the current task index
*/
int pr_get_task_index(void)
{
	return(gl_rec->task_index);
}

/* PUBLIC ROUTINE: pr_get_task_menu_value
**
*/
valtype pr_get_task_menu_value(char *menu_name, 
		char *entry_name, int value_index)
{
	if(!gl_rec->tasks || gl_rec->task_index < 0 || 
		gl_rec->task_index > gl_rec->tasks_length)
		return(NULLI);

	return(pl_group_get_vi(gl_rec->tasks[gl_rec->task_index]->task_menus,
		menu_name, entry_name, value_index));
}

/* PUBLIC ROUTINE: pr_get_trial_property
**
*/
valtype pr_get_trial_property(char *name, int value_index)
{ 
	if(!gl_rec->trialP)
		return(PL_NOVAL);

	return(pl_list_get_vi(gl_rec->trialP->list, 
									name, value_index));
}

/* PUBLIC ROUTINE: pr_get_pref
**
*/
valtype pr_get_pref(char *name)
{
	return(pl_list_get_v(gl_rec->prefs_menu, name));
}

/* PUBLIC ROUTINE: pr_set_rtvar
**
*/
void pr_set_rtvar(char *name, valtype val)
{
	pl_list_set_v(gl_rec->rt_vars, name, val);
}

/* PUBLIC ROUTINE: pr_set_clut
**
** "User-supplied function" in Rex (in
**		User Menus) to update the color look-up
**		table (CLUT) in dotsX. The CLUT is just
**		a matrix of <r,g,b,luminance> to be 
**		accessed by index (i.e., row in the matrix).
**		Here we send a matrix to update a part
**		of the clut; anything that has changed
**		(in the CLUT menu/list) is sent as:
**		<index, r, g, b, luminance>
*/
void pr_set_clut(void)
{
	register int i,changed=0;
	char buf[1024];
	_PLproperty pr=gl_rec->clut_menu->properties[0],
	 			   pg=gl_rec->clut_menu->properties[1],
					pb=gl_rec->clut_menu->properties[2],
					pl=gl_rec->clut_menu->properties[3];

	/* init command */
	/* jig 11/10/16 -- need to check this
	**
	** sprintf(buf, "rScreen('DxSetClut',[");
	*/

	/* append to command from list */
	for(i=0;i<gl_rec->clut_menu->values_length;i++) {
		if(pr->values[i] != pr->values_wc[i] ||
			pg->values[i] != pg->values_wc[i] ||
			pb->values[i] != pb->values_wc[i] ||
			pl->values[i] != pl->values_wc[i]) {
			changed++;
// CHECK: sprintf(buf,"%s%d,%d,%d,%d,%d;", buf, i,
			sprintf(buf,"clutX(%d, [%d,%d,%d,%d]);", i,
				pr->values[i],
				pg->values[i],
				pb->values[i],
				pl->values[i]);
			pr->values_wc[i] = pr->values[i];
			pg->values_wc[i] = pg->values[i];
			pb->values_wc[i] = pb->values[i];
			pl->values_wc[i] = pl->values[i];
			dx_send(buf); // not in new version
		}
	}	

	/* jig 11/10/16 -- need to check this
	**
	** if(changed) {
	** strcpy(&(buf[strlen(buf)-1]), "]);");
	** printf("set clut, buf is <%s>\n", buf);
	** fflush(stdout);
	** dx_send(buf);	
	** }
	**
	*/
}

/* PUBLIC ROUTINE: pr_make_trials
**
** Make the trial array for each task
**	 using task-specific "make_trials" methods
**
**	Returns:
**		0 for no error
*/
int pr_make_trials(void)
{
	register int i, count=0;

	/* Check "Reset_pmf" preference to determine whether	
	** 	or not to reset all pmfs
	*/
	pr_rec_clear(gl_rec, PRV("Reset_pmf"));

	/* Make working copies of the Paradigm_vars menu ...
	** This resets the working value of the "Proportion"
	** 	fields, which are used to select tasks/trials
	*/
	pl_list_set_v2w(gl_rec->paradigm_menu);

	/* reset the task index */
	gl_rec->task_index = pr_rec_get_task_first(gl_rec);

	/* Loop through the tasks, calling the
	** 	make_trials_fn methods. First check
	**		the paradigm_menu->Proportion_T* values
	**		if the task is being used.
	*/
	for(i=0;i<gl_rec->tasks_length;i++) {
		if(PL_L2PV(gl_rec->paradigm_menu, PIND+i)) {
			(*(gl_rec->tasks[i]->make_trials_fn))
				(gl_rec->tasks[i]);
			count++;
		}
	}

	/* print the result, if any tasks were made */
	if(count)
		pr_rec_print(gl_rec);
	else
		printf("pr_make_trials: no tasks\n");

	/* added TD 06/25/12 
	** jig removed 11/11/2016
	**
  	** 	pr_ini_current_block(); 
   **		pr_one_finished_flag();
	*/

	/* outta */
	return(0);
}

/* PUBLIC ROUTINE: pr_get_trial
**
** Gets the next trial from the current task.
**	Behavior depends on paradigm_menu variables
**		Total_trials
**		Repetitions
**		Task_order ... note that we use the WORKING COPY,
**						because it might have been overridden in
**						the paradigm. This gets reset in pr_start_trial
**						once this is all done.
**
** See below for details.
**
** Returns:
**		0 if error (e.g., no trials remain)
**		1 if no error 
*/
int pr_get_trial(void)
{

/* MACROS:
**	 1. GTF: Call task-specific get_trial_fn and
**					set gl_rec->trialP
**	 2. RET: reset working copy of "task_order" to value 
**					and outta
*/
#define GTF(flag) (gl_rec->trialP = 								\
	(*(gl_rec->tasks[gl_rec->task_index]->get_trial_fn))  	\
	(gl_rec->tasks[gl_rec->task_index], (flag)))
#define RET(rv)	{ pl_prop_set_v2w(pl_list_get_prop(			\
							gl_rec->paradigm_menu, "Task_order"));	\
						 	return(rv); }

	long	*var_ptrs[20]; /* trust me */
	int 	num_vars, prop, again, ind, reps = RW, cps,
			trial_repeats 	= PMV("Trial_repeats"),
			task_order  	= PMV("Task_order"),
			total 			= PMV("Total_trials");
	register int i;

	/* End conditions:
	** 	No tasks
	**		Total_trials reached
	**			NOTE special case: if Total_trials < 0,
	**			it just keeps going and going	and going
	**			and going and going and going and going
	**			and going and going and going and going
	**			and going and going and going and going
	**			and going and going and going and going
	**			and going and going and going and going
	**			and going and going and going and going
	*/
	if(gl_rec->tasks_length == 0 || 
		(total >= 0 && gl_rec->pmf->finished >= total)) {

		/* clear current task pointer and outta */
		gl_rec->trialP = NULL;
		RET(0);
	}
	/* check current task_index */
	if(gl_rec->task_index < 0 || 
		gl_rec->task_index >= gl_rec->tasks_length)
		gl_rec->task_index = 0;

	/* Check trial_repeats... Special case
	** used during training to force the monkey
	** to get the given number of identical
	**	trials correct in a row before moving on
	*/
	if(gl_rec->trialP != NULL && trial_repeats > 0 && 
		(gl_rec->pmf->last_score != 1 ||
		gl_rec->pmf->run_length < trial_repeats)) {

		/* Do nothing, return value indicates if
		**		the current trial actually exists.
		**	Ignores "reps".
		** Set flag in working copy of trial repeats
		**		so other routines can find out whether
		**		this condition was met
		*/
		pl_list_set_w(gl_rec->paradigm_menu, "Trial_repeats", 1);
		RET(1);
	} else {
		pl_list_set_w(gl_rec->paradigm_menu, "Trial_repeats", 0);
	}	

	switch(task_order) {

		/* REPEAT TRIAL condition.
		*/
		case kTaskRepeatTrial:

			/* Do nothing, return value indicates if
			**		the current trial actually exists.
			**	Ignores "reps".
 			*/
			RET(gl_rec->trialP != NULL);

		/* REPEAT TASK condition.
		*/
		case kTaskRepeatTask:
	
			/* Use current task pointer. Send in reset_flag
			** 	as true, to force update if end-of-the-line
			**	Ignores "reps".
			*/
			RET(GTF(1) != NULL);

		/* BLOCK or RANDOMIZE condition.
		*/
		default:

			/* Get task index from the working copies of
			**		the proportions fields 
			*/
			if(PMV("Task_order") == kTaskBlock) {
				gl_rec->task_index = pr_rec_get_task_first(gl_rec);
				cps = 0;	/* never use constant proportions in block mode */
			} else {
				gl_rec->task_index = pr_rec_get_task_random(gl_rec);
				cps = PRV("Constant_proportions");
			}
			/* check for no good tasks */
			if(gl_rec->task_index == -1) {

				/* Done -- nothing left */
				if(!reps) {
					gl_rec->trialP = NULL;
					RET(0);
				}

			printf("No good tasks, still %d reps\n", reps);

				/* otherwise reset proportions and trialP
				** arrays (per task), decrement
				** reps and try again 
				*/
				for(i=0;i<gl_rec->tasks_length;i++) {
					SAFE_FREE(gl_rec->tasks[i]->trialPs);
					gl_rec->tasks[i]->trialPs_length = 0;
					gl_rec->tasks[i]->trialPs_index  = -1;
				}
				pl_list_set_v2w(gl_rec->paradigm_menu);
				RW = reps-1;
				return(pr_get_trial());
			}				
			/* try to get a trial from the task */
			if((!cps && (GTF(0) || (--(TPW) > 0 && GTF(1)))) ||
				( cps && GTF(1))) {

				/* SUCCESS! Set appropriate graphics group */
				dx_set_graphic(gl_rec->dx,
					gl_rec->tasks[gl_rec->task_index]->graphics);

				/* Outta, no error */
				RET(1);
			}

			/* This happens only if we tried to get 
			**		another trial from the given task 
			**		but couldn't -- so remove the task
			*/
			if(TPW > 0) {
				TPV = 0;
				TPW = 0;
			}

			/* try again */
			return(pr_get_trial());
	}
}

/* PUBLIC ROUTINE: pr_set_trial
**
** Sets up the current trial
**
** Returns:
**		0 if error
**		1 if no error
*/
int pr_set_trial(void)
{
	if(gl_rec->trialP != NULL) {
		(*(gl_rec->trialP->task->set_trial_fn))(gl_rec->trialP);
		return(1);
	}

	/* error */
	return(0);
}

/* PUBLIC ROUTINE: pr_start_trial
**
**	Convenience routine to make a single
**		call for get/set trial. Also drops
**		STARTCD/LISTDONECD, as appropriate
**
** Returns:
**		0 if no trials left or error
**		1 if we got a good trial
*/
int pr_start_trial(void)
{
	valtype val;

#define SEND_CODE(cn) if((val=ECV((cn))) != NULLI) \
								ec_send_code(val)
#define SEND_CODET(cn,v) if((val=ECV((cn))) != NULLI) \
								ec_send_code_tagged(val,7000+(v))

	/* conditionally toggle the plexon file using
	**	the "fancy_ttl" ... byte (port) 4 of the dio board 
	** I'm also assuming this needs ~1 msec to settle
	*/
#ifdef PLEX_ECODES
	PR_DIO_ON("Plexon_toggle_bit");
	timer_pause(1.0);
#endif

	/* Try to get next trial ... */
	if(!pr_get_trial()) {

		/* end of list ... drop code */
		SEND_CODE("List_done_code");
		return(0);
	}

	/* drop start, task id, and trial id codes */
	SEND_CODE("Start_code");
	SEND_CODET("Task_id_code",gl_rec->trialP->task->id);
	SEND_CODET("Trial_id_code",gl_rec->trialP->id);

	/* If successful, try to set it ... */
	if(!pr_set_trial()) {

		printf("pr_start_trial: Could not set trial\n");
		return(0);
	}

	/* Give some feedback */
	printf("-> Next trial: TASK id %d, TRIAL id %d ...\n",
		gl_rec->trialP->task->id, gl_rec->trialP->id);

	/* Outta, no error */
	return(1);
}

/* PUBLIC ROUTINE: pr_score_trial
**
** "Score" a trial -- give it a numerical score, and that's it...
**	This is the basic version, see others below
*/
int pr_score_trial(long score, long reset_flag, long blank_flag)
{
   valtype val;

   /* possibly blank the screen */
   if(blank_flag)
      dx_blank(0, ECV("All_off_code"));

   /* set current trial score */
   pr_trial_set_pmf_score(gl_rec->trialP, score);

   /* drop standard codes */
#define DCODE(kv,cv) if(score==(kv)&&((val=ECV((cv)))>0)) \
      ec_send_code_lo(val)
   DCODE(kCorrect,   "Correct");
   DCODE(kError,     "Error");
   DCODE(kBrFix,     "Broken_fixation");
   DCODE(kNC,        "No_choice");

   if(score != kNoFix) {

      /* display current score info */
      printf("-> Score = %d.\n", score);

      /* display all score (pmf) info */
      pr_print_pmf();
   }

   /* reset if necessary */
   if(reset_flag) {
      abort_s(-1);   /* NOTE: these commands CALL ABORT LIST and  */
      again();       /*       RESET ALL STATES                    */
   }

   /* outta */
   return(0);
}

/* PUBLIC ROUTINE: pr_score_trialPRC
**
**	Score a trial, providing a proabilitistic reward count ("PRC").
** Uses global gl_reward_count.
**
*/
int pr_score_trial_prob_reward_count(long score, long reward_count, long no_reward_prob,
		long run_length, long reset_flag, long blank_flag)
{
 	int rn;
	valtype val;

	/* possibly blank the screen */
	if(blank_flag)
		dx_blank(0, ECV("All_off_code"));

	/* Compute actual reward count, using "reward_count" as follows:
	** 	if negative .. use as flag for specific reward schedules
	**		else 			.. specifies number of rewards
	*/
	if(DIV("Reward_bit") < 0) {
		gl_reward_count = 0;

	} else if(reward_count == -1) {
		rn = TOY_RAND(100);		
		if			(rn <  5)	gl_reward_count = 0;
		else if	(rn < 75)	gl_reward_count = 1;
		else if	(rn < 90)	gl_reward_count = 2;
		else 						gl_reward_count = 3;

	} else if(reward_count == -2) {
		rn = TOY_RAND(100);		
		if			(rn <  5)	gl_reward_count = 0;
		else if	(rn < 25)	gl_reward_count = 1;
		else if	(rn < 80)	gl_reward_count = 2;
		else 						gl_reward_count = 3;

	} else if(reward_count <= -3) {

		/* Increment reward count on runs of length "run_length"
		**		of the given score, up to abs(reward_count).
		** Note that run_length is ACROSS tasks; that is, 
		** 	number in a row independent of task.
		*/
		if(gl_rec->pmf->last_score == score)
			gl_reward_count = TOY_MIN(abs(reward_count), 
				1+floor(gl_rec->pmf->run_length / run_length));
		else
			gl_reward_count = 1;

	} else {
		
		/* positive # means fixed # of rewards */
		gl_reward_count = reward_count;
	}

	/* possibly give no reward */
	if(gl_reward_count > 0 && TOY_RCMP(no_reward_prob))
		gl_reward_count = 0;

	/* set current trial score */
	pr_trial_set_pmf_score(gl_rec->trialP, score);

	/* drop standard codes */
#define DCODE(kv,cv) if(score==(kv)&&((val=ECV((cv)))>0)) \
		ec_send_code_lo(val)
	DCODE(kCorrect, 	"Correct");
	DCODE(kError,		"Error");
	DCODE(kBrFix,		"Broken_fixation");
	DCODE(kNC,			"No_choice");

	if(score != kNoFix) {

		/* display current score info */
		printf("-> Score = %d, reward = %d.\n", score, reward_count);

		/* display all score (pmf) info */
		pr_print_pmf();
	}

	/* reset if necessary */
	if(reset_flag) {
		abort_s(-1);	/* NOTE: these commands CALL ABORT LIST and 	*/
		again();     	/* 		RESET ALL STATES							*/
	}

	/* outta */
	return(0);
}

/* PUBLIC ROUTINE: pr_score_trialRT
** modified from pr_score_trial, Long Ding 9-13-07
** "Score" a trial -- give it a numerical score and thats it..
*/
int pr_score_trialRT(long score, long reset_flag, long blank_flag, long rt)
{
   valtype val;

   /* possibly blank the screen */
   if(blank_flag)
      dx_blank(0, ECV("All_off_code"));

   /* set current trial score */
   pr_trial_set_pmf_scoreRT(gl_rec->trialP, score, rt);

   /* drop standard codes */
#define DCODE(kv,cv) if(score==(kv)&&((val=ECV((cv)))>0)) \
      ec_send_code_lo(val)
   DCODE(kCorrect,   "Correct");
   DCODE(kError,     "Error");
   DCODE(kBrFix,     "Broken_fixation");
   DCODE(kNC,        "No_choice");

   if(score != kNoFix) {

      /* display current score info */
      printf("-> Score = %d.\n", score);

      /* display all score (pmf) info */
      pr_print_pmf();
   }

   /* reset if necessary */
   if(reset_flag) {
      abort_s(-1);   /* NOTE: these commands CALL ABORT LIST and  */
      again();       /*       RESET ALL STATES                    */
   }

   /* outta */
   return(0);
}

/* PUBLIC ROUTINE: pr_set_reward
** sets the reward and beep parameters 
** created by sharath 07/05/07
*/
int pr_set_reward(int reward_num, long reward_on_time, long reward_off_time,
						  int beep_num, long beep_on_time,   long beep_off_time)
{
	int rn;

	/* Figure out number of rewards & beeps */
	if(!gl_reward_count) {

		/* Compute actual reward count, using "reward_count" as follows:
		** 	if negative .. use as flag for specific reward schedules
		**		else 			.. specifies number of rewards
		*/
		
		if(DIV("Reward_bit") < 0) {
		gl_reward_count = 0;

		} else if(reward_num == -1) {
			rn = TOY_RAND(100);		
			if			(rn <  5)	gl_reward_count = 0;
			else if	(rn < 75)	gl_reward_count = 1;
			else if	(rn < 90)	gl_reward_count = 2;
			else 						gl_reward_count = 3;

		} else if(reward_num == -2) {
			rn = TOY_RAND(100);		
			if			(rn <  5)	gl_reward_count = 0;
			else if	(rn < 25)	gl_reward_count = 1;
			else if	(rn < 80)	gl_reward_count = 2;
			else 						gl_reward_count = 3;

		} else if(reward_num <= -3) {

			/* Increment reward count on runs of length of the 
			** last score, up to abs(reward_num).
			*/
		
			if(gl_rec->pmf->run_length <= abs(reward_num))
				gl_reward_count = gl_rec->pmf->run_length;
			else	
				gl_reward_count = abs(reward_num);
					
		} else {
		
			/* positive # means fixed # of rewards */
			gl_reward_count = reward_num;
		}

		/* other reward parameters */
		gl_reward_times[0] = reward_on_time;  // first reward duration
		gl_reward_times[1] = reward_off_time; // inter reward interval

		/* then set beep args */ 
		if(beep_num<0) {
			gl_beep_count = gl_reward_count;	
			gl_beep_times[0] = gl_reward_times[0];	
			gl_beep_times[1] = gl_reward_times[1];
		} else {
			gl_beep_count = beep_num;	
			gl_beep_times[0] = beep_on_time;	
			gl_beep_times[1] = beep_off_time;
		}
	}

	/* store the reward info in pmf */	
	pr_trial_set_pmf_reward(gl_rec->trialP, gl_reward_count);

	/* send ecode for reward on time */
	if(gl_reward_times[0]<2000) {
		ec_send_code_tagged(I_REWLENCD,6000+(gl_reward_times[0]));
	} else {
		printf("Not sending I_REWLENCD as reward_on_time >=2000\n");
	}

	/* outta */
	return(0);
}

/* PUBLIC ROUTINE: pr_set_reward2
 ** sets the reward and beep parameters
 ** developed by taka from pr_set_reward 05-18-2015
 ** additional reward pulses can be shorter or longer than the initial pulse
 **
 ** reward_on_time:  initial reward duration
 ** reward_off_time: inter pulse interval
 ** reward_on_time2: reward duration after initial pulse
 */
int pr_set_reward2(int reward_num, long reward_on_time, long reward_off_time, long reward_on_time2,
                  int beep_num,   long beep_on_time,   long beep_off_time,   long beep_on_time2)
{
	int rn;
    
	/* Figure out number of rewards & beeps */
	if(!gl_reward_count) {
        
		/* Compute actual reward count, using "reward_count" as follows:
         ** 	if negative .. use as flag for specific reward schedules
         **		else 			.. specifies number of rewards
         */
		
		if(DIV("Reward_bit") < 0) {
            gl_reward_count = 0;
            
		} else if(reward_num == -1) {
			rn = TOY_RAND(100);
			if		(rn <  5)	gl_reward_count = 0;
			else if	(rn < 75)	gl_reward_count = 1;
			else if	(rn < 90)	gl_reward_count = 2;
			else 						gl_reward_count = 3;
            
		} else if(reward_num == -2) {
			rn = TOY_RAND(100);
			if		(rn <  5)	gl_reward_count = 0;
			else if	(rn < 25)	gl_reward_count = 1;
			else if	(rn < 80)	gl_reward_count = 2;
			else 						gl_reward_count = 3;
            
		} else if(reward_num <= -3) {
            
			/* Increment reward count on runs of length of the
             ** last score, up to abs(reward_num).
             */
            
			if(gl_rec->pmf->run_length <= abs(reward_num))
				gl_reward_count = gl_rec->pmf->run_length;
			else
				gl_reward_count = abs(reward_num);
            
		} else {
            
			/* positive # means fixed # of rewards */
			gl_reward_count = reward_num;
		}
        
		/* other reward parameters */
		gl_reward_times2[0] = reward_on_time;  // first reward duration
		gl_reward_times2[1] = reward_off_time; // inter reward interval
      gl_reward_times2[2] = reward_on_time2; // additional reward duration
      /* printf("%d\n",gl_reward_times[2]); */ 
 
		/* then set beep args */
		if(beep_num<0) {
            gl_beep_count    = gl_reward_count;
            gl_beep_times2[0] = gl_reward_times2[0];
            gl_beep_times2[1] = gl_reward_times2[1];
            gl_beep_times2[2] = gl_reward_times2[2];
            
		} else {
            gl_beep_count    = beep_num;
            gl_beep_times2[0] = beep_on_time;
            gl_beep_times2[1] = beep_off_time;
            gl_beep_times2[2] = beep_on_time2;
		}
	}
    
	/* store the reward info in pmf */
	pr_trial_set_pmf_reward(gl_rec->trialP, gl_reward_count);
    
	/* send ecode for reward on time */
	if(gl_reward_times2[0]<2000) {
        ec_send_code_tagged(I_REWLENCD,6000+(gl_reward_times2[0]));
	} else {
        printf("Not sending I_REWLENCD as reward_on_time >=2000\n");
	}
    
	/* outta */
	return(0);
}

/* PUBLIC ROUTINE: pr_beep_reward
** control routine for pr_give_reward and pr_give_beep
*/
int pr_beep_reward(void)
{
	static int reward = 0;
	static int beep   = 0;

	if(!reward || !beep) {
	      reward = pr_give_reward();
	      beep   = pr_give_beep();
         return(1);
	} else {
        reward = 0;
		beep   = 0;	
		return(0);
	}
}

/* PUBLIC ROUTINE: pr_beep_reward2
 ** Control routine for pr_give_reward2 and pr_give_beep2, 
 ** 	which deliver shorter rewards after the initial pulse
 ** 	TD, 2015-05-18
 */
int pr_beep_reward2(void)
{
	static int reward = 0;
	static int beep   = 0;
    
	/* if (reward==1)
	** printf("reward=%d / beep=%d, on=%d, off=%d, current=%ld\n",
	**		reward,beep,gl_reward_times[0], gl_reward_times[1],
	**		timer_tocR(1));
	*/
	
	if(!reward || !beep) {
        reward = pr_give_reward2();
        beep   = pr_give_beep2();
        return(1);
	} else {
        reward = 0;
			beep  = 0;
			return(0);
	}
}

/* PUBLIC ROUTINE: pr_give_reward
** controls reward on & off
**
** TD, 2013-06-13   
** send dio codes to signal reward off and beep off.
*/
int pr_give_reward(void)
{
   static 	long 	giving=-1;
	long		wait;
   valtype  reward_off_bit;
	
   /* "giving" specifies whether or not
   **    we are in the middle of giving a
   **    reward:
   **    -1 initial condition
   **     0 no
   **     1 yes
   */
   if(giving == -1 && !gl_reward_count) {
      /* do nothing */
      return(1);

   } else if(giving==1) {
      /* do nothing */
      /* We are in the middle of giving a reward...
      ** first check "on time" timer
      */
      // Waiting for a certain duration specified from reward_on_time (Taka 09-30-2011)
      if(timer_tocR(1) < gl_reward_times[0]) {
         /* still going... */
         return(0);
      }

      /* ***
      ** Reward_system_flag should be 0 for Crist's reward feeder and 1 for the other one (used at Rig 1).
      ** You can change it from the preference menu.
      ** If you use 1 for Crist's feeder, you will get a strange A/D error, and Rex will crash.
      ** TD 09-29-2011
		*/
		if(PRV("Reward_system_flag")==1) {
         PR_DIO_ON("Reward_bit"); 		//	[DEBUG]
			timer_tic(3);                	// Using 3rd timer, TD  09-29-2011
         while(timer_tocR(3) < 5)
   			wait = timer_tocR(3);
	   }
      /* done giving reward */
      PR_DIO_OFF("Reward_bit"); 			// [DEBUG]
      if( (reward_off_bit=DIV("Reward_off_bit")) >= 0 ) // TD, 2013-06-12
          ec_send_dio(reward_off_bit);  

      /* check for end of the line */
      if(!gl_reward_count) {
         giving = -1;
         return(1);
      }

      /* otherwise start the inter-reward
      ** timer and outta
      */
      giving = 0;
      timer_tic(1);
      return(0);

   } else {
      /* We are not in the middle of giving a reward...
      ** first check "off time" timer
      */
      if(giving != -1 &&
         timer_tocR(1) < gl_reward_times[1]) {

         /* still going... */
         return(0);
      }

      /* start giving reward */
      PR_DIO_ON("Reward_bit"); 
		
		if(PRV("Reward_system_flag")==1) {
      	timer_tic(3);
	   	do { wait = timer_tocR(3);	
	   	} while(timer_tocR(3) < 5);
         PR_DIO_OFF("Reward_bit"); //[DEBUG]
	  	}

		/* decrement reward counter, start
      ** reward timer, mark as
      ** "giving" and outta with continue flag
      */
      gl_reward_count--;
      giving = 1;
      timer_tic(1);
      return(0);
   }
}

/* PUBLIC ROUTINE: pr_give_reward2
 **
 ** Controls reward on & off, and send codes to signal reward off and beep off.
 **
 ** When executed from training.d the following are the values within a single trial:
 ** 	[giving = -1, gl_reward_count = 1]
 ** 	[giving =  1, gl_reward_count = 0]
 ** 	[giving =  1, gl_reward_count = 0]
 ** 	....
 **
 ** Developed from pr_give_reward by TD on 2015-05-18 for shorter additional reward/beep
 */
int pr_give_reward2(void)
{
	/* "giving" specifies whether or not we are in the middle of giving a reward:
	**    -1 initial condition
	**    0 	no
	**	   1 	yes
	**
	** first:
	**  	1 means first reward within a trial
	**  	0 means second, third, ... reward
	*/
    static long giving=-1, first=1;
    long	    	   wait, rewdur=0;
    valtype       tmpdv;
    
    /****************** [1] ********************/
    if(giving == -1 && !gl_reward_count) {
        /* do nothing */
        return(1);

    /****************** [2] ********************/
    // reward delivery was initiated in [3] and the process enteres here.
    // reward is being delivered
    } else if(giving==1) {
		// switch rewrad duration for the initial and subsequent pulses
		if(first==1) {
			rewdur = gl_reward_times2[0]; // base amount of reward
		} else if(first==0) {
			rewdur = gl_reward_times2[2]; // additional amount of reward
		}
      
		// printf("first, %d, dur, %d\n", first, rewdur);
		// printf("%d\n", gl_reward_times2[2]);
        
		// Waiting for a certain duration before tuning off the reward valve
		if(timer_tocR(1) < rewdur) {
			/* still going... */
			return(0);
		}
        
		/* Stop giving the reward */
		///////////////////////////////////////////////////////
		// Reward_system_flag should be 0 for Clist's reward feeder adn 1 for the other one (used at Rig 1).
		// You can change it from preference.
		// If you use 1 for Clist's feeder, you will get a strange A/D error, and Rex will crash.
		// (Taka 09-29-2011)
		if(PRV("Reward_system_flag")==1) {
			PR_DIO_ON("Reward_bit"); //[DEBUG]
			timer_tic(3);                // Using 3rd timer (Taka 09-29-2011)
			while(timer_tocR(3) < 5)
			wait = timer_tocR(3);
		}

		// done giving reward
		PR_DIO_OFF("Reward_bit");
		if( (tmpdv=DIV("Reward_off_bit")) >= 0 ) // TD, 2013-06-12
			ec_send_dio(tmpdv);
		////////////////////////////////////////////////////////
       
		/* check for end of the line */
		if(!gl_reward_count) {
			giving = -1;
			first  =  1; // next time will be the first time in that trial
			return(1);
		}
       
		/* otherwise start the inter-reward
		** timer and outta
		*/
		giving = 0;
		first  = 0;
		timer_tic(1); // set timer for inter-reward interval used in [3]
		return(0);

    /****************** [3] ********************/
    // Initial condition within a trial
    // "giving" is either 0 or -1
    } else {
		/* We are not in the middle of giving a reward...  check "off time" timer
		**  To make sure we wait a certain duration before opening the reward valve again
		*/
		if(giving != -1 && timer_tocR(1) < gl_reward_times2[1]) {
            
			/* still going... */
			return(0);
		}
        
		/* start to give reward */
		//////////////////////////////////////////////////////
		PR_DIO_ON("Reward_bit");
		
		if(PRV("Reward_system_flag")==1) {
			timer_tic(3);
			do { wait = timer_tocR(3);
			} while(timer_tocR(3) < 5);
			PR_DIO_OFF("Reward_bit"); //[DEBUG]
	 	}
		//////////////////////////////////////////////////////
        
		/* decrement reward counter, start
		** reward timer, mark as
		** "giving" and outta with continue flag
		*/
		gl_reward_count--;
		giving = 1;   // giving reward.
		timer_tic(1); // update timer to measure rewrad duration
		return(0);
	}
}

/* PUBLIC ROUTINE: pr_give_beep
** controls beeps
**
** TD, 2013-06-13   
** send dio codes to signal reward off and beep off.
*/
int pr_give_beep(void)
{
   static long beeping=-1;
   valtype tmpdv;

   /* "beeping" specifies whether or not
   **    we are in the middle of giving a
   **    beep:
   **    -1 initial condition
   **     0 no
   **     1 yes
   */
   if(beeping == -1 && !gl_beep_count) {

      /* do nothing */
      return(1);

   } else if(beeping==1) {

      /* We are in the middle of giving a reward...
      ** first check "on time" timer
      */
      if(timer_tocR(2) <  gl_beep_times[0]) {

         /* still going... */
         return(0);
      }

      /* done giving beep */
      PR_DIO_OFF("Beep_bit");
      if( (tmpdv=DIV("Beep_off_bit")) >= 0 ) // TD 2013-06-12
          ec_send_dio(tmpdv);  

      /* check for end of the line */
      if(!gl_beep_count) {
         beeping = -1;
         return(1);
      }

      /* otherwise start the inter-beep
      ** timer and outta
      */
      beeping = 0;
      timer_tic(2);
      return(0);

   } else {

      /* We are not in the middle of beeping
      ** first check "off time" timer
      */
      if(beeping != -1 &&
         timer_tocR(2) < gl_beep_times[1]) {

         /* still going... */
         return(0);
      }

      /* start beeping */
      PR_DIO_ON("Beep_bit");

      /* decrement beep counter, start
      ** beep timer, mark as
      ** "beeping" and outta with continue flag
      */
      gl_beep_count--;
      beeping = 1;
      timer_tic(2);
      return(0);
   }
}

/* PUBLIC ROUTINE: pr_give_beep2
 ** controls beeps
 **
 ** TD, 2013-06-13
 ** send dio codes to signal reward off and beep off.
 **
 ** developed from pr_give_reward by Taka on 2015-05-18 for shorter additional reward/beep
 */
int pr_give_beep2(void)
{
    static long beeping = -1;
    static long first   =  1;
    long        beepdur;
    valtype tmpdv;
    
    /* "beeping" specifies whether or not
     **    we are in the middle of giving a
     **    beep:
     **    -1 initial condition
     **     0 no
     **     1 yes
     */
    /****************** [1] ********************/
    if(beeping == -1 && !gl_beep_count) {
        /* do nothing */
        return(1);
        
    /****************** [2] ********************/
    } else if(beeping==1) {
        // switch beep duration for the initial and subsequent pulses
        if(first==1){
            beepdur = gl_beep_times2[0]; // base amount of reward
        }else if(first==0){
            beepdur = gl_beep_times2[2]; // additional amount of reward
        }

        /* We are in the middle of giving a reward...
         ** first check "on time" timer
         */
        if(timer_tocR(2) <  beepdur) {
            /* still going... */
            return(0);
        }
        
        /* done giving beep */
        PR_DIO_OFF("Beep_bit");
        if( (tmpdv=DIV("Beep_off_bit")) >= 0 ) // TD 2013-06-12
            ec_send_dio(tmpdv);
        
        /* check for end of the line */
        if(!gl_beep_count) {
            beeping = -1;
            first  =  1; // next call of the function will be in next trial
            return(1);
        }
        
        /* otherwise start the inter-beep
         ** timer and outta
         */
        beeping = 0;
        first   = 0;
        timer_tic(2);
        return(0);

    /****************** [3] ********************/
    } else {
        
        /* We are not in the middle of beeping
         ** first check "off time" timer
         */
        if(beeping != -1 &&
           timer_tocR(2) < gl_beep_times2[1]) {
            
            /* still going... */
            return(0);
        }
        
        /* start beeping */
        PR_DIO_ON("Beep_bit");
        
        /* decrement beep counter, start
         ** beep timer, mark as
         ** "beeping" and outta with continue flag
         */
        gl_beep_count--;
        beeping = 1;
        timer_tic(2);
        return(0);
    }
}

/* PUBLIC ROUTINE: pr_finish_trial
**
** Convenience routine to handle any clean-up at
**		the very end of the trial.
*/
int pr_finish_trial(void)
{
	/* Conditionally toggle the plexon file using
	**	the "fancy_ttl" ... byte (port) 4 of the dio board 
	** Not worrying about settle time (since no ecodes will
	** be sent right after this, unlike when we turned it on).
	**
	** jig added 4/10/07 -- be sure to flush the queues first
	*/
   //printf("DEBUG: pr_finish_trial is entered.\n");

#ifdef PLEX_ECODES

	/* flush the queues,
	** HIGH PRIORITY first
	*/
	while(i_b->pl_hidx != i_b->pl_hilx) {
	
		/* check status */
		if(pl_box_ready_) {

			/* send code and increment pointer */
			pl_send_code_(i_b->pl_hipri[i_b->pl_hidx]);	
   		if(++i_b->pl_hidx == PL_MAXCODES)
      		i_b->pl_hidx=0;
		}
	}

	/* now LOW PRIORITY
	*/
	while(i_b->pl_lodx != i_b->pl_lolx) {

		/* check status */
		if(pl_box_ready_) {

			/* send code and increment pointer */
			pl_send_code_(i_b->pl_lopri[i_b->pl_lodx]);
			if(++i_b->pl_lodx == PL_MAXCODES) 
				i_b->pl_lodx= 0;
     }
  }

	/* make sure last one got sent */
	while(!pl_box_ready_) ;	
	
	PR_DIO_OFF("Plexon_toggle_bit");
#endif

	/* dump buffered udp commands to file ... send the 
	**		current trial number (which was already incremented)
	*/
	if(PRV("Dump_commands") && gl_rec->pmf	&& gl_rec->pmf->started > 0)
		dx_dump(gl_rec->pmf->started - 1);
	else
		dx_dump(-1);

	/* outta */
	return(0);
}

/* PUBLIC ROUTINE: pr_toggle_file
**
** Convenience routine to handle any clean-up at
**		the very end of the trial.
*/
int pr_toggle_file(int onoff)
{
	valtype dv;

	if((dv=DIV("Plexon_toggle_bit"))>=0) 
		if(onoff == 1)
			dio_on(PR_DIO_ID(dv));
		else
			dio_off(PR_DIO_ID(dv));

	return(0);
}

	/* RECORD 		*/

/* PUBLIC ROUTINE: pr_rec_init
**
**	Initialize a _PRrecord_struct.
**
**	NOTE: this routine also initializes
**		rec->pmf and rec->dx (see below).
**
** Arguments:
**		int pmf_scores, pmf_history ... arguments needed
**				for pr_pmf_init
*/
_PRrecord pr_rec_init(int pmf_scores, int pmf_history)
{
	_PRrecord rec;

	/* clear gl_rec (shouldn't happen) */
	if(gl_rec != NULL) {
		printf("pr_rec_init: gl_rec already exists!!!\n");
		return(gl_rec);
	}

	/* Allocate the new _PRrecord_struct */
	rec = SAFE_STALLOC(_PRrecord_struct);

	/* add the pmf struct */
	rec->pmf 			 = pr_pmf_init(pmf_scores, pmf_history);

	rec->paradigm_menu = NULL;
	rec->prefs_menu	 = NULL;
	rec->clut_menu	 	 = NULL;
	rec->rt_vars 		 = NULL;

	/* add the dx (dotsX) struct */
	rec->dx				 = dx_init();

	/* no tasks yet */
	rec->tasks 			 = NULL;
	rec->tasks_length  = 0;
	rec->task_index 	 = 0;

	/* no trials yet */
	rec->trialP 		 = NULL;

	return(rec);

}

/* PUBLIC ROUTINE: pr_rec_add_task
**
**	Add a task to the given rec
**
** Arguments:
**		_PRrecord rec	... the rec
**		_PRtask	 task ... the task
**		int 		 id 	... the id. duh.
*/
void pr_rec_add_task(_PRrecord rec, _PRtask task, int id)
{
	if(!rec || !task)
		return;
	
	/* set the id */
	task->id = id;

	/* make the task point back to the rec */
	task->rec = rec;

	/* set up the pmf */
	task->pmf = pr_pmf_copy(rec->pmf);

	/* allocate a new pointer in the array */
	if(!rec->tasks_length)
		rec->tasks = (_PRtask *) SAFE_MALLOC(sizeof(_PRtask));
	else
		rec->tasks = (_PRtask *) SAFE_REALLOC(rec->tasks,
							((rec->tasks_length + 1) * sizeof(_PRtask)));

	/* set it and update the count */
	rec->tasks[rec->tasks_length++] = task;
}

/* PUBLIC ROUTINE: pr_rec_get_task_first
**
**	Gets the index of the first task in rec
**		with WORKING COPY of "Proportion_T*"
**		property in rec menu > 0
*/
int pr_rec_get_task_first(_PRrecord rec)
{
	register int i;

	if(!rec || !rec->tasks)
		return(-1);

	for(i=0;i<rec->tasks_length;i++)
		if(PL_L2PW(rec->paradigm_menu, PIND+i))
			return(i);

	return(-1);
}

/* PUBLIC ROUTINE: pr_rec_get_task_random
**
**	Gets the index of a task in rec, using a
**		random pick weighted by the 
**		WORKING COPY of "Proportion_T*"
**		property in rec menu.
**
**	Returns the index (0 -> tasks_length-1), or
**		-1 if none found.
*/
int pr_rec_get_task_random(_PRrecord rec)
{
	register int i;
	int total = 0, value;

	if(!rec || !rec->tasks)
		return(-1);

	/* count the total proportions */
	for(i=0;i<rec->tasks_length;i++)
		total += PL_L2PW(rec->paradigm_menu, PIND+i);

	/* none found */
	if(total <= 0)
		return(-1);

	/* pick a random number, weighted by the proportions */
	value = TOY_RAND(total);

	/* find the associated index */
	total = 0;
	i		= -1;
	while(total <= value)
		total += PL_L2PW(rec->paradigm_menu, PIND+(++i));

	return(i);
}

/* PUBLIC ROUTINE: pr_rec_print
**
** Prints info for a rec
**
**	Arguments:
**		_PRrecord rec
*/
void pr_rec_print(_PRrecord rec)
{
	register int i;

	if(!rec) {
		printf("**NULL REC**\n");
		return;
	}

	printf("**INFO** REC: %d tasks (ci = %d)", 
			rec->tasks_length, rec->task_index);
	if(rec->trialP)
		printf(", cur trial id = %d\n", 
			rec->trialP->id);
	else
		printf("\n");	

/*
**	pl_list_print(rec->paradigm_menu);
*/

/*
**	dx_print(rec->dx);
*/

	/* Task info */
	for(i=0;i<rec->tasks_length;i++)
		pr_task_print(rec->tasks[i]);
}

/* PUBLIC ROUTINE: pr_rec_clear
**
** Clears all of the tasks in a rec. 
**
**	Arguments:
**		_PRrecord rec
**		int reset_flag ... 0	if do NOT clear pmf
**								 1 if DO clear pmf
*/
void pr_rec_clear(_PRrecord rec, int clear_pmf)
{
	register int i;

	if(!rec || !rec->tasks)
		return;

	/* loop through the tasks */
	for(i=0;i<rec->tasks_length;i++)
		pr_task_clear(rec->tasks[i]);	

	/* reset the current index */
	rec->task_index = 0;

	/* clear the "current trial" pointer */
	rec->trialP = NULL;

	/* conditionally clear the pmf */
	if(clear_pmf)
		pr_pmf_clear(rec->pmf);
}

/* PUBLIC ROUTINE: pr_rec_free
**
** Frees all memory associated with the given rec.
*/
void pr_rec_free(_PRrecord rec)
{
	register int i;

	if(!rec)
		return;

	/* free the tasks */
	for(i=0;i<rec->tasks_length;i++)
		pr_task_free(rec->tasks[i]);
	SAFE_FREE(rec->tasks);

	/* free the rest */
	pr_pmf_free (rec->pmf);
	pl_list_free(rec->paradigm_menu);
	pl_list_free(rec->prefs_menu);
	pl_list_free(rec->clut_menu);
	pl_list_free(rec->rt_vars);
	dx_free(rec->dx);

	/* free the rec */
	SAFE_FREE(rec);

	/* free the global vlist pointer array */
	for(i=0;i<PR_MAX_MENUS;i++)
		SAFE_FREE(gl_vlistPs[i]);
}

   /* TASK        */

/* PUBLIC ROUTINE: pr_task_init
**
**	Initialize an empty _PRtask
*/
_PRtask pr_task_init(void)
{

	_PRtask task = SAFE_STALLOC(_PRtask_struct);

	task->id 				= 0;

	task->task_menus 		= NULL;
	task->graphics 		= NULL;
	task->pmf 				= NULL;

	task->rec 				= NULL;

	task->trials 			= NULL;
	task->trials_length 	= 0;
	task->trials_rows 	= 0;
	task->trials_columns = 0;
	task->trials_index 	= 0;

	task->trialPs 			= NULL;
	task->trialPs_length = NULL;
	task->trialPs_index 	= NULL;

	task->user_data 		= NULL;

	task->make_trials_fn	= NULL;
	task->get_trial_fn 	= NULL;
	task->set_trial_fn 	= NULL;
	task->clear_user_data_fn = NULL;

	return(task);
}


/* PUBLIC ROUTINE: pr_task_get_trial1D
**
** Get a pointer to a trial from a 1D array
*/
_PRtrial pr_task_get_trial1D(_PRtask task, int index)
{
   if(!task || !task->trials || index < 0 || index >= task->trials_length)
      return(NULL);

   return(task->trials[index]);
}

/* PUBLIC ROUTINE: pr_task_get_trial2D
**
** Get a pointer to a trial from a 2D array
*/
_PRtrial pr_task_get_trial2D(_PRtask task, int row, int column)
{
   if(!task || !task->trials || row < 0 || column < 0 || row*column >= task->trials_length)
      return(NULL);

   return(task->trials[row*task->trials_columns+column]);
}

/* PUBLIC ROUTINE: pr_task_add_trials1D
**
**	Add a one-dimensional array of trials
**		to a task.
**	
**	Arguments:
**		task
**		number of trials
*/
void pr_task_add_trials1D(_PRtask task, int num)
{
	register int i;
	_PRtrial trial;

	/* check for task */	
	if(!task)
		return;

	/* remove the old trials */
	if(task->trials)
		pr_task_clear(task);

	/* check if any to add */
	if(num < 1)
		return;

	/* Make array of trials (pointers),
	** 	then loop through and make the structs
 	*/
	task->trials = SAFE_ZALLOC(_PRtrial, num);

	for(i=0;i<num;i++) {

		/* make the trial struct */		
		trial = pr_trial_init();

		/* ID is task ID * 100 + i */
		trial->id = task->id*100+i;

		/* point back to this task */
		trial->task = task;

		/* add the pmf */
		trial->pmf  = pr_pmf_copy(task->pmf);

		/* add to the array */
		task->trials[i] = trial;
	}

	/* update the index, etc */
	task->trials_length  = num;
	task->trials_rows    = num;
	task->trials_columns = 1;
	task->trials_index 	= 0;
}

/* PUBLIC ROUTINE: pr_task_add_trials2D
**
**	Add a two-dimensional array of trials
**		to a task.
**	
**	Arguments:
**		task
**		number of trials
**		clear pmf flag
*/
void pr_task_add_trials2D(_PRtask task, int rows, int columns)
{

	if(!task)
		return;

	/* can't have a dimension < 1 */
	if(rows < 1)
		rows = 1;
	if(columns < 1)
		columns = 1;

	/* add one big row-by-row vector */
	pr_task_add_trials1D(task, rows*columns);

	/* update the indices */
	task->trials_rows 	= rows;
	task->trials_columns = columns;
}

/* PUBLIC ROUTINE: pr_task_make_trialP_all
**
** Makes an array of pointers to ALL trials
**		within a task, in order.
**
** Arguments:
**		_PRtask task	 ... the task (pointer to struct)
**		int num_blocks	 ... # of blocks of trials
**				to use. This makes sure that redoing
**				trials (e.g., after broken fixation) doesn't
**				cause a predictable set to accumulate at
**				the end of the list
*/
void pr_task_make_trialP_all(_PRtask task, int num_blocks)
{
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

	/* Allocate the array of pointers */
	total = task->trials_length * num_blocks;
	task->trialPs = SAFE_ZALLOC(_PRtrial, total);

	/* fill the array with repeated blocks of pointers 
	** to trials, in order
	*/
	for(k=0,i=0;i<num_blocks;i++)
		for(j=0;j<task->trials_length;j++)
			task->trialPs[k++] = task->trials[j];

	task->trialPs_length = total;
	task->trialPs_index  = 0;
}

/* PUBLIC ROUTINE: pr_task_make_trialP_allR
**
** Makes an array of pointers to ALL trials
**		within a task, in randomized order.
**
** Arguments:
**		_PRtask			 ... the task
**		int num_blocks	 ... # of blocks of trials
**				to use. This makes sure that redoing
**				trials (e.g., after broken fixation) doesn't
**				cause a predictable set to accumulate at
**				the end of the list
*/
void pr_task_make_trialP_allR(_PRtask task, int num_blocks)
{
	pr_task_make_trialP_all(task, num_blocks);
	pr_task_randomize_trialP(task);
}

/* PUBLIC ROUTINE: pr_task_make_trialP_row
**
** Makes an array of pointers to a ROW of trials
**		within a task, in order.
**
** Arguments:
**		_PRtask task ... the task
**		int	  row  ... the row to use
**		int num_blocks	 ... # of blocks of trials
**				to use. This makes sure that redoing
**				trials (e.g., after broken fixation) doesn't
**				cause a predictable set to accumulate at
**				the end of the list
*/
void pr_task_make_trialP_row(_PRtask task, int row, int num_blocks)
{
	register int i, j, len;
	_PRtrial *tp1, *tp2;

	if(!task)
		return;
	
	/* free the current pointers */
	SAFE_FREE(task->trialPs);
	
	if(!task->trials || row < 0 || row >= task->trials_rows ||
		num_blocks < 1)
		return;

	/* Make an array of pointers, length = num columns
	*/
	len = task->trials_columns;
	tp1 = task->trialPs = SAFE_ZALLOC(_PRtrial, len);
	for(i=num_blocks;i>0;i--) {
		tp2 = &(task->trials[row*len]);
		for(j=len;j>0;j--)
			*tp1++ = *tp2++;
	}

	task->trialPs_length = len;
	task->trialPs_index  = 0;
}

/* PUBLIC ROUTINE: pr_task_make_trialP_rowR
**
** Makes an array of pointers to a ROW of trials
**		within a task, in randomized order.
**
** Arguments:
**		_PRtask task ... the task
**		int	  row  ... the row to use
**		int 	  num_blocks ... # of blocks of trials
**				to use ... this makes sure that redoing
**				trials (e.g., after broken fixation) doesn't
**				cause a predictable set to accumulate at
**				the end of the list
*/
void pr_task_make_trialP_rowR(_PRtask task, int row, int num_blocks)
{
	pr_task_make_trialP_row(task, row, num_blocks);
	pr_task_randomize_trialP(task);
}

/* PUBLIC ROUTINE: pr_task_make_trialP_column
**
** Makes an array of pointers to a COLUMN of trials
**		within a task, in order.
**
** Arguments:
**		_PRtask task 	 ... the task
**		int	  column  ... the column to use
**		int num_blocks	 ... # of blocks of trials
**				to use. This makes sure that redoing
**				trials (e.g., after broken fixation) doesn't
**				cause a predictable set to accumulate at
**				the end of the list
*/
void pr_task_make_trialP_column(_PRtask task, int column, int num_blocks)
{
	register int i, j, len;
	_PRtrial *tp1;

	if(!task)
		return;
	
	/* free the current pointers */
	SAFE_FREE(task->trialPs);
	
	if(!task->trials || column < 0 || 
			column >= task->trials_columns || num_blocks < 1)
		return;

	/* Make an array of pointers, length = num rows
	*/
	len = task->trials_rows;
	tp1 = task->trialPs = SAFE_ZALLOC(_PRtrial, len);
	for(i=num_blocks;i>0;i--) {
		for(j=0;j<len;j++)
			*tp1++ = task->trials[j*task->trials_columns+column];
	}

	task->trialPs_length = len;
	task->trialPs_index  = 0;
}

/* PUBLIC ROUTINE: pr_task_make_trialP_columnR
**
** Makes an array of pointers to a COLUMN of trials
**		within a task, in randomized order.
**
** Arguments:
**		_PRtask task 	 	 ... the task
**		int	  column  	 ... the column to use
**		int 	  num_blocks ... # of blocks of trials
**				to use ... this makes sure that redoing
**				trials (e.g., after broken fixation) doesn't
**				cause a predictable set to accumulate at
**				the end of the list
*/
void pr_task_make_trialP_columnR(_PRtask task, int column, int num_blocks)
{
	pr_task_make_trialP_column(task, column, num_blocks);
	pr_task_randomize_trialP(task);
}

/* PUBLIC ROUTINE: pr_task_randomize_trialP
**
**	Randomize the array of trial pointers in
**		task->trialPs.
**
*/
void pr_task_randomize_trialP(_PRtask task)
{
	register int i,j,k;
	_PRtrial *dummy;

	if(!task || !task->trialPs)
		return;

	/* allocate the dummy array */
	dummy = SAFE_ZALLOC(_PRtrial, task->trialPs_length);
	
	/* Now pick random entries from the dummy
	**  and move into the pointer array 
	*/
	for(k=0,i=task->trialPs_length;i>0;i--) {

		/* get a random number between 0 and i-1 (index) */
		j = TOY_RAND(i);	

		/* copy random index to next entry in dummy array */
		dummy[k++] = task->trialPs[j];

		/* swap out the used pointer */
		task->trialPs[j] = task->trialPs[i-1];
	}

	/* Free the trial pointers array, swap in the dummy */
	SAFE_FREE(task->trialPs);
	task->trialPs = dummy;

	/* reset the index */
	task->trialPs_index = 0;
}
	
/* PUBLIC ROUTINE: pr_task_swap_trialP
**
** Swap the current trial with one later in the array.
**		Typically used after a broken fixation or
**		no-choice trial.
**	
*/
void pr_task_swap_trialP(_PRtask task)
{
	int index;	
	_PRtrial trial;

	if(!task || !task->trialPs ||
		task->trialPs_index == task->trialPs_length - 1)
		return;

	/* Pick a random # to index trials between
	** current index and end
	*/
	index = task->trialPs_index + 1 + 
		TOY_RAND(task->trialPs_length - task->trialPs_index - 1);

	/* swap */
	trial = task->trialPs[task->trialPs_index];
	task->trialPs[task->trialPs_index] = task->trialPs[index];
	task->trialPs[index] 				  = trial;
}

/* PUBLIC ROUTINE: pr_task_print_pmf
**
*/
void pr_task_print_pmf(_PRtask task)
{
	register int i;

	if(!task) {
		printf("  **** NULL TASK ***** \n");
		return;
	}

	printf("  **** TASK (%2d): ", task->id);
	pr_pmf_print(task->pmf);

	for(i=0;i<task->trials_length;i++)
		pr_trial_print_pmf(task->trials[i]);
}

/* PUBLIC ROUTINE: pr_task_print
**
** Print useful info about a task
**	
*/
void pr_task_print(_PRtask task)
{
	register int i;

	if(!task) {
		printf(" ** NULL TASK ** \n");
		return;
	}

	/* task info */
	printf("  ** TASK (%d,%s): %d (%dr x %dc; i=%d) trials; %d (i=%d) trialPs.\n",
		task->id, task->type, task->trials_length, 
		task->trials_rows, task->trials_columns, 
		task->trials_index, task->trialPs_length,
		task->trialPs_index);

	/* print trial info */
	for(i=0;i<task->trials_length;i++)
		pr_trial_print(task->trials[i]);
}

/* PUBLIC ROUTINE: pr_task_clear
**
** Free all the trials in a task, and
**		clear the pmf.
**	
*/
void pr_task_clear(_PRtask task)
{
	register int i;

	if(!task || !task->trials)
		return;

   /* clear the user data */
   if(task->clear_user_data_fn != NULL) {
      (*(task->clear_user_data_fn)) (task);
      task->clear_user_data_fn = NULL;
   }

	/* free the array of trials */
	for(i=0;i<task->trials_length;i++)
		pr_trial_free(task->trials[i]);
	SAFE_FREE(task->trials);
	task->trials_length 	= 0;
	task->trials_rows 	= 0;
	task->trials_columns = 0;
	task->trials_index 	= 0;

	/* free the pointer array */
	SAFE_FREE(task->trialPs);
	task->trialPs_length = 0;
	task->trialPs_index  = 0;

	/* clear the pmf */
	pr_pmf_clear(task->pmf);
}

/* PUBLIC ROUTINE: pr_task_free
**
**	Remember that task->graphics is just
**		a pointer to a struct; rec->graphics
**		is where the unique struct is kept
**		(and free'd)
*/
void pr_task_free(_PRtask task)
{
	if(!task)
		return;

	/* clear the trials */
	pr_task_clear(task);

	/* free the rest */
	pl_group_free(task->task_menus);
	pr_pmf_free  (task->pmf);
}

   /* TRIAL       */

/* PUBLIC ROUTINE: pr_trial_init
**
*/
_PRtrial pr_trial_init()
{
	_PRtrial trial = SAFE_STALLOC(_PRtrial_struct);

	trial->id 			 = 0;
	trial->repetitions = 0;

	trial->user_data 	 = NULL;

	trial->list  		 = NULL;
	trial->pmf			 = NULL;
	trial->task			 = NULL;

	return(trial);
}

/* PUBLIC ROUTINE: pr_trial_set_pmf_score
**
**	Sets the pmf for a given trial,
**		and climbs the hierarchy to set the pmf
**		for the task and rec, as well.
*/
void pr_trial_set_pmf_score(_PRtrial trial, int score)
{
	if(!trial)
		return;
	
	/* set the trial pmf */
	if(trial->pmf)
		pr_pmf_set_score(trial->pmf, score);

	/* set the task pmf */
	if(trial->task && trial->task->pmf)
		pr_pmf_set_score(trial->task->pmf, score);

	/* set the rec pmf and total counts */
	if(trial->task && trial->task->rec && trial->task->rec->pmf)
		pr_pmf_set_score(trial->task->rec->pmf, score);
}

/* PUBLIC ROUTINE: pr_trial_set_pmf_reward
**
**	Sets the pmf for a given trial,
**		and climbs the hierarchy to set the pmf
**		for the task and rec, as well.
*/
void pr_trial_set_pmf_reward(_PRtrial trial, int reward)
{
	if(!trial)
		return;

	/*	
	** printf("pr_trial_set_pmf_reward\n");  
	*/

	/* set the trial pmf */
	if(trial->pmf)
		pr_pmf_set_reward(trial->pmf, reward);

	/* set the task pmf */
	if(trial->task && trial->task->pmf)
		pr_pmf_set_reward(trial->task->pmf, reward);

	/* set the rec pmf and total counts */
	if(trial->task && trial->task->rec && trial->task->rec->pmf)
		pr_pmf_set_reward(trial->task->rec->pmf, reward);
}

/* PUBLIC ROUTINE: pr_trial_set_pmf_scoreRT
** Added 9-13-07, Long Ding, keep track of mean RT
**
*/
void pr_trial_set_pmf_scoreRT(_PRtrial trial, int score, long rt)
{

   int i;
   pr_trial_set_pmf_score(trial, score);

   if (trial->pmf && score == 1)  {
      trial->pmf->rt_mean = (trial->pmf->rt_mean * trial->pmf->rt_len + rt) / (trial->pmf->rt_len + 1);
      trial->pmf->rt_len++;
   }
}

/* PUBLIC ROUTINE: pr_trial_print_pmf
**
*/
void pr_trial_print_pmf(_PRtrial trial)
{
	register int i;

	if(!trial) {
		printf("		 NULL TRIAL\n");
		return;
	}

	if(trial->list) {

		/* print the property values */
		printf("     TR (%2d", trial->id);
		for(i=0;i<trial->list->properties_length;i++)
			printf(",%4d",(int) (trial->list->properties[i]->values[0]));
		printf("): ");

	} else {
		printf("     TR (%3d): ", trial->id);
	}

	pr_pmf_print(trial->pmf);
}

/* PUBLIC ROUTINE: pr_trial_print
**
*/
void pr_trial_print(_PRtrial trial)
{
	register int i;

	if(!trial) {
		printf("		 NULL TRIAL\n");
		return;
	}

	printf("     TR (%2d): ", trial->id);

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

/* PUBLIC ROUTINE: pr_trial_free
**
**	Free the trial.
*/
void pr_trial_free(_PRtrial trial)
{
	if(!trial)
		return;

	/* free stuff */
	pl_list_free(trial->list);
	pr_pmf_free (trial->pmf);

	/* free the user data */
	SAFE_FREE(trial->user_data);

	/* free the trial struct */
	SAFE_FREE(trial);
}

   /* PMF         */

/* PUBLIC ROUTINE: pr_pmf_init
**
** Arguments:
**		int scores  ... number of scores to keep
**		int history ... number of history (trials) to keep
*/
_PRpmf pr_pmf_init(int scores, int history)
{
	register int i;
	_PRpmf pmf = SAFE_STALLOC(_PRpmf_struct);

	pmf->started 		= 0;
	pmf->finished		= 0;
	pmf->rewarded		= 0;
	pmf->rewards		= 0;
	pmf->correct 		= 0;
	pmf->error 			= 0;
	pmf->no_choice 	= 0;
	pmf->broken_fix 	= 0;

	/* NOTE: default scores are:
	**		-2	... broken fixation
	**		-1	... no choice
	**		 0	... error
	**		 1 ... correct
	**
	**	This array is needed only if scores
	**		other than this default set are used
	*/
	if(scores < 1) {
		pmf->scores 			= NULL;
		pmf->scores_length 	= 0;
	} else {
		pmf->scores = SAFE_ZALLOC(int, scores);
		for(i=0;i<scores;i++)
			pmf->scores[i] = PL_NOVAL;
		pmf->scores_length = scores;
	}

	/* "History" is the number of trials in the
	** 	past that we keep track of
	*/
	pmf->last_score 	= PL_NOVAL;
	pmf->run_length 	= 0;
	pmf->last_reward 	= PL_NOVAL;

	if(history > 0) {

		pmf->history_length	= history;
		pmf->history_index	= 0;
		pmf->score_history 	= SAFE_ZALLOC(int, history);
		pmf->reward_history 	= SAFE_ZALLOC(int, history);
		for(i=0;i<history;i++) {
			pmf->score_history[i] 	= PL_NOVAL;
			pmf->reward_history[i] 	= PL_NOVAL;
		}

	} else {
		pmf->history_length 			= 0;
		pmf->history_index			= 0;
		pmf->score_history			= NULL;
		pmf->reward_history			= NULL;
	}	
}

/* PUBLIC ROUTINE: pr_pmf_copy
**
**	Makes a new pmf struct based on the 
**		gven pmf.
*/
_PRpmf pr_pmf_copy(_PRpmf pmf)
{
	if(!pmf)
		return(NULL);

	return(pr_pmf_init(pmf->scores_length,
		pmf->history_length));
}

/* PUBLIC ROUTINE: pr_pmf_set_score
**
*/
void pr_pmf_set_score(_PRpmf pmf, int score)
{

	/* always increment "trial started" counter */
	pmf->started++;

	/* update appropriate score count */	
	switch(score) {

		case -2:
			pmf->broken_fix++;
			break;

		case -1:
			pmf->no_choice++;
			break;

		case 0:
			pmf->error++;
			pmf->finished++;
			break;

		case 1:
			pmf->correct++;
			pmf->finished++;
			break;
	}
	
	/* possibly update scores array */
	if(pmf->scores_length > 0 && score >= 0 && score < pmf->scores_length)
		pmf->scores[score]++;
	
	/* always update last_score, run_length */
	if(score == pmf->last_score)
		pmf->run_length++;
	else
		pmf->run_length = 1;
	
	/* possibly update history */
	pmf->last_score  = score;	
	if(pmf->history_length) {
		
		if(++pmf->history_index == pmf->history_length)
			pmf->history_index = 0;

		pmf->score_history[ pmf->history_index] = score;
	}
}

/* PUBLIC ROUTINE: pr_pmf_set_reward
**
*/
void pr_pmf_set_reward(_PRpmf pmf, int reward)
{
	if(reward>0)
		pmf->rewarded++;
	pmf->rewards += reward;
	pmf->last_reward = reward;
	
	if(pmf->history_length)
		pmf->reward_history[pmf->history_index] = pmf->last_reward;
}

/* PUBLIC ROUTINE: pr_pmf_print
**
*/
void pr_pmf_print(_PRpmf pmf)
{
	double corr;	
	double fini;
	double perc;

	if(!pmf) {
		printf("NULL PMF\n");
		return;
	}

	fini = pmf->finished;
	corr = pmf->correct;
	perc = (corr/fini)*100;

/*
	printf("%3ds, %3df, %3dr, %3drs, %3dc, %3de, %3dn %3db: %6.2lf pct\n",
		pmf->started, pmf->finished, pmf->rewarded, pmf->rewards,
		pmf->correct, pmf->error, pmf->no_choice,  pmf->broken_fix,
		perc);
*/

      printf("%3ds, %3df, %3dr, %3dc, %3de, %3dn %3db: %6.2lf pct %d ms\n",
         pmf->started, pmf->finished, pmf->rewarded,
         pmf->correct, pmf->error, pmf->no_choice,  pmf->broken_fix,
         pmf->finished ? 100.0*((double)pmf->correct/pmf->finished):0, pmf->rt_mean);
}

/* PUBLIC ROUTINE: pr_pmf_clear
**
*/
void pr_pmf_clear(_PRpmf pmf)
{
	register int i;

	if(!pmf)
		return;

	pmf->started  	 = 0;
	pmf->finished 	 = 0;
	pmf->correct	 = 0;	
	pmf->error		 = 0;	
	pmf->no_choice	 = 0;	
	pmf->broken_fix = 0;	

	for(i=0;i<pmf->scores_length;i++)
		pmf->scores[i] = 0;

	pmf->last_score  = PL_NOVAL;
	pmf->run_length  = 0;
	pmf->last_reward = PL_NOVAL;

	for(i=0;i<pmf->history_length;i++) {
		pmf->score_history[i]  = PL_NOVAL;
		pmf->reward_history[i] = PL_NOVAL;
	}
	pmf->history_index = 0;
}
		
/* PUBLIC ROUTINE: pr_pmf_free
**
*/
void pr_pmf_free(_PRpmf pmf)
{
	if(!pmf)
		return;

	SAFE_FREE(pmf->scores);
	SAFE_FREE(pmf->score_history);
	SAFE_FREE(pmf->reward_history);
	
	SAFE_FREE(pmf);
}

/* PRIVATE ROUTINES */

/* PRIVATE ROUTINE: pr_list_to_menu
**
*/
void pr_list_to_menu(_PLlist list, MENU *umenus, int index, const char *name_prefix)
{
	register int i;
	char buf[256];

	/* check for list */
	if(!list || !list->properties_length) {
		umenus[index].me_name = TOY_CARBONCOPY("");
		umenus[index].me_vlp  = NP;
		return;
	}

	/* add the name */
	if(name_prefix != NULL)
		sprintf(buf, "%s", name_prefix);
	else
		buf[0] = '\0';
	
	if(list->name != NULL)
		sprintf(buf, "%s%s",  buf, list->name);
	else	
		sprintf(buf, "%sm%d", buf, index);

	umenus[index].me_name = TOY_CARBONCOPY(buf);

	/* Make the vlist array ... use 'length+1' to 
	**		add trailing NULL, below.
	*/
	gl_vlistPs[index] = 
		malloc(sizeof(VLIST)*(list->properties_length+1));

	for(i=0;i<list->properties_length;i++) {

		(gl_vlistPs[index])[i].vl_name  = 
			TOY_CARBONCOPY(list->properties[i]->name);
		(gl_vlistPs[index])[i].vl_add   =
			(void *) (list->properties[i]->values);
		(gl_vlistPs[index])[i].vl_basep = NP;
		(gl_vlistPs[index])[i].vl_accf  = NP;
		(gl_vlistPs[index])[i].vl_type  = ME_NVALD;

		/* Set up compound menus ... vl_flag indicates
		**		that we'll add vl_add to me_basep ...
		**		Here we also use vl_basep as a dummy to
		**		store the total length of the list, which
		**		gets checked in the maf (below)
		*/
		if(list->values_length == 1) {
			(gl_vlistPs[index])[i].vl_basep	= NP;
			(gl_vlistPs[index])[i].vl_flag	= 0;
		} else {
			(gl_vlistPs[index])[i].vl_basep	= list->values_length;
			(gl_vlistPs[index])[i].vl_flag	= ME_GB;
		}
	}

	/* add trailing NULL VLIST */
	(gl_vlistPs[index])[i].vl_name = TOY_CARBONCOPY("");
	
	/* Fill the menu... again setting up access functions
	** 	for compound lists (I just made that term up.
	**		Nice, eh?) 
	*/	
	umenus[index].me_vlp  	= gl_vlistPs[index];
	umenus[index].me_basep 	= NP;
	umenus[index].me_help 	= TOY_CARBONCOPY("");

	if(list->values_length == 1) {

		umenus[index].me_accf  	= NP;
		umenus[index].me_flag 	= NP;
		umenus[index].me_rtagen = NP;

	} else {

		umenus[index].me_accf  	= pr_maf;
		umenus[index].me_flag 	= ME_BEF;
		umenus[index].me_rtagen = pr_agf;
	}
}

/* PRIVATE ROUTINE: pr_agf
**
**	Argument generation function for Rex menus created
**		automatically from task lists (used for root files).
*/
int pr_agf(int call_cnt, MENU *mp, char *astr)
{
	/* check menu's me_basep for address of list,
	**		to determine the number of parameters 
	*/
	int values_length = mp->me_vlp[0].vl_basep;

   if(call_cnt >= values_length) 
		*astr='\0';
   else 
		itoa_RL(call_cnt, 'd', astr, &astr[P_ISLEN]);

   return(0);
}

/* PRIVATE ROUTINE: pr_maf
**
**	Menu access function for Rex menus created
**		automatically from task lists
*/
int pr_maf(int flag, MENU *mp, char *astr, ME_RECUR *rp)
{
	/* check menu's me_basep for address of list,
	**		to determine the number of parameters 
	*/
	int values_length = mp->me_vlp[0].vl_basep,
		 num   			= (*astr == '/0') ? 0 : atoi(astr);

	if(num < 0 || num >= values_length)
		return(-1);

	mp->me_basep = num*sizeof(valtype);

	return(0);
}
