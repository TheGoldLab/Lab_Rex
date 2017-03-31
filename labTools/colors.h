/* COLORS.H
*
*	header file for COLORS.c, c-routines for
*		working with colors via dotsX
*/

#ifndef COLORS_H_
#define COLORS_H_

#include "property_list.h"

/* PUBLIC DEFINES */
#define CO_GET_R(co,ind)	(co)->properties[0]->values[(ind)]
#define CO_GET_G(co,ind)	(co)->properties[1]->values[(ind)]
#define CO_GET_B(co,ind)	(co)->properties[2]->values[(ind)]
#define CO_GET_L(co,ind)	(co)->properties[3]->values[(ind)]

#define CO_SET_R(co,ind,v)	CO_GET_R((co),(ind)) = (v)
#define CO_SET_G(co,ind,v)	CO_GET_G((co),(ind)) = (v)
#define CO_SET_B(co,ind,v)	CO_GET_B((co),(ind)) = (v)
#define CO_SET_L(co,ind,v)	CO_GET_L((co),(ind)) = (v)

/* PUBLIC ROUTINE PROTOTYPES */

_PLlist 	co_init		(void);
void		co_set		(_PLlist, int, int, int, int, int);
void	  	co_parse		(char *, _PLlist, int);

#endif /* COLORS_H_ */
