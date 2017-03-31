/* dioLT.c
**
** description: routines for interfacing with the
** Digital Input/Output board on REX.
*/

/* INCLUDED FILES */
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#include "../hdr/pcmsg.h"
#include "../hdr/rexHdr.h"
#include "../sset/lcode.h"
#include "../sset/ldev.h"

#include "dioLT.h"
#include "ecodeLT.h"
#include "timerLT.h"
#include "toys.h"

/* PRIVATE MACROS */

/* GLOBALS */

/* PUBLIC ROUTINES */

/* DIO */

/* PUBLIC ROUTINES: dio_give_reward
**
** description: activates the water reward system
** 	give_reward gives a single reward and sets the count
** args:
**    dio   	... Digitial I/O address
**		duration ... duration of dio "on" time, in ms
*/
int dio_give_reward(long dio, long duration)
{

/*
**	printf("give reward (%ld %ld)\n", dio, duration);
*/

	dio_on(dio);
	timer_pause(duration);
	dio_off(dio);

	/* drop the code */ 
	ec_send_dio(dio);

	return(0);
}

/* PUBLIC ROUTINES: dio_clear/check/test_stim
**
** description: apply electrical microstimulation
**    with "prob" probability (between 0 and 1000)
*/
static int gl_stim_flag = 0;
int dio_clear_stim_flag(void)  { gl_stim_flag = 0; return(0); }
int dio_check_stim_flag(void)  { return(gl_stim_flag); }
int dio_conditional_stim(long dio1, long dio2, long prob, long pause)
{
   gl_stim_flag = 0;

   /* only stim if a random draw between 0 & 999 < prob */
   if(TOY_RAND(1000) < prob) {

    	/* put in stim mode  */
      dio_on(dio1);
     	/* stim trigger on   */
      dio_on(dio2);
      timer_pause(pause);
    	/* stim trigger off  */
      dio_off(dio2);
     	/* NOT back in rec mode 
		** dio_off(dio1);
		*/

      gl_stim_flag = 1;

		/* drop code only if stim  */
		ec_send_code(ELESTM);
      ec_send_dio(dio1);
   }

	/* otherwise don't drop any codes   */
   return(0);  
}

/* PUBLIC ROUTINE: dio_check_joybut
**
** check joystick button
**
** returns:	1 if joystick button is pressed
**			   0 if joystick button is not pressed or 
**					no joystick button defined
*/
int dio_check_joybut()
{
#ifdef JOYBUT
	return dio_in(JOYBUT);
#else
	return(0);
#endif
}

