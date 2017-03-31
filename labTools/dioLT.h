/* DIOLT.H
**
**	header file for dio_mns, which contains
**	routines for sending Digital Input/Output
** signals via REX
**
** routines have prefix dio_
*/

#ifndef DIOLT_H_
#define DIOLT_H_

/* Public Macros */

/* Public Routine Prototypes */

/* DIO */

int	dio_give_reward			(long, long);
int	dio_clear_stim_flag		(void);
int	dio_check_stim_flag		(void);
int	dio_conditional_stim		(long, long, long, long);
int 	dio_check_joybut			(void);

#endif /* DIOLT_H_ */
