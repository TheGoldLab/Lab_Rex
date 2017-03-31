/* TIMER.C
*
*	a collection of c-routines
*	to be called from REX to run timers within the state set
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include "../hdr/rexhdrs.h"

#include "timerLT.h"
#include "toys.h"

/* PRIVATE DEFINES */
#define NUM_TIMERS 		5
#define NUM_TICS			10

/* PRIVATE ROUTINE PROTOTYPES */
static int timer_set			(long, long, long, long, long, long, long);
static int timer_continue	(long, long, long);
static int timer_check		(long);

/* PRIVATE GLOBALS */
static int gl_time_begin[NUM_TIMERS];
static int gl_time_end[NUM_TIMERS];
static struct timespec gl_tic_start[NUM_TICS];

/* PUBLIC ROUTINES: timer_set
**
**	description: sets a timer from an exponential distribution
**		of values
*/
int timer_set1(long a, long b, long c, long d, long e, long f)
	{ return(timer_set(0,a,b,c,d,e,f)); }
int timer_set2(long a, long b, long c, long d, long e, long f)
	{ return(timer_set(1,a,b,c,d,e,f)); }
int timer_set3(long a, long b, long c, long d, long e, long f)
	{ return(timer_set(2,a,b,c,d,e,f)); }
int timer_set4(long a, long b, long c, long d, long e, long f)
	{ return(timer_set(3,a,b,c,d,e,f)); }
int timer_set5(long a, long b, long c, long d, long e, long f)
	{ return(timer_set(4,a,b,c,d,e,f)); }

/* PRIVATE ROUTINE: timer_set
** 	The workhorse.
**
** Arguments:
**  	probability 	 ... chance that we'll use exponential time
**								([0..1000]: 1000 for always, 0 for never)
**		min_time    	 ... minimum if we use exponential
**		max_time    	 ... maximum if we use exponential
**		mean_time   	 ... mean if we use exponential
**		override_time 	 ... time we use if we don't use exponential
**		override_random ... offset to override_time if used
** 
** Returns:
*/
static int timer_set(long index, long probability, long min_time, 
							long max_time, long mean_time,
							long override_time, long override_random)
{
	int time_interval;
	gl_time_begin[index] = (int) (i_b->i_time);

	if(TOY_RAND(1000) < probability)	

		/* use exponential time 
		** VERSION 1: toy_exp
		** VERSION 2: toy_exp2
		** 	see toys.c for details
		*/
		time_interval = toy_exp(min_time, max_time, mean_time); 

	else										

		/* use override value 	*/
		time_interval = override_time + (override_random ? 
										rand() % override_random : 0);

	/* save the end time in a global variable, used elsewhere */
	gl_time_end[index] = gl_time_begin[index] + time_interval;

	/* Long Ding 2009-07-01 return the time interval */
	return(time_interval);
}

/* PUBLIC ROUTINES: timer_continue
**
**	description: extends the timer to the given value
**	(or does nothing if the current timer is longer than the given
**	value -- this allows you to pass in a 0 & have it not affect
**	anything)
*/
int timer_continue1(long a, long b) { return(timer_continue(0,a,b)); }
int timer_continue2(long a, long b) { return(timer_continue(1,a,b)); }
int timer_continue3(long a, long b) { return(timer_continue(2,a,b)); }
int timer_continue4(long a, long b) { return(timer_continue(3,a,b)); }
int timer_continue5(long a, long b) { return(timer_continue(4,a,b)); }

static int timer_continue(long index, long abs_time, long rel_time )
{
	if(rel_time > 0) {
		gl_time_end[index] += rel_time;
	
	} else {
		int tmp_end = gl_time_begin[index] + abs_time;
	
		if(tmp_end > gl_time_end[index])
			gl_time_end[index] = tmp_end; 
	}
	
	return(0);
}

/* PUBLIC ROUTINES: timer_check
**
**	description: checks if timer is done
*/
int timer_check1(void) { return(timer_check(0)); }
int timer_check2(void) { return(timer_check(1)); }
int timer_check3(void) { return(timer_check(2)); }
int timer_check4(void) { return(timer_check(3)); }
int timer_check5(void) { return(timer_check(4)); }

static int timer_check(long index)
{
	return((i_b->i_time < gl_time_end[index]) ? 0 : MET);
}

/* PUBLIC ROUTINE: timer_pause
**
**	pause for a given number of msec
*/
int timer_pause(long msec)
{
	long diff;
	struct timespec start, now;

	if(clock_gettime(CLOCK_REALTIME, &start) == -1)
		return(0);

	do {
		clock_gettime(CLOCK_REALTIME, &now);

		diff = (now.tv_sec -  start.tv_sec)  * 1000L +
				 (now.tv_nsec - start.tv_nsec) / 1000000L;

	} while(diff < msec);

	return(0);
}

/* PUBLIC ROUTINE: timer_tic
**
*/
void timer_tic(long index)
{
/*
** CLOCK RESOLUTION APPEARS TO BE ~1 ms.
**		(999847 nsec, to be exact.)
**
**	clock_getres(CLOCK_REALTIME, &gl_tic_start);
**	printf("timer tic, res is %ld\n", gl_tic_start.tv_nsec);
*/

	if (index < 0 )
		index = 0;
	if (index >= NUM_TICS)
		index = NUM_TICS - 1;

	if(clock_gettime(CLOCK_REALTIME, &(gl_tic_start[index])) == -1)
		printf("timer_tic: could not get time\n");
}

/* PUBLIC ROUTINE: timer_toc
**
** Prints time, in msec, since "timer_tic()" call
*/
void timer_toc(long index)
{
	struct timespec now;

	if (index < 0 )
		index = 0;
	if (index >= NUM_TICS)
		index = NUM_TICS - 1;

	if(clock_gettime(CLOCK_REALTIME, &now) == -1)
		printf("timer_toc: could not get time\n");

	/* Compute/print elapsed time in MSEC */
	printf("timer_toc: %ld ms\n", 
			(now.tv_sec  - gl_tic_start[index].tv_sec)  * 1000L +
			(now.tv_nsec - gl_tic_start[index].tv_nsec) / 1000000L);
}

/* PUBLIC ROUTINE: timer_tocR
**
**	R for return ... returns time, in msec, since
**		"timer_tic()" call
*/
long timer_tocR(long index)
{
	struct timespec now;

	if (index < 0 )
		index = 0;
	if (index >= NUM_TICS)
		index = NUM_TICS - 1;

	if(clock_gettime(CLOCK_REALTIME, &now) == -1)
		printf("timer_tocR: could not get time\n");

	/* Return time in MSEC */
	return((now.tv_sec  - gl_tic_start[index].tv_sec)  * 1000L +
			 (now.tv_nsec - gl_tic_start[index].tv_nsec) / 1000000L);
}

/* PUBLIC ROUTINE: timer_tocu
**
** Prints time, in msec, since "timer_tic()" call
*/
void timer_tocu(long index)
{
	struct timespec now;

	if (index < 0 )
		index = 0;
	if (index >= NUM_TICS)
		index = NUM_TICS - 1;

	if(clock_gettime(CLOCK_REALTIME, &now) == -1)
		printf("timer_tocu: could not get time\n");

	/* Compute/print elapsed time in USEC */
	printf("timer_tocu: %ld us\n", 
			(now.tv_sec  - gl_tic_start[index].tv_sec)  * 1000000L +
			(now.tv_nsec - gl_tic_start[index].tv_nsec) / 10000L);
}

/* PUBLIC ROUTINE: timer_tocuR
**
**	R for return ... returns time, in usec, since
**		"timer_tic()" call
*/
long timer_tocuR(long index)
{
	struct timespec now;

	if (index < 0 )
		index = 0;
	if (index >= NUM_TICS)
		index = NUM_TICS - 1;

	if(clock_gettime(CLOCK_REALTIME, &now) == -1)
		printf("timer_tocuR: could not get time\n");

	/* Return time in USEC */
	return((now.tv_sec  - gl_tic_start[index].tv_sec)  * 1000000L +
			 (now.tv_nsec - gl_tic_start[index].tv_nsec) / 1000L);
}

/* PUBLIC ROUTINE: timer_msec
**
**	R for return ... returns time, in msec, 
**		since the first time this function is called.
*/
long timer_msec(void)
{
	static int first_time = 1;
	static struct timespec start;
	struct timespec now;

	/* get a reference time */
	if(first_time == 1) {
		first_time = 0;
		if(clock_gettime(CLOCK_REALTIME, &start) == -1)
			printf("timer_sec: could not get time\n");
	}

	if(clock_gettime(CLOCK_REALTIME, &now) == -1)
		printf("timer_sec: could not get time\n");

	return((now.tv_sec  - start.tv_sec)  * 1000L +
			 (now.tv_nsec - start.tv_nsec) / 1000000L);
}
