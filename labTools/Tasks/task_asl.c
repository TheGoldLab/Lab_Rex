/*
** task_asl.c
**
** Task asl for calibrating ASL video eye tracker.
**	Basically just makes a grid of fixation points.
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
void 		asl_make_trials(_PRtask);
_PRtrial asl_get_trial	(_PRtask, int);
void 		asl_set_trial	(_PRtrial);
void 		asl_show_all	(void);
void 		asl_hide_all	(void);

/* PRIVATE VARIABLES */
_PLgroup	gl_graphic; /* see show/hide, below */

/* 
** TASK: ASL
*/
struct _TAtask_struct gl_asl_task[] = {

	/* name */
	"asl",

	/* 
	** Menus 
	*/
	{
		/* 
		** Task info
		*/
		{ TI, 					1 },
		{ "Randomize",			0, 1.0 },
		{ "Trial_blocks",		1, 1.0 },
		{ "Cal0/Val1",			0, 1.0 },
   	{ DX_DIAM,				0, 0.1 },
   	{ DX_CLUT,				0, 1.0 },
		{ "Grid_size",			1, 1.0 },
   	{ "Grid_spacing_x", 	0, 1.0 },
   	{ "Grid_spacing_y", 	0, 1.0 },
   	{ "Grid_center_x", 	0, 1.0 },
   	{ "Grid_center_y", 	0, 1.0 },
   	{ "Grid_rotate_x", 	0, 1.0 },
   	{ "Grid_rotate_y", 	0, 1.0 },
   	{ "Grid_rotate_ang",	0, 1.0 },
		{ "Drop_codes",		1, 1.0 },
   	{ NULL },

		/* END OF MENUS */
		{ NULL }
	},

	/* 
	** 1 dXtarget  graphics objects
	** 1 dXtargets graphics objects
	**
	**	Yes, this is a little bit silly.
	**	The "target" object is for showing
	**		one at a time.
	**	The "targets" is for showing all at
	**		once.
	*/
	{  { DX_TARGET,	1 }, 
		{ DX_TARGETS,	1 }, 
		{ NULL }
	},

	/* 
	**	User functions
	*/
	{	{ "asl_show_all", &asl_show_all },
		{ "asl_hide_all", &asl_hide_all },
		{ NULL },
	},

	/* 
	**	Task methods
	*/
	&asl_make_trials,
	&asl_get_trial,
	&asl_set_trial,
};

/* PUBLIC ROUTINE: asl_make_trials
**
*/
void asl_make_trials(_PRtask task)
{
	register int i,j,ind;
   int sz = TIMV(task, "Grid_size"),
		 sx = TIMV(task, "Grid_spacing_x"),
		 sy = TIMV(task, "Grid_spacing_y"),
		 cx = TIMV(task, "Grid_center_x"),
		 cy = TIMV(task, "Grid_center_y"),
		 rx = TIMV(task, "Grid_rotate_x"),
		 ry = TIMV(task, "Grid_rotate_y"),
		 ra = TIMV(task, "Grid_rotate_ang"),
		 cosa = cos(ra*DEG2RAD),
		 sina = sin(ra*DEG2RAD);
	double x,y;

	/* Make trials */
	pr_task_add_trials1D(task, sz*sz);

	/* compute x,y for each trial */
	y = cy - ((sz-1)*sy)/2.0;
	for(ind=0,i=sz;i>0;i--,y+=sy) {
		x = cx - ((sz-1)*sx)/2.0;
		for(j=sz;j>0;j--,x+=sx) {

			task->trials[ind++]->list = 
				pl_list_initV("1dtc", 0, 1,
					"x", (int) (rx+(x-rx)*cosa-(y-ry)*sina), 1.0,
					"y", (int) (ry+(x-rx)*sina-(y-ry)*cosa), 1.0,
					NULL);
		}
	}
}

/* PUBLIC ROUTINE: asl_get_trial
**
** Returns:
**		_PRtrial if trials remain
**		NULL otherwise
*/
_PRtrial asl_get_trial(_PRtask task, int reset_flag)
{
   int trial_blocks = TIMV(task, "Trial_blocks");

   switch(TIMV(task, "Randomize")) {

      /* BLOCK */
      case 0:

			return(tu_get_block(task, trial_blocks, reset_flag));

      /* RANDOMIZE */
      case 1:

         return(tu_get_random(task, trial_blocks, reset_flag));

      /* REPEAT */
      default:

			return(PR_TRIAL(task));
   }
}

/* PUBLIC ROUTINE: asl_set_trial
**
*/
void asl_set_trial(_PRtrial trial)
{
	/* Set x, y positions, diameter & clut 
	**		in dXtarget object
	*/
	dx_set_by_indexIV(DXF_NODRAW, 0, 0, NULL, 
		1, PL_L2PV(trial->list, 0),
		2, PL_L2PV(trial->list, 1),
		3, TIMV(trial->task, DX_DIAM),
		4, TIMV(trial->task, DX_CLUT),
		ENDI);

	if(TIMV(trial->task, "Drop_codes") != 0) 

		pl_ecodes_by_nameV(dx_get_object("dXtarget"), 7000,
			DX_X, 		0, 	I_FIXXCD,
			DX_Y, 		0, 	I_FIXYCD,
			DX_CLUT, 	0, 	I_FIXLCD,
			NULL);
}

/* PUBLIC ROUTINE: asl_show_all
**
*/
void asl_show_all(void)
{
	register int i;
	_PRrecord rec = pr_get_rec();
	_PRtask   task;
	char	buf[1024];

	/* make sure we are in "pause" mode */
	if(softswitch != PSTOP)
		return;

	/* find asl task graphic & set,
	** saving current 
	*/
	for(i=0;i<rec->tasks_length;i++)
		if(!strcmp(rec->tasks[i]->type, "asl"))
			break;

	/* this shouldn't happen */
	if(i==rec->tasks_length)
		printf("asl_show_all: asl task not found: huh?");

	/* save the old graphic */
	gl_graphic = rec->dx->current_graphic;

	/* get the asl task */
	task = rec->tasks[i];

	/* swap in the asl graphics */
	dx_set_graphic(rec->dx, task->graphics);

	/* make sure we set the graphics group... this
	** is necessary to put here because first time
	** through it might not be set yet
	*/
	/* Set x, y positions, diameter & clut 
	**		in dXtarget object ... make command
	**		by hand. Careful to use the appropriate multipliers.
	** NOTE that I probably should set LOCAL visible
	**	 	(and other)	property to current values...
	*/
	sprintf(buf,"rSet('dXtargets',1,'%s',true,'%s',%.1f,'%s',[",
		DX_VIS, DX_DIAM, 0.1*TIMV(task,DX_DIAM), DX_CLUT);
	for(i=0;i<task->trials_length;i++)
		sprintf(buf,"%s%d,", buf,(int) (i%6+1)); 
	buf[strlen(buf)-1] = ']';
	sprintf(buf,"%s,'%s',[", buf, DX_X);

	for(i=0;i<task->trials_length;i++)
		sprintf(buf,"%s%.1f,", buf, 0.1*task->trials[i]->list->properties[0]->values[0]);
	buf[strlen(buf)-1] = ']';
	sprintf(buf,"%s,'%s',[",buf,DX_Y);
	for(i=0;i<task->trials_length;i++)
		sprintf(buf,"%s%.1f,", buf, 0.1*task->trials[i]->list->properties[1]->values[0]);
	buf[strlen(buf)-1] = ']';
	sprintf(buf,"%s);draw_flag=1;",buf);

	/* send it */
	dx_send(buf);
}

/* PUBLIC ROUTINE: asl_hide_all
**
*/
void asl_hide_all(void)
{
	_PRrecord rec = pr_get_rec();
	_PRtask   task;

	/* get task, make sure it's an asl task */
	if(!rec->tasks ||
		!(task = rec->tasks[rec->task_index]) ||
		strcmp(task->type, "asl"))
		return;

	/* set all visible=false, no return flag */
	dx_send("rSet('dXtargets',1,'visible',false);draw_flag=1;");

	/* restore old graphics */
	dx_set_graphic(rec->dx, gl_graphic);
}

