/* rewcalib.d
**
** simple paradigm to calibrate the total reward time to actual reward volume
** Long Ding 2007-03-15
*/

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include "../hdr/rexHdr.h"
#include "paradigm_rec.h"
#include "ldev.h"
#include "lcode.h"

#define REWDIO 	Dio_id(PCDIO_DIO, 4, 0x2)

int numrew = 1000;
int rewcount = 0;
int totaltime = 0;
int rewsize = 100;
int flag = 0;
int flagControl = 0;
char hm_sv_vl[] = "";

void rinitf(void)
{

}

int setrew()
{
	set_times("rewardon", rewsize, -1);
	return(0);
}

int update(void)
{
	rewcount++;
	totaltime = totaltime + rewsize;
	if (rewcount>=numrew) 	flag = 1;
}


VLIST state_vl[] = {
	{"rewsize", &rewsize, NP, NP, 0, ME_DEC},
	{"numrew",	&numrew, NP, NP, 0, ME_DEC},
	{"timed0push1",	&flagControl, NP, NP, 0, ME_DEC},
	NS,
};

MENU umenus[] = {
	{"state_vl", &state_vl, NP, NP, 0, NP, hm_sv_vl},	
	NS,
};

USER_FUNC ufuncs[] = {
	
};
	 	
RTVAR rtvars[] = {
	{"totalnum", &rewcount},
	{"totaltime", &totaltime}, 
	{"flag", &flag}, 
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
		do setrew()
		to rewardon on +0 = flag
		to loop
	rewardon:
		do dio_on(REWDIO)
		to rewbitoff1
	rewbitoff1:
		do dio_off(REWDIO)
		to rewbiton2 on + 1 = flagControl
		to finish on +0 = flagControl
	rewbiton2:
		do dio_on(REWDIO)
		to rewardoff
	rewardoff:
		do dio_off(REWDIO)
		to finish
	finish:
		do update()
		to loop		
}