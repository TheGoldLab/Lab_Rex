/* TIMER.H
*
*	header file for TIMER.C, a collection of c-routines
*	to be called from REX to run timers within the state set
*/

#ifndef TIMERLT_H_
#define TIMERLT_H_

int  timer_set1		(long, long, long, long, long, long);
int  timer_set2		(long, long, long, long, long, long);
int  timer_set3		(long, long, long, long, long, long);
int  timer_set4		(long, long, long, long, long, long);
int  timer_set5		(long, long, long, long, long, long);

int  timer_continue1	(long, long);
int  timer_continue2	(long, long);
int  timer_continue3	(long, long);
int  timer_continue4	(long, long);
int  timer_continue5	(long, long);

int  timer_check1		(void);
int  timer_check2		(void);
int  timer_check3		(void);
int  timer_check4		(void);
int  timer_check5		(void);
 
int  timer_pause		(long);

void timer_tic			(long);
void timer_toc			(long);
long timer_tocR		(long);
void timer_tocu		(long);
long timer_tocuR		(long);

long timer_msec		(void);

#endif /* TIMER_H_ */
