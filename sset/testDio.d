/*	testDio.d.d
**
**	created by jig 7/6/06
*/

#include "rexHdr.h"
#include "paradigm_rec.h"
#include "toys.h"
#include "lcode.h"
#include "ldev.h"

/* PRIVATE data structures */

/* GLOBAL VARIABLES */
static _PRrecord 	gl_rec = NULL; /* KA-HU-NA */

	/* for now, allocate these here... */
MENU 	 	 umenus[30];
RTVAR		 rtvars[15];
USER_FUNC ufuncs[15];

/* ROUTINES */

/*
**** INITIALIZATION routines
*/

/* ROUTINE: autoinit
**
**	Initialize gl_rec. This will automatically
**		set up the menus, etc.
*/
void autoinit(void)
{
	gl_rec = pr_initV(0, 0, 
		umenus, NULL, rtvars, NULL, ufuncs, NULL);
}

/* ROUTINE: rinitf
**
** initialize at first pass or at r s from keyboard 
*/
void rinitf(void)
{
	static int first_time = 1;

	/* This stuff needs to be done only once, but also
	**		needs to be done after the clock has started
	**		(so do NOT put it up in autoinit).
	*/
	if(first_time) {

		/* do this once */
		first_time = 0;

#define WIND0 		0
#define EYEH_SIG 	0
#define EYEV_SIG 	1

		/* initialize interface (window) parameters */
		wd_src_check(WIND0, WD_SIGNAL, EYEH_SIG, WD_SIGNAL, EYEV_SIG);
		wd_src_pos  (WIND0, WD_DIRPOS, 0, WD_DIRPOS, 0);
		wd_cntrl    (WIND0, WD_ON);

		/* init the screen */
		pr_setup();
	}
}

/* THE STATE SET 
*/
%%
id 666
restart rinitf
main_set {
status ON
begin	first:
		to pause

	pause:
		to d1on on -PSTOP & softswitch
	d1on:
		time 500
		do dio_on(FTTL1)
		to d2on
	d2on:
		time 500
		do dio_onoff(FTTL2, FTTL1)	
		to d3on
	d3on:
		time 500
		do dio_onoff(FTTL3, FTTL2)	
		to d4on
	d4on:
		time 500
		do dio_onoff(FTTL4, FTTL3)	
		to d5on
	d5on:
		time 500
		do dio_onoff(FTTL5, FTTL4)	
		to d6on
	d6on:
		time 500
		do dio_onoff(FTTL6, FTTL5)	
		to d7on
	d7on:
		time 500
		do dio_onoff(FTTL7, FTTL6)	
		to d8on
	d8on:
		time 500
		do dio_onoff(FTTL8, FTTL7)	
		to pause

abort list:		
}

