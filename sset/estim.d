/* estim.d
**
** simple paradigm to deliver stimulation pulses 
** for monkeys sitting in the dark.
** Long Ding 2007-10-02
*/

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include "../hdr/rexHdr.h"
#include "paradigm_rec.h"
#include "ldev.h"
#include "lcode.h"

#define WIND0 0      /* window to compare with eye signal */
#define WIND1 1      /* window for correct target */
#define WIND2 2      /* window for other target in dots task */
   /* added two dummy window to signal task events in rex window */
#define WIND3 3      /* dummy window for fix point */
#define WIND4 4      /* dummy window for target */
#define EYEH_SIG  0
#define EYEV_SIG  1

#define REWDIO 	PR_DIO_ID(2)
#define STIMDIO	PR_DIO_ID(5)		/* input pulse to Grass */
#define FILEDIO	PR_DIO_ID(8)		/* input pulse to open file */

int rewsize = 100;
int numtrial =0;
int waitsac = 300;
char hm_sv_vl[] = "";



VLIST state_vl[] = {
	{"rewsize", &rewsize, NP, NP, 0, ME_DEC},
	{"waitsac",	&waitsac, NP, NP, 0, ME_DEC},
	NS,
};

void rinitf(void)
{

}

int fun_set_trial(void)
{
	set_times("rewardon", rewsize, -1);
	set_times("waitrew", waitsac, -1);
	dio_on(FILEDIO);
	ec_send_code_hi(1005);
}


int fun_dio(long ecode, DIO_ID id, long flag)
{
	if (flag==1)
	{
		dio_on(id);
	}
	else
	{
		dio_off(id);
	}
	if (ecode>0) 
	{
		ec_send_code(ecode);
		ec_send_dio(id);
	}
	return(0);
}

int fun_reward(long ecode, long flag)
{
	fun_dio(ecode, REWDIO, flag);
	return(0);
}

int fun_estim(long ecode, long flag)
{
	fun_dio(ecode, STIMDIO, flag);
	return(0);
}
	
int update(void)
{
	numtrial++;
	ec_send_code_hi(4905);
	dio_off(FILEDIO);
}

MENU umenus[] = {
	{"state_vl", &state_vl, NP, NP, 0, NP, hm_sv_vl},	
	NS,
};

USER_FUNC ufuncs[] = {
	
};
	 	
RTVAR rtvars[] = {
	{"trials", &numtrial},
};



/* THE STATE SET 
*/
%%
id 1001
restart rinitf
main_set {
status ON
begin	first:
		to loop
	loop:
		time 1000
		to pause on +PSTOP & softswitch
		to go
	pause:
		to go on -PSTOP & softswitch
	go:
		do fun_set_trial()
		to wait
	wait:
		do timer_set1(1000, 200, 2000, 1000, 0, 0)
 		to stimon on 1 % timer_check1
	stimon:
		time 5
		do fun_estim(STIMCD, 1)
		rl 10
		to stim_delay
	stim_delay:
		time 100
		to stimoff
	stimoff:
		do fun_estim(0, 0)
		to waitrew
	waitrew:
		to rewardon 

/* for the UW water feeder, toggle method for computer control */
	rewardon:
		rl 20
		do fun_reward(REWCD, 1)
		to rewbitoff1
	rewbitoff1:
		do fun_reward(0, 0)
		to rewbiton2
	rewbiton2:
		rl 0
		do fun_reward(REWOFFCD, 1)
		to rewardoff
	rewardoff:
		do fun_reward(0, 0)
		to finish
	finish:
		do update()
		to loop		
}
