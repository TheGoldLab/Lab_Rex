/* DOTSX.C
**
**	A collection of c-routines for controlling
**		mac dotsX routines, via udp
**
**	Revision history:
**		2/17/06 created by jig
*/

/* INCLUDED files */
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <sys/types.h>
#include "dotsX.h"
#include "toys.h"
#include "udp.h"
#include "../sset/lcode.h"

/* PRIVATE DATA TYPES */

/* PRIVATE CONSTANTS */
#undef  SHOW_COMMANDS
enum {
	kSent,
	kReturned
};

/* PRIVATE VARIABLES */

/* 
** Global dx struct, for dx_set/show/hide calls
*/
static _DXgraphics gl_dx = NULL;

/* 
**
** GRAPHICS REGISTRY 
**
*/
static struct _PLinit_struct gl_graphics[] = {
	/*
	** dXtarget
	*/
   { DX_TARGET },
   { DX_VIS,    0, 1.0 },
   { DX_X,      0, 0.1 },
   { DX_Y,      0, 0.1 },
   { DX_DIAM,   0, 0.1 },
   { DX_CLUT,   0, 1.0 },
   { NULL },

	/*
	** dXdots
	*/
   { DX_DOTS },
   { DX_VIS,    0, 1.0 },
   { DX_X,      0, 0.1 },
   { DX_Y,      0, 0.1 },
   { DX_DIAM,   0, 0.1 },
   { DX_CLUT,   0, 1.0 },
   { DX_COH,    0, 0.1 },
   { DX_DIR,    0, 1.0 },
   { DX_SPEED,  0, 0.1 },
   { DX_SEED,   0, 1.0 },
   { DX_DENSITY, 167, 0.1 },
   { NULL },

	/*
	** dXtargets
	*/
   { DX_TARGETS },
   { DX_VIS,    0, 1.0 },
   { DX_DIAM,   0, 0.1 },
   { DX_CLUT,   0, 1.0 },
   { NULL },

   /*
   ** dXbeep
   */
   { DX_BEEP },
   { DX_FREQ,   0, 1.0 },
   { DX_DUR,    0, 0.1 },
   { DX_GAIN,   0, 0.1 },
   { NULL },

	/*
	**	DONE
	*/
   { NULL }};

/* PUBLIC ROUTINES */

/* PUBLIC ROUTINE: dx_setup
**
** args:
**		mon_horiz_cm	... horizontal dimension of monitor	(cm)
**		view_dist_cm	... distance from viewer to center of monitor (cm)
*/
void dx_setup(int mon_horiz_cm, int view_dist_cm)
{
	char buf[256],*ret;
	register int i,j;
	_PLgroup *gr;
	_PLlist  *li;
	long	tm, min_t=9999L, max_t=0L, mean_t=0L;

	/* Call rRemoteSetup, which will:
	**	- call rClear to clear ROOT_STRUCT
	**	- send a return message with (useless) screen info
	**	- send return messages to test timing
	*/
	sprintf(gl_dx->message_buf, "rRemoteSetup(%d,%d);", mon_horiz_cm, view_dist_cm);
	dx_send_message_buf(); /* send the command via dx_send, which will save to buf */
	if(!(ret = udp_get(3000))) {
		perror("dx_setup: no return message after 3000 ms\n");
		exit(0);
	}
	/* reset waiting flag */
	gl_dx->waiting = 0;

	/* Test sequence of round trips.
	**	Logic as follows:
	** 	- prime with one test message
	**		- LOOP ..
	**			- start timer (tic)
	**			- send message
	**			- get return message (contains GetSecs() on Mac)
	**			- if round-trip time < 1000 ms, compute offset
	**				(in seconds) between Rex time and Mac time
	*/
#define NUM_TESTS 50 

	/* Prime with one test message/return */
	udp_send("%%");
	if(!(ret = udp_get(3000))) {
		perror("dx_setup: no return message after 3000 ms\n");
		exit(0);
	}

	/* Wait until fast (<=1ms) round trip,
	**		then set up comparison times
	*/
	for(i=0;i<NUM_TESTS;i++) {
		timer_tic();
		udp_send("%%");
		if(!(ret = udp_get(3000))) {
			perror("dx_setup: no return message");
			exit(1);
		}
		if(timer_tocR() <= 1L) {
			tm = dx_compare_times(ret);
			break;
		}	
	}
	if(i == NUM_TESTS) {
		printf("dx_setup: could not establish fast communication\n");
		fflush(stdout);
		exit(0);		
	}

	/* Test multiple round trips */
	for(i=0;i<NUM_TESTS;i++) {
		udp_send("%%");
		if(!(ret = udp_get(3000))) {
			perror("dx_setup: no return message");
			exit(1);
		}
		tm = dx_compare_times(ret);
		if(min_t > tm)
			min_t = tm;
		if(max_t < tm)
			max_t = tm;
		mean_t += tm;
	}

	/* send final message and give feedback */
	printf("dx_setup udp times: mean=%ld msec (min=%ld, max=%ld)\n",
			gl_dx->time_offset = mean_t/NUM_TESTS, min_t, max_t);

	/* store new screen values */
	gl_dx->mon_horiz_cm = mon_horiz_cm;
	gl_dx->view_dist_cm = view_dist_cm;

	/* Add all the objects from the gl_dx struct
	** 	to the dotsX ROOT_STRUCT
	*/
	gl_dx->current_graphic = NULL;	
	for(gr=gl_dx->graphics,i=gl_dx->graphics_length;i>0;i--,gr++) {

		/* set as current (this will send the group command) */
		dx_set_graphic(gl_dx, *gr);

		/* add the objects (lists) */
		for(li=(*gr)->lists,j=(*gr)->lists_length;j>0;j--,li++)
			dx_add(*li);
	}
}

/* PUBLIC ROUTINE: dx_set_index
**
**	Associates certain graphics "objects" in the current
**		graphics group with indices that can be used
**		in various routines, below, including
**			dx_draw_by_index*	
**			dx_position_window
*/
void dx_set_index(int index, int list_index, int value_index)
{
	gl_dx->object_indices[index][0] = list_index;
	gl_dx->object_indices[index][1] = value_index;
}

/* PUBLIC ROUTINE: dx_set_ecode
**
*/
void dx_set_ecode(int ecode)
{
	gl_dx->ecode = ecode;
}

/* PUBLIC ROUTINE: dx_get_object
**
*/
_PLlist dx_get_object(char *name)
{
	return(pl_group_get_list(gl_dx->current_graphic, name));
}

/* PUBLIC ROUTINE: dx_print
**
*/
void dx_print(void)
{
	dx_print_dx(gl_dx);
}

/* PUBLIC ROUTINE: dx_set_flags
**
*/
void dx_set_flags(dflag flags)
{
	gl_dx->draw_flags = flags;
}

/* PUBLIC ROUTINE: dx_add_flags
**
*/
void dx_add_flags(dflag flags)
{
	gl_dx->draw_flags |= flags;
}

/* PUBLIC ROUTINE: dx_remove_flags
**
*/
void dx_remove_flags(dflag flags)
{
	gl_dx->draw_flags &= ~flags;
}

/* PUBLIC ROUTINE: dx_get_flags
**
*/
dflag dx_get_flags(void)
{
	return(gl_dx->draw_flags);
}

/* PUBLIC ROUTINE: dx_send_message_buf
**
*/
void dx_send_message_buf(void)
{
	dx_send(gl_dx->message_buf);
}

/* PUBLIC ROUTINE: dx_send
**
**	Send the contents of gl_dx->message_buf,
**	after checking that the previous handshake
**	had been received
*/
void dx_send(char *buf)
{
	if(strlen(buf) == 0) {
		printf("dx_send: strlen = 0");
		return;	
	}

#ifdef SHOW_COMMANDS
	printf("dx_send: <%s>\n", buf);
	fflush(stdout);
#endif

	/* check for handshake */
	if(gl_dx->waiting && udp_get(3000) == NULL)
		printf("dx_send: waited 3 sec, no handshake\n");

	/* send the message */
	udp_send(buf);

	/* set waiting (for handshake) flag */
	gl_dx->waiting = 1;

	/* save a copy of the command in the buffer */
	strcpy(&(gl_dx->cmd_buf[gl_dx->cmd_index][0]), buf);
	gl_dx->cmd_type[gl_dx->cmd_index] = kSent;
		
	/* increment the index */
	if(++(gl_dx->cmd_index) == DX_CMD_BUFLEN) {
		printf("dx_send: save buffer overflow\n");
		gl_dx->cmd_index = 0;
	}
}

/* PUBLIC ROUTINE: dx_check
**
**	Check for "went" message from dotsX, 
**		typically called in state set after any 
**		dotsX command is sent
**	
**	Returns:
**		DX_MSG if a return mesasge was received 
**			(or if we're not waiting)
**		DX_NO_MSG if the expected return message 
**			has not yet arrived
*/
int dx_check(void)
{
	char *buf;

	/* If there's no ecode to drop or we're not
	**		waiting, outta...
	*/
	if(!gl_dx->ecode || !gl_dx->waiting)
		return(DX_MSG);

	/* check for message */
	if(!(buf = udp_get(0)))
		return(DX_NO_MSG);

	/* found message, reset flag */
	gl_dx->waiting = 0;

	/* drop code, then reset */
	ec_send_code_hi(gl_dx->ecode);
	gl_dx->ecode = 0;

	/* store return time in command buffer */
	strcpy(&(gl_dx->cmd_buf[gl_dx->cmd_index][0]), buf);
	gl_dx->cmd_type[gl_dx->cmd_index] = kReturned;
	if(++gl_dx->cmd_index == DX_CMD_BUFLEN) {
		printf("dx_check: save buffer overflow\n");
		gl_dx->cmd_index = 0;
	}

	/* Check timing..
	**	NOTE 7/11/06 jig
	**	The Mac and Rex clocks are not exactly
	**	in sync .. there seems to be a drift
	**	of ~1ms/min between the two, which
	**	is, I think, why dx_compare_times
	**	will return increasingly large offsets
	**	as time goes on.
	**	However, I tested the timing of Rex
	**	states of the form...
	** state1: 
	**		do draw_something()
	**		to state2 on DX_MSG % dx_check
	**	state2:
	**		do ec_send_code(CODE)
	**
	**	And it all looks good -- that is,
	** very little wasted time between
	**	draw_something and dx_check,
	**	and dx_check seems to be called
	**	every ~100 usec until it gets something,
	**	then again quickly to ec_send_code
 	**
	** dx_compare_times(buf);
	*/

	/* return flag */
	return(DX_MSG);
}

/* PUBLIC ROUTINE: dx_wait
**
** Waits for went message, if one is
**		expected ... used as a clean-up
**		routine.
*/
void dx_wait(void)
{
	int count = 100;

	/* Check for possible return messages */
	if(gl_dx->waiting) {
		while(count && !udp_get(1000L)) count--;
		if(!count)
			printf("dx_wait: still waiting for went\n");
		gl_dx->waiting = 0;	/* clear flag 								*/
		gl_dx->ecode 	= 0; 	/* make sure we don't drop an ecode */
	}
}

/* PUBLIC ROUTINE: dx_dump
**
**	Possibly dump buffered messages to a file
**
**	If flag is true, dump
**	Otherwise clear buffer
*/
void dx_dump(int trial_num)
{
	register int i;
	char sbuf[256], tbuf[64];
	time_t time_of_day = time(NULL);

	if(trial_num >= 0) {

    	if(!gl_dx->cmd_fp) {

       	/* open file to write */
       	strftime(sbuf, 256, "/lab/scripts/DX_%m-%d-%y@%R",
				localtime(&time_of_day));
			if(!(gl_dx->cmd_fp = fopen(sbuf, "w"))) {
				printf("Could not open Script file to write\n");
				return;
			}
		}

		/* Dump the buffered commands,
		** 	including the inter-trial marker with time 
		*/
		strftime(tbuf, 52, "%T", localtime(&time_of_day));
		sprintf(sbuf, "# ** TRIAL %d: %s **\n", trial_num, tbuf);

		if(fputs(sbuf, gl_dx->cmd_fp) == EOF)
			printf("dx_dump: ERROR writing to file\n");

		for(i=0;i<gl_dx->cmd_index;i++) {
			if(gl_dx->cmd_type[i] == kSent)
				fprintf(gl_dx->cmd_fp, "S->%s\n", gl_dx->cmd_buf[i]);
			else
				fprintf(gl_dx->cmd_fp, "R->%s\n", gl_dx->cmd_buf[i]);
		}

		/* flush buffer now  */
		fflush(gl_dx->cmd_fp);
	}

	/* Always reset index */
	gl_dx->cmd_index = 0;

	/* be nice and clear ecode, since we're at the end 
	**	 the trial
	*/
	gl_dx->ecode = 0;
}

/* PUBLIC ROUTINE: dx_blank
**
** Call DX_BLANK command to blank the
**		screen ... note that we use draw_flag=3,
**		which means to clear buffer then draw ONCE
*/
int dx_blank(long return_flag, long ecode)
{
	register int i,j;
	_PLlist *listP;

	/* get return message, if necessary */
	dx_wait();

	/* Send message, possibly asking for return message */
	if(return_flag == 1)
		gl_dx->ecode = ecode;
	else if(ecode)
		ec_send_code(ecode);
	sprintf(gl_dx->message_buf, "%s;draw_flag=3;", DX_BLANK);
	dx_send_message_buf();

	/* reset gl_dx->is_fixating */
	gl_dx->is_fixating = 0;

	/* Set visible flags to zero. Assume
	**		that if an object has a visible
	**		flag, it is the first property
	*/
	for(i=gl_dx->current_graphic->lists_length,
		listP = gl_dx->current_graphic->lists;
		i>0;i--, listP++) {
		if(!strcmp((*listP)->properties[0]->name, "visible")) {
			for(j=0;j<(*listP)->values_length;j++) {
				(*listP)->properties[0]->values[j] = 0;
			}
		}
	}

	/* outta */
	return(0);
}

/* PUBLIC ROUTINE: dx_add
**
*/
void dx_add(_PLlist list)
{
	register int i;
	int vi=-1, *vis=NULL;

	/* call parse start to clean things up */
	dx_parse_start(list, &vi, &vis);

	/* set up the "change" arrays by hand, to 
	**		include all properties, all values
	*/
	for(i=0;i<list->properties_length;i++)
		gl_dx->p2change[i] = list->properties[i];
	gl_dx->p2change_length = list->properties_length;

	for(i=0;i<list->values_length;i++)
		gl_dx->v2change[i] = 1;

	/* Make & send it, with no return message */
	dx_parse(DXF_NODRAW, DX_ADD, list, vi, vis);
}

/* PUBLIC ROUTINE: dx_set
**
** Basic set routine, for a single property, single value.
*/
void dx_set(dflag flags, _PLlist list, 
	_PLproperty prop, int vi, valtype value)
{
	int *vs = NULL;

	/* Start the parse machinery */
	dx_parse_start(list, &vi, &vs);

	/* Set the value */
	dx_prop_set(prop, vs[0], value);

	/* Make & send it, with return message */
	dx_parse(flags, DX_SET, list, vi, vs);
}	

/* PUBLIC ROUTINE: dx_setV
**
** Basic set routine, for multiple properties, 
**		multiple values.
** V for "va_list" ... properties, values
** 
** Examples:
**		dx_setV(0, target_list, 3, NULL,
**			x_prop, 40,
**			y_prop, 50,
**			NULL);
*/
void dx_setV(dflag flags, _PLlist list, 
	int vi_length, int *vis, ...)
{
	_PLproperty 	prop;
	va_list 	 		ap;
	char 			  *property_name;
	register int   i;

	/* initialize machinery to create rSet command */ 
	dx_parse_start(list, &vi_length, &vis);

	/* get the va_args */
	va_start(ap, vis);

	/* loop through the properties given as arguments */
	while(prop = va_arg(ap, _PLproperty)) {

		/* loop through the inputs, getting and setting */	
		for(i=0;i<vi_length;i++)
			dx_prop_set(prop, vis[i], va_arg(ap, valtype));
	}

	/* end args */
	va_end(ap);

	/* Make & send it */		
	dx_parse(flags, DX_SET, list, vi_length, vis);
}	

/* PUBLIC ROUTINE: dx_set1
**
*/
int dx_set1(long ecode, long class_index, long property_index,
   long object_index, long value)
{
   /* set ecode */
   gl_dx->ecode = ecode;

   /* set visible flag to 1 */
   dx_set_by_index(class_index, property_index, object_index, value);
}

/* PUBLIC ROUTINE: dx_show_fp
**
** Assumes fixation point is object index 0
**
** Arguments:
**		ecode				... to drop upon draw
**		index				... in object_indices
**		dstart			... start diameter
**		dend				... end diameter
**		cstart			... start clut index
**		cend				... end clut index
*/
int dx_show_fp(long ecode, long index, long dstart, 
	long dend, long cstart, long cend)
{
	register int i;
	int num_ds, dinc, dv, num_cs, cinc, cv;

	/* set ecode */
	gl_dx->ecode = ecode;

	/* Get dXtarget fp object (PLlist) */	
	if(!DX_CHKI(index)) {
		printf("dx_show_fp: could not get dXtarget object(s)");
		return(0);
	}

	gl_dx->fp->fp   = DX_OBJ_BYI(index);
	gl_dx->fp->vind = DX_VI(index);

	/* get visible, diameter, clut properties */
	gl_dx->fp->visible  = pl_list_get_prop(gl_dx->fp->fp, DX_VIS);
	gl_dx->fp->diameter = pl_list_get_prop(gl_dx->fp->fp, DX_DIAM);
	gl_dx->fp->clut 	  = pl_list_get_prop(gl_dx->fp->fp, DX_CLUT);

	/* send rSet values ... send a flag to draw once */
	dx_setV(DXF_D3, gl_dx->fp->fp, gl_dx->fp->vind, NULL, 
		gl_dx->fp->visible, 	1,
		gl_dx->fp->diameter,	dstart < 0 ? PL_NOVAL : dstart,
		gl_dx->fp->clut,		cstart < 0 ? PL_NOVAL : cstart,
		NULL);

	/* Set up "change_fp" arrays for diameter, color.*/
	num_ds = dstart < 0 || dend < 0 ? 0 : abs(dend - dstart + 1);
	num_cs = cstart < 0 || cend < 0 ? 0 : abs(cend - cstart + 1);

	if(!num_ds && !num_cs) {
		gl_dx->fp->num_reps  = 0;
		gl_dx->fp->rep_count = 0;
		return;
	}

	dv = dstart = dstart < 0 && dend < 0 ? NULLI :
				dstart < 0 ? dend : dstart;
	dinc = dstart < 0 || dend < 0 || dstart == dend ? 0 : 
				dstart < dend ? 1 : -1;
	cv = cstart = cstart < 0 && cend < 0 ? NULLI :
				cstart < 0 ? cend : cstart;
	cinc = cstart < 0 || cend < 0 || cstart == cend ? 0 : 
				cstart < cend ? 1 : -1;

	gl_dx->fp->num_reps  = TOY_MAX(num_ds, num_cs);
	gl_dx->fp->rep_count = 1;
	for(i=0;i<gl_dx->fp->num_reps;i++,dv+=dinc,cv+=cinc) {

		/* set diameter */
		gl_dx->fp->ds[i] = dv;
		if(dv==dend)
			dinc = 0;

		/* set clut */
		gl_dx->fp->cs[i] = cv;
		if(cv==cend)
			cv = cstart-cinc;
	}

	/* outta */
	return(0);
}

/* PUBLIC ROUTINE: dx_change_fp
**
*/
int dx_change_fp(void)
{
	/* nothing to change */
	if(gl_dx->fp->rep_count >= gl_dx->fp->num_reps)
		return(0);

	/* set new values ... here we send flags to draw once
	** but do NOT send return message
	*/
	dx_setV(DXF_NO_RFLAG|DXF_DRAW_ONCE, gl_dx->fp->fp, 
		gl_dx->fp->vind, 		NULL, 
		gl_dx->fp->diameter, gl_dx->fp->ds[gl_dx->fp->rep_count],
		gl_dx->fp->clut,		gl_dx->fp->cs[gl_dx->fp->rep_count],
		NULL);

	/* increment count */
	gl_dx->fp->rep_count++;

	/* outta */	
	return(1);
}

/* PUBLIC ROUTINE: dx_hide_fp
**
** Utility routine to hide (set property visible=0) 
**		the fixation point.
**
** Arguments:
**		nada
**
** Returns:
**		int 0 ... so this can be called from a statelist
*/
int dx_hide_fp(long ecode)
{
	/* set ecode */
	gl_dx->ecode = ecode;

	/* make dx_set do all the hard work */
	dx_set(DXF_DEFAULT, gl_dx->fp->fp, gl_dx->fp->visible, 
		gl_dx->fp->vind, 0);

	/* outta */
	return(0);
}

/* PUBLIC ROUTINE: dx_toggle1
**
** Convenience routine for showing/hiding one object.
**
** Arguments:
**		toggle_flag ... determines how to interpret
**								TOY_RCMP(prob) (which returns
**								1 if rand() < prob, else 0),
**								as follows:
**							 0: hide if TOY_RCMP == 1, else do nothing
**							 1: show if TOY_RCMP == 1, else do nothing
**							 2: show if TOY_RCMP == 1, else hide
**		index			... index into gl_dx->object_indices
*/
int dx_toggle1(long ecode, long toggle_flag, 
	long index, long prob, long diam, long clut)
{
	int rcmp;

	/* check for do nothing */
	if(prob < 0 || (!(rcmp=TOY_RCMP(prob)) && toggle_flag < 2))
		return(0);

	/* check for hide */
	if(toggle_flag == 0 || (toggle_flag == 2 && rcmp == 0) || 
		diam == 0) {

      /* set ecode */
      gl_dx->ecode = ecode;

		/* simple hide command */
		dx_off_by_index(DX_OI(index), ecode, DX_VI(index));

	} else {

		/* set ecode */
		gl_dx->ecode = ecode;

		/* set visible, diam, clut */
		dx_set_by_nameIV(DXF_DEFAULT, DX_OBJ_BYI(index)->name,
			DX_VI(index), NULL,
			DX_VIS, 	1,
			DX_DIAM,	diam > 0 ? diam : NULLI,
			DX_CLUT,	clut > 0 ? clut : NULLI,
			NULL);
	}

	/* outta, no codes to drop explicitly */
	return(0);
}

/* PUBLIC ROUTINE: dx_toggle2
**
** Convenience routine for showing/hiding two objects.
** Toggle means you can either show or hide (or do nothing),
**	depending on toggle_flag and prob*.
**
** Arguments:
**		ecode			... ecode to send after receiving
**							"went" flag after draw
**		toggle_flag ... determines how to interpret
**								TOY_RCMP(prob*) (which returns
**								1 if rand() < prob*, else 0),
**								as follows:
**							 0: hide if TOY_RCMP == 1, else do nothing
**							 1: show if TOY_RCMP == 1, else do nothing
**							 2: show if TOY_RCMP == 1, else hide
**		index*		... index into gl_dx->object_indices
**		prob*			... <0 means do nothing, else arg to TOY_RCMP
*/
int dx_toggle2(long ecode, long toggle_flag,
   long indexA, long probA,
   long indexB, long probB)
{
   int inds[2], rcmp, vis0, vis1, tf;

   if(indexA < 0 && indexB < 0)
      return;

   if(indexA < 0)
      return(dx_toggle1(ecode, toggle_flag, indexB, probB, -1, -1));
   else if(indexB < 0)
      return(dx_toggle1(ecode, toggle_flag, indexA, probA, -1, -1));

   /* get visible flags for object1, 2 */
   tf   = toggle_flag < 3 ? toggle_flag : toggle_flag - 3;
   vis0 = probA < 0 || (!(rcmp=TOY_RCMP(probA)) && tf < 2) ? NULLI :
            tf == 0 || (tf == 2 && rcmp == 0) ? 0 : 1;
   tf   = toggle_flag < 3 ? toggle_flag : 4 - toggle_flag;
   vis1 = probB < 0 || (!(rcmp=TOY_RCMP(probB)) && tf < 2) ? NULLI :
            tf == 0 || (tf == 2 && rcmp == 0) ? 0 : 1;

   /* check */
   if((vis0 == NULLI && vis1 == NULLI) ||
      (indexA < 0 && vis1 == NULLI)    ||
      (indexB < 0 && vis0 == NULLI))
      return;

   /* get indices */
   inds[0] = indexA < 0 ? NULLI : DX_VI(indexA);
   inds[1] = indexB < 0 ? NULLI : DX_VI(indexB);

   /* set ecode */
   gl_dx->ecode = ecode;


   /* check whether two objects are the same class or not */
   if(indexA >= 0 && indexB >= 0 &&
      DX_OI(indexA) == DX_OI(indexB) &&
      vis0 != NULLI && vis1 != NULLI) {

      /* send one command, both objects */
      dx_set_by_nameIV(DXF_DEFAULT, DX_OBJ_BYI(indexB)->name,
         2, inds, DX_VIS,  vis0, vis1, NULL);

   } else if(indexA >= 0 && indexB >= 0 &&
               vis0 != NULLI && vis1 != NULLI) {

      /* Send two commands. All the draw_flags stuff
      ** is there to append the two commands together,
      ** then restore the default draw_flags
      */
      dflag flags = gl_dx->draw_flags;

      dx_set_flags(DXF_STORE);
      dx_set_by_name(DX_OBJ_BYI(indexA)->name, DX_VIS, inds[0], vis0);
      dx_set_flags(DXF_APPEND);
//    dx_set_flags(flags|DXF_APPEND);
      dx_set_by_name(DX_OBJ_BYI(indexB)->name, DX_VIS, inds[1], vis1);
      dx_set_flags(flags);

   } else {

      /* send one command */
      if(vis0 != NULLI)
         dx_set_by_name(DX_OBJ_BYI(indexA)->name, DX_VIS, inds[0], vis0);
      else
         dx_set_by_name(DX_OBJ_BYI(indexB)->name, DX_VIS, inds[1], vis1);
   }

   /* outta, no codes to drop explicitly */
   return(0);
}

/* PUBLIC ROUTINE: dx_toggle3
**
** Convenience routine for showing/hiding three objects.
** Toggle means you can either show or hide (or do nothing),
**	depending on toggle_flag and prob*. Uses first three
**	entries of gl_dx->object_indices and assumes all
**	three objects are of the same class.
**
** Arguments:
**		ecode			... ecode to send after receiving
**							"went" flag after draw
**		toggle_flag ... determines how to interpret
**								TOY_RCMP(prob*) (which returns
**								1 if rand() < prob*, else 0),
**								as follows:
**							 0: hide if TOY_RCMP == 1, else do nothing
**							 1: show if TOY_RCMP == 1, else do nothing
**							 2: show if TOY_RCMP == 1, else hide
**		index*		... index into gl_dx->object_indices
**		prob*			... <0 means do nothing, else arg to TOY_RCMP
*/
int dx_toggle3(long ecode, long toggle_flag,
   long probA, long probB, long probC)
{
   int inds[3], rcmp, vis0, vis1, vis2;

   /* get visible flags for object1, 2 */
   vis0 = probA < 0 || (!(rcmp=TOY_RCMP(probA)) && toggle_flag < 2) ? NULLI :
            toggle_flag == 0 || (toggle_flag == 2 && rcmp == 0) ? 0 : 1;
   vis1 = probB < 0 || (!(rcmp=TOY_RCMP(probB)) && toggle_flag < 2) ? NULLI :
            toggle_flag == 0 || (toggle_flag == 2 && rcmp == 0) ? 0 : 1;
   vis2 = probC < 0 || (!(rcmp=TOY_RCMP(probC)) && toggle_flag < 2) ? NULLI :
            toggle_flag == 0 || (toggle_flag == 2 && rcmp == 0) ? 0 : 1;

   /* check */
   if(vis0 == NULLI && vis1 == NULLI && vis2 == NULLI)
      return;

   /* get indices */
   inds[0] = DX_VI(0);
   inds[1] = DX_VI(1);
   inds[2] = DX_VI(2);

   /* set ecode */
   gl_dx->ecode = ecode;

   /* set */
   dx_set_by_nameIV(DXF_DEFAULT, DX_OBJ_BYI(0)->name,
         3, inds, DX_VIS,  vis0, vis1, vis2, NULL);

   /* outta, no codes to drop explicitly */
   return(0);
}

/* PUBLIC ROUTINE: dx_position_window
**
** Arguments:
** 	width, height 	... size of window
**		position_index ... where to position it:
**			<0	... @ eye position
**			0+ ... x,y position of object associated with given
**						object_indices in gl_dx
**		fix_flag	... whether (1) or not (0) to check for fixation
**		window 	... first arg to wd_pos (window number)
**		
*/
int dx_position_window(long width, long height, long position_index, 
		long fix_flag, long window)
{
	valtype show_corner;

	/* Center window on a target */
	if(position_index >= 0) {
		_PLlist	dXobj;

		/* check object (list), value index */
		if(!DX_CHKI(position_index)) {
			printf("dx_position_window: Can not position window at index %d\n",
				position_index);
			return;
		}
			
		/* get object (list), value index */
		dXobj = DX_OBJ_BYI(position_index);

		/* call wd_pos to position the window at the object x,y */
		wd_pos(window, 
			pl_list_get_vi(dXobj, DX_X, (long) DX_VI(position_index)),
			pl_list_get_vi(dXobj, DX_Y, (long) DX_VI(position_index)));
	
	} else if(position_index == -1) {

		/* Center window on eye position */
		wd_pos(window, (long) (eyeh/4), (long) (eyev/4));
	}

	/* Check fix flag ... if it has changed, reset global
	**	 and possibly show/hide corner spot indicating fixation...
	**	 Do NOT send dXcorner info as ecodes.
 	*/
	if(gl_dx->is_fixating != fix_flag) {
		if((show_corner = pr_get_pref("Show_corner")) > 0) {

			/* possibly initialize */
			if(gl_dx->is_fixating == -1) {

				/* add dXcorner object */
				sprintf(gl_dx->message_buf, 
					"%s('%s',1,'location',%d,'visible',%ld);",
					DX_ADD, DX_CORNER, (int) show_corner, fix_flag);
				dx_send_message_buf();

			} else {

				sprintf(gl_dx->message_buf, 
					"%s('%s',1,'visible',%d);", DX_SET, DX_CORNER, fix_flag);
				dx_send_message_buf();
			}
		}

		/* set global is_fixating flag */
		gl_dx->is_fixating = fix_flag;
	}

	/* resize the window, activate it, and outta */
	if(position_index > -2) {
		wd_siz(window, width, height);
		wd_cntrl(window, WD_ON);
	}
	
	return(0);
}

/* PUBLIC ROUTINE: dx_check_fix
**
*/
int dx_check_fix(void)
{
	return(gl_dx->is_fixating);
}

int dx_set_fix(long flag)
{
	gl_dx->is_fixating = flag;
}

/* PUBLIC ROUTINE: dx_compare_times
**
*/
int dx_compare_times(char *mac_time)
{
	static double mac_start = 0;
	static struct timespec rex_start;
	struct timespec rex_now;
	double mac_now;
	long rex, mac;

	/* get time from mac */
	if((mac_now = atof(mac_time)) == 0.0)
		return(-999);

	/* get current time */
	if(clock_gettime(CLOCK_REALTIME, &rex_now) == -1) {
		printf("dx_compare_time: could not get time\n");
		return(-9999);
	}

	if(mac_start == 0) {

		/* First time through, get mac, rex
		** reference times ... this is so
		**	we can avoid dealing with really large
		**	numbers, below
		*/
		mac_start 			= mac_now;
		rex_start.tv_sec  = rex_now.tv_sec;
		rex_start.tv_nsec = rex_now.tv_nsec;
	}

	/* compute: 
	**		mac time, since start & in ms
	**		rex time, since start & in ms
	*/ 
	mac = (long) ((mac_now - mac_start) * 1000.);
	rex = (rex_now.tv_sec  - rex_start.tv_sec)  * 1000L +
         (rex_now.tv_nsec - rex_start.tv_nsec) / 1000000L;

/*
**	printf("mac = %ld, rex = %ld, diff = %ld\n",
**		mac, rex, mac-rex);
*/

	/* warning message */
	if(abs(mac-rex) > 5) {
		printf("WARNING: rex/mac times off by %d\n", mac-rex);
		fflush(stdout);
	}

	/* return difference */
	return(mac-rex);
}

/* PUBLIC ROUTINE: dx_beep
**
*/
int dx_beep(long ecode)
{
   /* get return message, if necessary */
   dx_wait();

   /* possibly send ecode */
   if(ecode)   {
      ec_send_code_hi(ecode);
   }
   /* send the message */
   sprintf(gl_dx->message_buf, "%s;draw_flag=0;", DX_PLAY);
   dx_send_message_buf();

   /* outta */
   return(0);
}

	/* SET COMMANDS */

/* PUBLIC ROUTINE: dx_set_by_name
**
**	Sets a single property to the given value, by name.
**
** Example:
**		dx_set_by_name(DX_TARGET, "x", 0, 10);
**
**		sets value[0] of property "x" of 
**		class DX_TARGET to 10
*/
void dx_set_by_name(char *class_name, char *property_name,
	int vi, valtype value)
{
	_PLlist list = pl_group_get_list(
							gl_dx->current_graphic, class_name);

	dx_set(gl_dx->draw_flags, list, 
		pl_list_get_prop(list, property_name), vi, value);
}

/* PUBLIC ROUTINE: dx_set_by_nameIV
**
**	Sets a list of properties to given values, by name
**
**	I for "Indexed" (values/objects)
**	V for "va_list" (property names)
**
**	Examples:
**		dx_set_by_nameIV(0, DX_TARGET, 2, &({2 5}),
**			"x", 40, 50, 
**			"y", 30, 30,
**			NULL);
**
**		dx_set_by_nameIV(0, DX_TARGET, 1, NULL,
**			"x", 40,
**			"y", 30,
**			NULL);
*/
void dx_set_by_nameIV(dflag flags, char *class_name, 
	int vi_length, int *vis, ...)
{
	_PLlist 			list = pl_group_get_list(
									gl_dx->current_graphic, class_name);
	_PLproperty 	prop;
	va_list 	 		ap;
	char 			  *property_name;
	register int   i;

	/* initialize machinery to create rSet command */ 
	dx_parse_start(list, &vi_length, &vis);

	/* get the va_args */
	va_start(ap, vis);

	/* loop through the property names given as arguments */
	while(property_name = va_arg(ap, char *)) {

		/* get the property */
		prop = pl_list_get_prop(list, property_name);

		/* loop through the inputs, getting and setting */	
		for(i=0;i<vi_length;i++) {
			dx_prop_set(prop, vis[i], va_arg(ap, valtype));
		}
	}

	/* end args */
	va_end(ap);

	/* Make & send it */		
	dx_parse(flags, DX_SET, list, vi_length, vis);
}

/* PUBLIC ROUTINE: dx_set_by_index
**
**	Sets a single property to the given value, by index.
**
** Example:
**		dx_set_by_index(0, 0, 0, 10);
**
**		sets value[0] of property[0] of 
**		class[0] (i.e., gl_dx->current_graphic->lists[0])
**		to 10.
*/
void dx_set_by_index(int class_index, int property_index,
	int vi, valtype value)
{
	_PLlist list = gl_dx->current_graphic->lists[class_index];

	dx_set(gl_dx->draw_flags, list, list->properties[property_index],
			vi, value);
}

/* PUBLIC ROUTINE: dx_set_by_indexIV
**
**	Sets a list of properties to given values, by index
**
**	I for "Indexed" (values/objects)
**	V for "va_list" (property names)
**
**	Example:
**		dx_set_by_indexIV(FLAGS, DXTARGET_INDEX, 2, &({2 5}),
**			X_INDEX, 40, 50, 
**			Y_INDEX, 30, 30,
**			NULL);
**
**		dx_set_by_nameIV(FLAGS, dXTARGET_INDEX, 3, NULL,
**			X_INDEX, 40,
**			Y_INDEX, 30,
**			NULL);
*/
void dx_set_by_indexIV(dflag flags, int class_index, 
	int vi_length, int *vis, ...)
{
	_PLlist 			list = gl_dx->current_graphic->lists[class_index];
	_PLproperty 	prop;
	va_list 	 		ap;
	int 			  	property_index;
	register int 	i;

	/* initialize machinery to create rSet command */ 
	dx_parse_start(list, &vi_length, &vis);

	/* get the va_args */
	va_start(ap, vis);

	/* loop through the arguments (per property) */
	while((property_index = va_arg(ap, int)) != ENDI) {

		/* get the property */
		prop = list->properties[property_index];

		/* loop through the inputs, getting and setting */	
		for(i=0;i<vi_length;i++)
			dx_prop_set(prop, vis[i], va_arg(ap, valtype));
	}

	/* end args */
	va_end(ap);

	/* Make & send it */		
	dx_parse(flags, DX_SET, list, vi_length, vis);
}

/* PUBLIC ROUTINE: dx_set_array_by_nameIV
**
** For a single object, sets values of properties
** in which the values are arrays, not scalars
**
** I for "Indexed" (values/objects)
** V for "va_list" (property names)
**
** Example:
*/
void dx_set_array_by_nameIV(dflag flags, int class_name,
      int object_index, ...)
{
   _PLlist        list = pl_group_get_list(
                           gl_dx->current_graphic, class_name);
   char           buf[512], *property_name;
   int            num_entries;
   double         *entries, entry;
   va_list        ap;
   register int   i;

   /* set up the output buffer */
   sprintf(buf, "rSet('%s',%d", class_name, object_index+1);

   /* get the va_args */
   va_start(ap, object_index);

   /* loop through the property names given as arguments */
   while(property_name = va_arg(ap, char *)) {

      /* get number of entries */
      num_entries = va_arg(ap, int);

      if (num_entries == 1) {

         entry = va_arg(ap, double);
         sprintf(buf, "%s,'%s',%.2lf", buf, property_name, entry);

      } else {

         /* get array of entries */
         entries = va_arg(ap, double *);

         /* start array */
         sprintf(buf, "%s,'%s',[", buf, property_name);

         /* loop through the entries, adding to string command */
         for (i=0;i<(num_entries);i++)
            sprintf(buf, "%s%.2lf ", buf, entries[i]);

         /* finish array */
         sprintf(buf, "%s]", buf);
      }
   }

   /* finish va_args */
   va_end(ap);

   /* finish buffer */
   sprintf(buf, "%s);", buf);

   /* send it */
   dx_send(buf);
}

/* PUBLIC ROUTINE: dx_setc_by_name
**
** 'setc' is conditional set
*/
void dx_setc_by_name(char *class_name, char *property_name,
	int prob, int vi, valtype value)
{
	if(TOY_RAND(1000.0) < prob)
		dx_set_by_name(class_name, property_name, vi, value);
}

/* PUBLIC ROUTINE: dx_setc_by_nameIV
**
** 'setc' is conditional set ... first two va_args
**		are "conditionalizations" -- probabilities
**		(0 - 1000) that each indexed value (object)
**		should actually be set.
**
**	Example:
**		dx_setc_by_nameIV(0, 2, DX_TARGET, 2, &({2 5}),
**				  500, 500,
**			"x",  40,  50, 
**			"y",  30,  30,
**			NULL);
**
*/
void dx_setc_by_nameIV(dflag flags, char *class_name, 
	int vi_length, int *vis, ...)
{
	_PLlist 			list = pl_group_get_list(
									gl_dx->current_graphic, class_name);
	_PLproperty 	prop;
	va_list 	 		ap;
	char 			  *property_name;
	register int   i;
	int				conds[16], count;
	valtype			val;

	/* initialize machinery to create rSet command */ 
	dx_parse_start(list, &vi_length, &vis);

	/* get the va_args */
	va_start(ap, vis);

	/* get conditionalizing probabilities */
	for(count=0,i=0;i<vi_length;i++)
		count += (conds[i] = TOY_RCMP(va_arg(ap, int)));

	/* check for any change */
	if(!count) {
		va_end(ap);
		return;
	}

	/* loop through the remaining args */
	while(property_name = va_arg(ap, char *)) {

		/* get the property */
		prop = pl_list_get_prop(list, property_name);

		/* loop through the inputs, getting and setting */	
		for(i=0;i<vi_length;i++) {

			val = va_arg(ap, valtype);
			if(conds[i])
				dx_prop_set(prop, vis[i], val);
		}
	}

	/* end args */
	va_end(ap);

	/* Make & send it */		
	dx_parse(flags, DX_SET, list, vi_length, vis);
}

/* PUBLIC ROUTINE: dx_setc_by_index
**
** 'setc' is conditional set
*/
void dx_setc_by_index(int class_index, int property_index,
	int prob, int vi, valtype value)
{
	if(TOY_RAND(1000.0) < prob)
		dx_set_by_index(class_index, property_index, vi, value);
}

/* PUBLIC ROUTINE: dx_setc_by_indexIV
**
** 'setc' is conditional set
*/
void dx_setc_by_indexIV(dflag flags, int class_index, 
	int vi_length, int *vis, ...)
{
	_PLlist 			list = gl_dx->current_graphic->lists[class_index];
	_PLproperty 	prop;
	va_list 	 		ap;
	int				property_index, conds[16], count;
	valtype			val;
	register int   i;

	/* initialize machinery to create rSet command */ 
	dx_parse_start(list, &vi_length, &vis);

	/* get the va_args */
	va_start(ap, vis);

	/* get conditionalizing probabilities */
	for(count=0,i=0;i<vi_length;i++)
		count += (conds[i] = TOY_RCMP(va_arg(ap, int)));

	/* check for any change */
	if(!count) {
		va_end(ap);
		return;
	}

	/* loop through the remaining args */
	while((property_index = va_arg(ap, int)) != ENDI) {

		/* get the property */
		prop = list->properties[property_index];

		/* loop through the inputs, getting and setting */	
		for(i=0;i<vi_length;i++) {

			val = va_arg(ap, valtype);
			if(conds[i])
				dx_prop_set(prop, vis[i], val);
		}
	}

	/* end args */
	va_end(ap);

	/* Make & send it */		
	dx_parse(flags, DX_SET, list, vi_length, vis);
}

/* PUBLIC ROUTINE: dx_setwIA
**
** 'setw' is set from working copy
**	I for Indices
**	A for All (properties)
*/
void dx_setwIA(dflag flags, _PLlist list, 
			int vi_length, int *vis)
{
	register int i,j;
	_PLproperty *prop = list->properties;

	/* initialize machinery to create rSet command */ 
	dx_parse_start(list, &vi_length, &vis);

	/* Parse the args and set properties... */
	for(i=0;i<list->properties_length;i++,prop++) 
		for(j=0;j<vi_length;j++)
			dx_prop_set(*prop, vis[j], (*prop)->values_wc[vis[j]]);

	/* Make & send it */		
	dx_parse(flags, DX_SET, list, vi_length, vis);
}

/* PUBLIC ROUTINE: dx_setw_by_name
**
** 'setw' is set from working copy
*/
void dx_setw_by_name(char *class_name, char *property_name, int vi)
{
	_PLlist 		list = pl_group_get_list(
								gl_dx->current_graphic, class_name);
	_PLproperty prop = pl_list_get_prop(list, property_name);

	dx_set(gl_dx->draw_flags, list, prop, vi, prop->values_wc[vi]);
}

/* PUBLIC ROUTINE: dx_setw_by_nameIV
**
** 'setw' is set from working copy
**
**	Examples:
**		dx_setw_by_nameIV(0, 2, DX_TARGET, 2, &({2 5}),
**			"x", "y", NULL);
*/
void dx_setw_by_nameIV(dflag flags, char *class_name, 
	int vi_length, int *vis, ...)
{
	_PLlist 			list = pl_group_get_list(
									gl_dx->current_graphic, class_name);
	_PLproperty 	prop;
	va_list 	 		ap;
	char 			  *property_name;
	register int   i;

	/* get the va_args */
	va_start(ap, vis);

	/* get first property name */
	property_name = va_arg(ap, char *);

	/* special case of NULL list -- use all properties */
	if(!property_name) {
		va_end(ap);
		dx_setwIA(flags, list, vi_length, vis);
		return;
	}

	/* initialize machinery to create rSet command */ 
	dx_parse_start(list, &vi_length, &vis);

	/* Parse the args and set properties... */
	do {
	
		/* get the property */
		prop = pl_list_get_prop(list, property_name);

		/* loop through the inputs, getting and setting */	
		for(i=0;i<vi_length;i++)
			dx_prop_set(prop, vis[i], prop->values_wc[vis[i]]);

	} while(property_name = va_arg(ap, char *));

	/* end args */
	va_end(ap);

	/* Make & send it */		
	dx_parse(flags, DX_SET, list, vi_length, vis);
}

/* PUBLIC ROUTINE: dx_setw_by_index
**
** 'setw' is set from working copy 
*/
void dx_setw_by_index(int class_index, int property_index, int vi)
{
	_PLlist 		list = gl_dx->current_graphic->lists[class_index];
	_PLproperty prop = list->properties[property_index];

	dx_set(gl_dx->draw_flags, list, prop, vi, prop->values_wc[vi]);
}

/* PUBLIC ROUTINE: dx_setw_by_indexIV
**
** 'setw' is set from working copy
**
**	Examples:
**		dx_setw_by_indexIV(0, 2, DXTARGET_INDEX, 2, &({2 5}),
**			X_INDEX, Y_INDEX, NULL);
*/
void dx_setw_by_indexIV(dflag flags, int class_index, 
	int vi_length, int *vis, ...)
{
	_PLlist 			list = gl_dx->current_graphic->lists[class_index];
	_PLproperty 	prop;
	va_list 	 		ap;
	int 			   property_index;
	register int   i;

	/* get the va_args */
	va_start(ap, vis);

	/* get first property name */
	property_index = va_arg(ap, int);

	/* special case of NULL list -- use all properties */
	if(!property_index) {
		va_end(ap);
		dx_setwIA(flags, list, vi_length, vis);
		return;
	}

	/* initialize machinery to create rSet command */ 
	dx_parse_start(list, &vi_length, &vis);

	/* Parse the args and set properties... */
	do {
	
		/* get the property */
		prop = list->properties[property_index];

		/* loop through the inputs, getting and setting */	
		for(i=0;i<vi_length;i++)
			dx_prop_set(prop, vis[i], prop->values_wc[vis[i]]);

	} while((property_index = va_arg(ap, int)) != ENDI);

	/* end args */
	va_end(ap);

	/* Make & send it */		
	dx_parse(flags, DX_SET, list, vi_length, vis);
}

/* PUBLIC ROUTINE: dx_setl_by_name
**
** 'setlv' is set from another list
*/
void dx_setl_by_name(_PLlist from, char from_type, char *from_pname, 
	int from_vi, char *to_cname, char *to_pname, int to_vi)
{
	_PLlist 		to 		 = pl_group_get_list(
										gl_dx->current_graphic, to_cname);
	_PLproperty to_prop	 = pl_list_get_prop(to,   to_pname),
					from_prop = pl_list_get_prop(from, from_pname);

	dx_set(gl_dx->draw_flags, to, to_prop, to_vi, 
				from_type == 'v' ? 
				from_prop->values[from_vi] :
				from_prop->values_wc[from_vi]);
}

/* PUBLIC ROUTINE: dx_setl_by_nameIV
**
** 'setl' is set from another list.
**		Note that EITHER
**			from_vi_length must equal to_vi_length
**		- OR -
**			*_vis are length 1 or NULL
**
**	Examples:
**		dx_setl_by_nameIV(0, 
**			from_list,  'w', 2, &({2 5}),
**			DX_TARGET, 2, {1 3}, 
**			"x",
**			"y",
**			NULL);
*/
void dx_setl_by_nameIV(dflag flags, 
	_PLlist from,   char from_type, int from_vi_length, int *from_vis, 
	char *to_cname, int to_vi_length,   int *to_vis, ...)
{
	_PLlist 			to = pl_group_get_list(
								gl_dx->current_graphic, to_cname);
	_PLproperty 	prop_to, prop_from;
	valtype		  *values_from;
	va_list 	 		ap;
	char 			  *pfrom_name, *pto_name;
	register int   i;
	unsigned int	val_offset;

	/* initialize machinery to create rSet command */ 
	dx_parse_start(to, &to_vi_length, &to_vis);

	/* get the va_args */
	va_start(ap, to_vis);

	/* loop through the properties... they come
	**	in pairs:
	**		from property name
	**		to   property name (if NULL, use from name)
 	*/
	while(pfrom_name = va_arg(ap, char *)) {

		/* get the properties to write from, to
		** jig and kas changed 10/31/17 to fix bug with 	
		** accessing values_wc
		*/
		prop_from = pl_list_get_prop(from, pfrom_name);
		if(from_type == 'v')
			values_from = prop_from->values;
		else
			values_from = prop_from->values_wc;
		prop_to   = ((pto_name = va_arg(ap, char *)) != NULL) ?
						pl_list_get_prop(to, pto_name) :
						pl_list_get_prop(to, pfrom_name);

		/* loop through the inputs, getting and setting */	
		for(i=0;i<to_vi_length;i++) {
			dx_prop_set(prop_to, to_vis[i], 
				*((valtype *) (values_from + from_vis[i])));
		}
	}

	/* end args */
	va_end(ap);

	/* Make & send it */		
	dx_parse(flags, DX_SET, to, to_vi_length, to_vis);
}

/* PUBLIC ROUTINE: dx_setl_by_index
**
** 'setl' is set from another list
*/
void dx_setl_by_index(_PLlist from, char from_type, 
	int from_pi, int from_vi, int to_i, int to_pi, int to_vi)
{
	_PLlist 		to 		 = gl_dx->current_graphic->lists[to_i];
	_PLproperty to_prop	 = to->properties[to_pi],
					from_prop = from->properties[from_pi];

	dx_set(gl_dx->draw_flags, to, to_prop, to_vi, 
				from_type == 'v' ? 
				from_prop->values[from_vi] :
				from_prop->values_wc[from_vi]);
}

/* PUBLIC ROUTINE: dx_setl_by_indexIV
**
** 'setl' is set from another list
*/
void dx_setl_by_indexIV(dflag flags,
	_PLlist from, char from_type, int from_vi_length, int *from_vis, 
	int to_i, int to_vi_length, int *to_vis, ...)
{
	_PLlist 			to = gl_dx->current_graphic->lists[to_i];
	_PLproperty 	prop_to, prop_from;
	va_list 	 		ap;
	register int   i;
	int				pfrom_i, pto_i;
	unsigned int	val_offset;

	/* get value/wc */
	if(from_type == 'v')
		val_offset = (unsigned int) ((_PLproperty) NULL)->values;
	else
		val_offset = (unsigned int) ((_PLproperty) NULL)->values_wc;

	/* initialize machinery to create rSet command */ 
	dx_parse_start(to, &to_vi_length, &to_vis);

	/* get the va_args */
	va_start(ap, to_vis);

	/* loop through the properties... they come
	**	in pairs:
	**		from property index
	**		to   property index (if negative, use from name)
 	*/
	while((pfrom_i = va_arg(ap, int)) != ENDI) {

		/* get the properties to write from, to */
		prop_from = from->properties[pfrom_i];
		prop_to   = ((pto_i = va_arg(ap, int)) >= 0) ?
						to->properties[pto_i] :
						to->properties[pfrom_i];

		/* loop through the inputs, getting and setting */	
		for(i=0;i<to_vi_length;i++)
			dx_prop_set(prop_to, to_vis[i], 
				*((valtype *) (prop_from + val_offset + from_vis[i])));
	}

	/* end args */
	va_end(ap);

	/* Make & send it */		
	dx_parse(flags, DX_SET, to, to_vi_length, to_vis);
}

/* PUBLIC ROUTINE: dx_on_by_name
**
*/
void dx_on_by_name(char *class_name, int ecode, int vi)
{
	_PLlist 		list = pl_group_get_list(
							gl_dx->current_graphic, class_name);
	_PLproperty prop = pl_list_get_prop(list, "visible");

	gl_dx->ecode = ecode;
	dx_set(gl_dx->draw_flags, list, prop, vi, 1);
}

/* PUBLIC ROUTINE: dx_on_by_nameI
**
*/
void dx_on_by_nameI(dflag flags,
	char *class_name, int ecode, int vi_length, int *vis)
{
	_PLlist 		list = pl_group_get_list(
							gl_dx->current_graphic, class_name);
	_PLproperty prop = pl_list_get_prop(list, "visible");
	register int i;

	/* set the ecode (dropped when return value received) */
	gl_dx->ecode = ecode; 

	/* Start the parse machinery */
	dx_parse_start(list, &vi_length, &vis);

	/* Set all to 1 */
	dx_prop_set_many(prop, vi_length, vis, 1);

	/* Make & send it, with return message */
	dx_parse(flags, DX_SET, list, vi_length, vis);
}	

/* PUBLIC ROUTINE: dx_on_by_index
**
** Set "visible" property to 1 of given (indexed) 
**		graphics object.
**	Arguments are <long> and return is <int> so this
**		can be called directly from a state set
*/
int dx_on_by_index(long class_index, long ecode, long vi)
{
	_PLlist 		list = gl_dx->current_graphic->lists[class_index];
	_PLproperty prop = pl_list_get_prop(list, DX_VIS);

	/* set the ecode (dropped when return value received) */
	gl_dx->ecode = ecode; 

	/* call dx_set to do the work */
	dx_set(gl_dx->draw_flags, list, prop, vi, 1);

	/* prevent a code from being dropped if called from a state set */
	return(0);
}

/* PUBLIC ROUTINE: dx_on_by_indexI
**
** Set "visible" property to 1 of given (indexed) 
**		graphics objects.
*/
void dx_on_by_indexI(dflag flags, int class_index, int ecode,
	int vi_length, int *vis)
{
	_PLlist 		list = gl_dx->current_graphic->lists[class_index];
	_PLproperty prop = pl_list_get_prop(list, DX_VIS);
	register int i;

	/* set the ecode (dropped when return value received) */
	gl_dx->ecode = ecode; 

	/* Start the parse machinery */
	dx_parse_start(list, &vi_length, &vis);

	/* Set all to 1 */
	dx_prop_set_many(prop, vi_length, vis, 1);

	/* Make & send it, with return message */
	dx_parse(flags, DX_SET, list, vi_length, vis);
}	

/* PUBLIC ROUTINE: dx_off_by_name
**
*/
void dx_off_by_name(char *class_name, int ecode, int vi)
{
	_PLlist 		list = pl_group_get_list(
							gl_dx->current_graphic, class_name);
	_PLproperty prop = pl_list_get_prop(list, "visible");

	/* set the ecode (dropped when return value received) */
	gl_dx->ecode = ecode; 

	/* call dx_set to do the work */
	dx_set(gl_dx->draw_flags, list, prop, vi, 0);
}

/* PUBLIC ROUTINE: dx_off_by_nameI
**
*/
void dx_off_by_nameI(dflag flags, 
	char *class_name, int ecode, int vi_length, int *vis)
{
	_PLlist 		list = pl_group_get_list(
							gl_dx->current_graphic, class_name);
	_PLproperty prop = pl_list_get_prop(list, "visible");
	register int i;

	/* set the ecode (dropped when return value received) */
	gl_dx->ecode = ecode; 

	/* Start the parse machinery */
	dx_parse_start(list, &vi_length, &vis);

	/* Set all to 1 */
	dx_prop_set_many(prop, vi_length, vis, 0);

	/* Make & send it, with return message */
	dx_parse(flags, DX_SET, list, vi_length, vis);
}	

/* PUBLIC ROUTINE: dx_off_by_index
**
** Set "visible" property to 0 of given (indexed) 
**		graphics object.
**	Arguments are <long> and return is <int> so this
**		can be called directly from a state set
*/
int dx_off_by_index(long class_index, long ecode, long vi)
{
	_PLlist 		list = gl_dx->current_graphic->lists[class_index];
	_PLproperty prop = pl_list_get_prop(list, "visible");

	/* set the ecode (dropped when return value received) */
	gl_dx->ecode = ecode; 

	/* call dx_set to do the work */
	dx_set(gl_dx->draw_flags, list, prop, vi, 0);

	/* prevent code from being dropped if called from a state set */
	return(0);
}

/* PUBLIC ROUTINE: dx_off_by_indexI
**
*/
void dx_off_by_indexI(dflag flags, int class_index, 	
	int ecode, int vi_length, int *vis)
{
	_PLlist 		list = gl_dx->current_graphic->lists[class_index];
	_PLproperty prop = pl_list_get_prop(list, "visible");
	register int i;

	/* set the ecode (dropped when return value received) */
	gl_dx->ecode = ecode; 

	/* Start the parse machinery */
	dx_parse_start(list, &vi_length, &vis);

	/* Set all to 1 */
	dx_prop_set_many(prop, vi_length, vis, 0);

	/* Make & send it, with return message */
	dx_parse(flags, DX_SET, list, vi_length, vis);
}	

/* PUBLIC ROUTINE: dx_parse_start
**
** Resets fields of the _DXgraphics_struct
**		used to store information about the 
**		dx command to be built.
*/
void dx_parse_start(_PLlist list, int *length, int **indices)
{
#define VISZ 64
	register int i, *ic;
	static   int vis[VISZ], first_time = 1;

	/* make static array of indices */
	if(first_time) {

		first_time = 0;
	
		for(i=0;i<VISZ;i++)
			vis[i] = i;
	}

	/* check for indices ... */
	if(!*indices) {

		/* treat *length as index */
	 	if(*length >= 0) {
	
			*indices = &(vis[*length]);
			*length  = 1;	

		/* use all indices from list */
		} else if (list) {

			*indices = vis;
			*length  = list->values_length;

		} else {

			printf("dx_parse_start: error\n");
			return;
		}
	}
	
	/* Now reset the "change" fields in gl_dx.
	**	Note that p2change is an array of _PLproperty
	**		pointers; just resetting the "length" (i.e.,
	**		number of pointers added to the array)
	**		resets the list.
	**	Conversely, v2change is an array of indices.
	**	I really don't feel like explaining more right
	**		now.
	*/
	gl_dx->p2change_length = 0;
	for(ic=(*indices),i=(*length)-1;i>=0;i--)
		gl_dx->v2change[ic[i]] = 0;
}

/* PUBLIC ROUTINE: dx_parse
**
** The Big Kahuna -- creates and sends the dotsX
**		command string, the evaluable string sent to MATLAB
**
** Arguments:
** Treat vi_length and vis as elsewhere:
**		if both are given, vi_length is the length
**			of the array vis, which contains indices
**			of values (i.e., indices into list->property[i]->values)
**		otherwise, if vi_length is negative, use
**			all values in the array
**		otherwise, use vi_length as a single index
**			
*/
void dx_parse(dflag flags, const char *cmd, 
	_PLlist list, int vi_length, int *vis)
{
#define STC(ct) strcpy(&(gl_dx->message_buf[strlen(gl_dx->message_buf)-1]), (ct))

	register int i,j;
	int changed = 0, cis[32], *cptr;
	_PLproperty prop;
	char *start = &(gl_dx->message_buf[strlen(gl_dx->message_buf)]);

	/* check flags ... use default */
	if(flags == DXF_DEFAULT)
		flags = gl_dx->draw_flags;

	/* first check if any properties changed */
	if(!gl_dx->p2change_length)
		return;

	/* check for "append" flag */
	if(!(flags&DXF_APPEND))
		gl_dx->message_buf[0] = '\0';

	/* Check for special case of rAdd */
	if(!strcmp(cmd, DX_ADD)) {

		/* Syntax is:
		**		rAdd(<num>, ...);
		**			where <num> is the number of objects to add
		*/
		sprintf(gl_dx->message_buf, "%s%s('%s',%d,", 
				gl_dx->message_buf, cmd, list->name, list->values_length);

		/* mark all values as changed */
		changed = list->values_length;
		cptr 	  = vis;

	} else {

		/* rSet command, syntax:
		**		rSet([i, j, ..], ... );
		*/

		/* loop through the indices */
		for(i=0;i<vi_length;i++) {

			/* Check whether the values associated with
			**		the given index have changed
			*/
			if(gl_dx->v2change[vis[i]]) {

				/* start the command */
				if(!changed)
					sprintf(gl_dx->message_buf, "%s%s('%s',[", 
							gl_dx->message_buf, cmd, list->name);

				/* add the indices (remember to add 1 from C->Matlab) */
				sprintf(gl_dx->message_buf, "%s%d,", gl_dx->message_buf, vis[i]+1);
		
				/* update the cis array (changed indices) */
				cis[changed++]= vis[i];
			}

			/* set cptr to the array of changed indices */
			cptr = cis;
		}
		
		/* If nothing changed, clear the buffer and return */
		if(!changed) {
			*start = '\0';	
			return;
		}

		/* add "]," */
		STC("],");
	}

	/* loop through the properties */
	for(i=0;i<gl_dx->p2change_length;i++) {

		/* get the property */
		prop = gl_dx->p2change[i];

		/* add the name */
		if(changed == 1)
			sprintf(gl_dx->message_buf, "%s'%s',", gl_dx->message_buf, prop->name);
		else
			sprintf(gl_dx->message_buf, "%s'%s',{", gl_dx->message_buf, prop->name);

		/* loop through the values */
		for(j=0;j<changed;j++)
			sprintf(gl_dx->message_buf, "%s%.2lf,", 
				gl_dx->message_buf, PL_PV(prop, cptr[j]));

		/* possibly add end of cell array */		
		if(changed != 1)
			STC("},");
	}

	/* finish the command */
	STC(");");

	/* Just store it; do not send */
	if(flags&DXF_STORE)
		return;

	/* Conditionally add draw flag */
	if(!(flags&DXF_NO_DFLAG))
		sprintf(gl_dx->message_buf, "%sdraw_flag=%d;", gl_dx->message_buf, 
			(int) ( (flags&DXF_DFLAG0) ? 0 :
				((flags&(DXF_NO_CLEAR|DXF_DRAW_ONCE))+1)));

	/* Send it */ 
	dx_send_message_buf();

	/* clear buffer */
	gl_dx->message_buf[0] = '\0';
}

/* PUBLIC ROUTINE: dx_prop_set
**
*/
void dx_prop_set(_PLproperty prop, int vi, valtype value)
{
	/* check for good index and new value */
	if(vi < 0 || vi >= prop->values_length ||
		value == PL_NOVAL || prop->values[vi] == value)
		return;

	
	/* set it */
	prop->values[vi] = value;

	/* register property as changed */
	if(gl_dx->p2change_length == 0 ||	
		gl_dx->p2change[gl_dx->p2change_length-1] != prop)
		gl_dx->p2change[gl_dx->p2change_length++] = prop;

	/* register index as changed */
	gl_dx->v2change[vi] = 1;
}

/* PUBLIC ROUTINE: dx_prop_set_many
**
*/
void dx_prop_set_many(_PLproperty prop, int vi_length, 
		int *vis, valtype value)
{
	int i, changed = 0;

	/* check for good (new) value */
	if(value == PL_NOVAL)
		return;

	/* normal vi_length/vis nonsense */	
	if(!vis) {

		/* just one index */
		if(vi_length >= 0) {
			dx_prop_set(prop, vi_length, value);
			return;
		}

		/* loop through all the values */
		for(i=0;i<prop->values_length;i++) {

			/* conditionally set it, mark as changed */
			if(prop->values[i] != value) {
				changed 				 = 1;
				prop->values[i] 	 = value;
				gl_dx->v2change[i] = 1;
			}
		}
	
	} else {

		/* loop through given indices */
		for(i=0;i<vi_length;i++) {

			/* conditionally set it, mark as changed */
			if(prop->values[vis[i]] != value) {
				changed 						= 1;
				prop->values[vis[i]] 	= value;
				gl_dx->v2change[vis[i]]	= 1;
			}
		}
	}

	/* register property as changed */
	if(changed && (gl_dx->p2change_length == 0 ||	
		gl_dx->p2change[gl_dx->p2change_length-1] != prop))
		gl_dx->p2change[gl_dx->p2change_length++] = prop;

}

/* PUBLIC ROUTINE: dx_init
**
**	jig changed 5/31/06 to store a pointer to
**		the initialized struct in gl_dx, in addition
**		to returning the pointer (presumably to be stored
**		in a _PRrecord_struct).
**
*/
_DXgraphics dx_init(void)
{
	if(gl_dx) {
		printf("dx_init: already done. This should not happen.\n");
		return NULL;
	}

	/* Initialize the struct */
	gl_dx	= SAFE_STALLOC(_DXgraphics_struct);

	gl_dx->graphics_length 	= 0;
	gl_dx->graphics			= NULL;
	gl_dx->current_graphic 	= NULL;
	gl_dx->fp 					= SAFE_STALLOC(_DXfp_struct);
	gl_dx->mon_horiz_cm		= 0;
	gl_dx->view_dist_cm		= 0;
	gl_dx->time_offset		= 0;
	gl_dx->is_fixating		= -1;
	gl_dx->draw_flags			= DXF_D1;
	gl_dx->p2change_length	= 0;
	gl_dx->waiting				= 0;
	gl_dx->ecode				= 0;
	gl_dx->cmd_fp				= NULL;
	gl_dx->cmd_index			= 0;
	
	return(gl_dx);	
}

/* PUBLIC ROUTINE: dx_make_graphic
**
*/
_PLgroup dx_make_graphic(_PLinit names)
{
	static _PLgroup registered_graphics = NULL;
	_PLgroup graphic;

	/* make persistent graphics_group struct */
	if(!registered_graphics)
		registered_graphics = pl_init_to_group(gl_graphics);

   /* initialize the group struct */
   graphic = pl_group_init(NULL, 0);

   /* loop through the graphics objects, adding to the
   **    task graphics struct
   */
   while(names->name != NULL) {

      /* 1. Get the graphic object list from the graphics_group
      ** 2. Make a copy
      ** 3. Add it to the group
      */
      pl_group_add_list(graphic, 
			pl_list_copy(
            pl_group_get_list(registered_graphics, names->name), 
				names->value));
		
		/* increment the pointer */
		names++;
   }

	return(graphic);
}

/* PUBLIC ROUTINE: dx_add_graphic
**
*/
void dx_add_graphic(_DXgraphics dx, _PLgroup graphic)
{
	char buf[256];

	if(!dx || !graphic)
		return;

	/* allocate a new pointer in the array */
	if(!dx->graphics_length)
		dx->graphics = (_PLgroup *) SAFE_MALLOC(sizeof(_PLgroup));
	else
		dx->graphics = (_PLgroup *) SAFE_REALLOC(dx->graphics,
				((dx->graphics_length + 1) * sizeof(_PLgroup)));

	/* Set the name: G*, where * is the index of the 
	**		graphic we're about to add. This is just 
	**		to ensure that all graphics groups
	**		have unique names that can be used 
	**		as DotsX group names.	
	*/
	sprintf(buf, "G%d", dx->graphics_length);
	graphic->name = TOY_CARBONCOPY(buf);

	/* set it (as current) and update the count */
	dx->current_graphic = 
			dx->graphics[dx->graphics_length++] = graphic;
}

/* PUBLIC ROUTINE: dx_set_graphic
**
**	Sets the given graphic group as "current".
**
** Arguments:
**		dx 	  ... the _DXgraphics struct holding all the graphics groups
**		graphic ... the _PLgroup to set
**
** Returns:
**		int 	  ... flag indicating whether the current group
**						was changed (1) or not (0). Used by
**						dx_add_graphic.
*/
int dx_set_graphic(_DXgraphics dx, _PLgroup graphic)
{
	char 		buf[256];
	register int i=0, j;
	int		listi, vali;
	_PLlist	dXt;

	/* didn't change current graphic */
	if(!dx || !graphic || (dx->current_graphic == graphic))
		return(0);

	/* set it */
	dx->current_graphic = graphic;

	/* send rGroup command */
	sprintf(gl_dx->message_buf, "%s('%s');", DX_GROUP, graphic->name);
	dx_send_message_buf();

	/* set default object indices (set to 
	**		dXtarget objects incrementally, then
	**		dXdots, then nothing
	*/
	if((listi=pl_group_get_listi(dx->current_graphic, DX_TARGET)) != -1) {
		dXt = dx->current_graphic->lists[listi];
		for(i=0;i<dXt->values_length;i++) {
			dx->object_indices[i][0] = listi;
			dx->object_indices[i][1] = i;
		}
	}
	if((listi=pl_group_get_listi(dx->current_graphic, DX_DOTS)) != -1) {
		dXt = dx->current_graphic->lists[listi];
		for(j=0;j<dXt->values_length;i++,j++) {
			dx->object_indices[i][0] = listi;
			dx->object_indices[i][1] = j;
		}
	}
	/* fill in the rest */
	for(;i<DX_MAX_INDICES;i++) {
		dx->object_indices[i][0] = 0;
		dx->object_indices[i][1] = 0;
	}

	/* return true */
	return(1);
}

/* PUBLIC ROUTINE: dx_print_dx
**
*/
void dx_print_dx(_DXgraphics dx)
{
	register int i;

	if(!dx)
		return;

	printf(" ## DX GRAPHICS, %d groups:\n",
		dx->graphics_length);

	for(i=0;i<dx->graphics_length;i++)
		pl_group_print(dx->graphics[i]);
}

/* PUBLIC ROUTINE: dx_free
**
*/
void dx_free(_DXgraphics dx)
{
	register int i;

	if(!dx)
		return;

	/* possibly close the udp commands file */
	if(dx->cmd_fp)
		fclose(dx->cmd_fp);

	/* free each graphics group */
	for(i=0;i<dx->graphics_length;i++)
		pl_group_free(dx->graphics[i]);

	/* free the rest */
	SAFE_FREE(dx->fp);
	SAFE_FREE(dx->graphics);
	SAFE_FREE(dx);
}	

/* PUBLIC ROUTINE: dx_change_target
**
** Routine for changing either color or diamter of 1/2 targets
** The routine only modifies targets that are already visible
**
** Arguments:
**          index*   ...   index into gl_dx->object_indices
**          clut*    ...   determines what color to change the target to
**          diam*    ...   change target diameter to this value
*/
int dx_change_target(int index0, int clut0, int diam0,
      int index1, int clut1, int diam1)
{
	int targi[2] = {index0, index1};

	_PLlist list = pl_group_get_list(
      gl_dx->current_graphic, DX_TARGET);

	/* get visibility of both objects first */
	valtype vis0 = pl_list_get_vi(list, "visible", index0);
	valtype vis1 = pl_list_get_vi(list, "visible", index1);

   if(index0 < 0 && index1 < 0)
      return;

	if(index0 < 0 || vis0 != 1)
		return(dx_change_target1(index1, clut1, diam1));

	if(index1 < 0 || vis1 != 1)
		return(dx_change_target1(index0, clut0, diam0));

	dx_set_by_nameIV(DXF_DEFAULT, DX_TARGET, 2, targi,
		DX_CLUT, clut0, clut1,
		DX_DIAM, diam0, diam1,
		NULL);
	
	ec_send_code_hi(TARGONCD);

	return(0);

}

/* PUBLIC ROUTINE: dx_change_target1
**
** Sub-routine for changing either color or diamter of 1 targets
** The routine only modifies targets that are already visible
**
** Arguments:
**          index*   ...   index into gl_dx->object_indices
**          clut*    ...   determines what color to change the target to
**          diam*    ...   change target diameter to this value
*/
int dx_change_target1(int index0, int clut0, int diam0)
{

	_PLlist list = pl_group_get_list(
                  gl_dx->current_graphic, DX_TARGET);

	/* get visibility of both objects first */
	valtype vis0 = pl_list_get_vi(list, "visible", index0);
   
	if(index0 < 0)
      return;

	if(vis0 !=1)
		return;

   dx_set_by_nameIV(DXF_DEFAULT, DX_TARGET, index0, NULL,
      DX_CLUT, clut0,
      DX_DIAM, diam0,
      NULL);

	ec_send_code_hi(TARGONCD);

   return(0);

}
