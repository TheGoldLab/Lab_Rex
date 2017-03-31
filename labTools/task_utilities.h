/*
** TASK_UTILITIES.H
**
** Utility functions for tasks.
*/

#ifndef TASK_UTILITIES_H_
#define TASK_UTILITIES_H_

#include "paradigm_rec.h"
#include "../sset/lcode.h"

/* PUBLIC DEFINES */
#define TU_XYRT_LEN 10
#define TU_XYRT_LIST 		\
	{ DX_X,		 0, 0.1 },	\
	{ DX_Y,		 0, 0.1 },	\
	{ DX_DX,		 0, 0.1 },	\
	{ DX_DY,		 0, 0.1 },	\
	{ DX_R,		 0, 0.1 },	\
	{ DX_T,		 0, 0.1 },	\
	{ DX_DR,		 0, 0.1 },	\
	{ DX_DT,		 0, 0.1 },	\
	{ DX_VERTEX,-1, 1.0 },	\
	{ DX_WRT,	-1, 1.0 }

enum {
	kX,
	kY,
	kDX,
	kDY,
	kR,
	kT,
	kDR,
	kDT,
	kVERTEX,
	kWRT
};

enum {
	kTrialBlock,
	kTrialRandomize,
	kTrialRepeat,
	kTrialStaircase,
	kTrialHazard
};

/* PUBLIC DATA STRUCTURES */

/* PUBLIC ROUTINE PROTOTYPES */

	/* make trials 	*/
void		tu_make_trials1D	(_PRtask,	char *, int, int *, float);
void		tu_make_trials2D	(_PRtask,	char *, int, int *, float, 
													char *, int, int *, float);
	/* get trials */
_PRtrial	tu_get_block 				(_PRtask, int, int);
_PRtrial	tu_get_random 				(_PRtask, int, int);
_PRtrial tu_get_random_altRows	(_PRtask, int, int);
_PRtrial	tu_get_stair 				(_PRtask, int, int);

  /* miscellaneous */
void		tu_compute_xyrt		(_PLlist);
int		*tu_compute_dangle 	(int, int, int, int, int);
int		tu_get_seed				(int, int, int);

/*****
****** Taka's get trial and subroutines  
******
****** 2013-06-07
*/
_PRtrial td_get_random_altRows (_PRtask, int);
void     td_make_block (_PRtask, int);
void     td_calc_numTrialCoh(_PRtask, int*, int);

// variable duration Dots
_PRtrial td_get_random_altRows2(_PRtask, int);
void     td_make_block2(_PRtask, int);

// taka1_3.d (flagrepeat with a cap)
_PRtrial td_get_random_altRows3(_PRtask, int); // flag repeat with a cap

// taka1_5.d (a flat distribution for coh)
_PRtrial td_get_random_altRows4(_PRtask, int);     // flat coherence distribution. flag repeat with a cap
void     td_make_block3(_PRtask, int);             // create a flat distribution
void     td_calc_numTrialCoh2(_PRtask, int*, int); // create a biased distribution

// taka1_6.d (arbitrary distribution for coh [0-999])
_PRtrial td_get_random_altRows5(_PRtask, int, int);     
void     td_make_block4(_PRtask, int, int);             // create an arbitrary coh distribution
void     td_calc_numTrialCoh4(_PRtask, int*, int, int); 

_PRtrial td_get_random_altRows6(_PRtask, int, int);    // Conditional flag repeat by RT 

/* debug */
int tu_put_breakpoint(void);

#endif /* TASK_UTILITIES_H_ */
