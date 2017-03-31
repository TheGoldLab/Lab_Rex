/* DOTSX.H
*
*	header file for DOTSX.c, c-routines for
*		communicating with and controlling DotsX
*		graphics routines on the Mac.
*
*	5/31/05 jigold made a global pointer to the
*		current dx struct; this allows calls to be
*		made without referencing the struct.
*/

#ifndef DOTSX_H_
#define DOTSX_H_

#include "property_list.h"

/* PUBLIC DEFINES */
#define DX_CSIZE			256 /* array sizes in _DXgraphics */
#define DX_NO_MSG			0	 /* returned by dx_check 		 */
#define DX_MSG				1	 /* returned by dx_check 		 */
#define DX_MAX_INDICES	16	 /* indexed in gl_dx				 */	

	/* DotsX property names */
#define DX_VIS			"visible"
#define DX_X			"x"
#define DX_Y			"y"
#define DX_DX			"dx"
#define DX_DY			"dy"
#define DX_R			"r"
#define DX_T			"t"
#define DX_DR			"dr"
#define DX_DT			"dt"
#define DX_VERTEX		"vertex"
#define DX_WRT			"wrt"
#define DX_DIAM		"diameter"
#define DX_CLUT		"color"
#define DX_COH			"coherence"
#define DX_DIR			"direction"
#define DX_SPEED		"speed"
#define DX_SEED		"seed"
#define DX_DENSITY	"density" 
#define DX_SHAPE     "cmd"
#define DX_FREQ      "frequency"
#define DX_DUR       "duration"
#define DX_GAIN      "gain"

	/* DotsX class names */
#define DX_TARGET		"dXtarget"
#define DX_TARGETS	"dXtargets"
#define DX_OTHER		"dXother"
#define DX_DOTS		"dXdots"
#define DX_CORNER		"dXcorner"
#define DX_BEEP      "dXbeep"

	/* DotsX command names */
#define DX_ADD			"rAdd"
#define DX_SET			"rSet"
#define DX_GROUP		"rGroup"
#define DX_BLANK		"rGraphicsBlankRex"
#define DX_PLAY      "rSoundPlay"

	/* buffering messages */
#define DX_CMD_BUFLEN	256

/* PUBLIC DATA TYPES */

	/* draw flags ... see dx_parse */
typedef unsigned int 					dflag; 

	/* fixation point information -- for making
	** 	funky, changing spots
	*/
typedef struct _DXfp_struct *_DXfp;
struct _DXfp_struct {
	int         num_reps;
   int         rep_count;
   int         ds[32];
   int         cs[32];

   _PLlist     fp;
	int			vind;

   _PLproperty visible;
   _PLproperty diameter;
   _PLproperty clut;
};

	/* the big kahuna */
typedef struct _DXgraphics_struct *_DXgraphics;
struct _DXgraphics_struct {

	/* array of graphics groups */
	int		   graphics_length;
	_PLgroup   *graphics;

	/* pointer to current group */
	_PLgroup	 	current_graphic;

	/* Indices of objects to use to position 
	**	fixation window. Each index corresponds
	**	to a 2 (column) vector of the list and
	**	value index in graphics (above) of the
	**	referenced object. E.g., 
	**		target_indices = {{0,1}, {1,2}}
	**		means that 
	**			index 0 corresponds to list 0, value 1
	**			index 1 corresponds to list 1, value 2
	*/
	int			object_indices[DX_MAX_INDICES][2];

	/*
	** Global fp struct, for chagning fp diameter/color
	**    dynamically until fixation. See dx_change_fp, below.
	*/
	_DXfp			fp;

	/* monitor parameters */
	int		 	mon_horiz_cm;
	int		 	view_dist_cm;

	/* time offset in ms, wrt Mac "GetSecs()" (see
	**	dx_setup() for details)
	*/
	long 			time_offset;

	/* Fixation flag ... keeps track of whether or not to
	**    monitor fixation ... it's kept here because typically
	**    it will get set in dx_position_window, which can place
	**    the eye window based on graphical object locations
	*/
	int			is_fixating;

	/* flags used when sending messages */
	dflag			draw_flags;

	/* string buffers to hold messages to send 
	**	message_buf holds the impending message
	**	
	*/
	char		 	message_buf[512];

	/* Arrays of properties, values to change...
	**	p2change is an array of _PLproperty (pointers)
	**		to objects with values that have been
	**		changed.
	**	v2change is an array of indices corresponding
	**		to values that have changed.
 	*/
	int 			p2change_length;
	_PLproperty p2change[DX_CSIZE];
	int		 	v2change[DX_CSIZE];

	/* UDP Communications
	**
	** These variables control rex<->mac communication
	**	via udp, including buffering all commands
	**	and dumping them into a time- and date-stamped
	**	file at the end of each trial.
	*/

	/* Flag indicating whether we're (still) waiting for
	**	a return message
	*/
	int			waiting;

	/* Ecode to drop upon receipt of return
	** message
	*/
	int			ecode;

	/* Buffers to hold commands until file dump
	*/
	FILE		  *cmd_fp;
	char			cmd_buf[DX_CMD_BUFLEN][512];
	int			cmd_type[DX_CMD_BUFLEN];
	int			cmd_index;
};

#define DX_OI(i)			(gl_dx->object_indices[(i)][(0)])
#define DX_VI(i)			(gl_dx->object_indices[(i)][(1)])
#define DX_OBJ_BYI(i)	(gl_dx->current_graphic->lists[DX_OI(i)])
#define DX_CHKI(i)	(gl_dx->current_graphic && ((i) >= 0) && ((i) < DX_MAX_INDICES) && \
					(DX_OI(i) < gl_dx->current_graphic->lists_length) && \
					(DX_VI(i) < DX_OBJ_BYI(i)->values_length))

	/* flags */
#define DXF_NO_CLEAR		0001		/* don't clear buffer on draw 		*/
#define DXF_DRAW_ONCE 	0002		/* draw only once 						*/
#define DXF_DFLAG0		0004		/* do not draw (draw_flag=0) 			*/
#define DXF_NO_DFLAG		0010		/* do not send draw_flag 				*/
#define DXF_APPEND		0020		/* append to stored message 			*/
#define DXF_STORE			0040		/* don't send -- just store message */
#define DXF_NO_RFLAG		0100		/* don't send return_flag 				*/
#define DXF_DEFAULT		0200		/* use default (global) flags 		*/

	/* these are "standard" flags ... D1-D4 correspond to draw_flag
	** values defined in rRemoteClient (on the Mac, silly)
	*/
#define DXF_NODRAW		DXF_DFLAG0|DXF_NO_RFLAG			/* do NOT draw, NO return flag */
#define DXF_D1				0										/* draw, clear buffer, return flag  			*/
#define DXF_D2				DXF_NO_CLEAR						/* draw, do NOT clear buffer, return flag  	*/
#define DXF_D3				DXF_DRAW_ONCE						/* draw ONCE, clear buffer, return flag  */
#define DXF_D4				DXF_DRAW_ONCE|DXF_NO_CLEAR		/* draw ONCE, do NOT clear buffer, return flag  */

/* PUBLIC ROUTINE PROTOTYPES */

	/* Utilities for settings/showing/hiding graphics
	**		objects. These are the routines that should
	**		be called from tasks and state sets.
	*/
void			dx_setup					(int, int);
void			dx_set_index			(int, int, int);
void			dx_set_ecode			(int);
_PLlist		dx_get_object			(char *);
void			dx_print					(void);

void			dx_set_flags			(dflag);
void			dx_add_flags			(dflag);
void			dx_remove_flags		(dflag);
dflag			dx_get_flags			(void);

void			dx_send_message_buf	(void);
void			dx_send					(char *);
int		   dx_check					(void);
void			dx_dump					(int);

int			dx_blank					(long, long);
int			dx_abort					(void);

void			dx_add					(_PLlist);
void			dx_set					(dflag, _PLlist, _PLproperty, int, valtype);
void			dx_setV					(dflag, _PLlist, int, int *, ...);

int         dx_set1              (long, long, long, long, long);

int			dx_show_fp				(long, long, long, long, long, long);
int			dx_change_fp			(void);
int			dx_hide_fp				(long);

int			dx_toggle1				(long, long, long, long, long, long);
int			dx_toggle2				(long, long, long, long, long, long);
int			dx_toggle3				(long, long, long, long, long);

int			dx_position_window	(long, long, long, long, long);
int			dx_check_fix			(void);
int			dx_set_fix				(long);

int			dx_compare_times		(char *);
int			dx_change_target 		(int, int, int, int, int, int);
int         dx_change_target1    (int, int, int);

int         dx_beep              (long);

	/* SET COMMANDS
	**
	**	These are the critical dotsX commands --
	**		they (hopefully) provide a reasonably
	**		complete interface for creating "rSet"
	**		commands to send to the mac, using 
	**		objects stored in gl_dx
	*/
	/* set */
void			dx_set_by_name			(char *, char *, int, valtype);
void			dx_set_by_nameIV		(dflag, char *, int, int *, ...);
void			dx_set_by_index		(int, int, int, valtype);
void			dx_set_by_indexIV		(dflag, int, int, int *, ...);

void        dx_set_array_by_nameIV(dflag, int, int, ...);

	/* conditionalized set */
void			dx_setc_by_name		(char *, char *, int, int, valtype);
void			dx_setc_by_nameIV		(dflag, char *, int, int *, ...);
void			dx_setc_by_index		(int, int, int, int, valtype);
void			dx_setc_by_indexIV	(dflag, int, int, int *, ...);

	/* set from working copies */
void			dx_setwIA				(dflag, _PLlist, int, int *);
void			dx_setw_by_name		(char *, char *, int);
void			dx_setw_by_nameIV		(dflag, char *, int, int *, ...);
void			dx_setw_by_index		(int, int, int);
void			dx_setw_by_indexIV	(dflag, int, int, int *, ...);

	/* set from another list -- value or working copy */
void			dx_setl_by_name		(_PLlist, char, char *, int, 
											 char *, char *, int);
void			dx_setl_by_nameIV		(dflag, _PLlist, char, int, int *,
											 char *, int, int *, ...);
void			dx_setl_by_index		(_PLlist, char, int, int, 
											 int, int, int);
void			dx_setl_by_indexIV	(dflag, _PLlist, char, int, int *,
											 int, int, int *, ...);

	/* quick turn on/off */
void			dx_on_by_name			(char *, int, int);
void			dx_on_by_nameI			(dflag, char *, int, int, int *);
int			dx_on_by_index			(long, long, long);
void			dx_on_by_indexI		(dflag, int, int, int, int *);

void			dx_off_by_name			(char *, int, int);
void			dx_off_by_nameI		(dflag, char *, int, int, int *);
int			dx_off_by_index		(long, long, long);
void			dx_off_by_indexI		(dflag, int, int, int, int *);

	/* command parsing (creation) utilities */
void			dx_parse_start			(_PLlist, int *, int **);
void			dx_parse					(dflag, const char *, _PLlist, int, int *);

void			dx_prop_set				(_PLproperty, int, valtype);
void			dx_prop_set_many		(_PLproperty, int, int *, valtype);
	
	/* _DXgraphics_struct utilities.
	**		None of these are likely to be
	**		called from tasks and state sets.
	*/
_DXgraphics dx_init					();
_PLgroup		dx_make_graphic		(_PLinit);
void		 	dx_add_graphic			(_DXgraphics, _PLgroup);
int			dx_set_graphic			(_DXgraphics, _PLgroup);
void 			dx_print_dx				(_DXgraphics);
void			dx_free					(_DXgraphics);

#endif /* PDOTSX_H_ */
