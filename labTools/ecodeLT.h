/* ECODELT.H
**
** Revision history:
**	5/31/01 created by jig
*/

#ifndef _ECODELT_H_
#define _ECODELT_H_

/* PUBLIC ROUTINE PROTOTYPES */
int	ec_send_code					(long);
int 	ec_send_code_hi				(long);
int 	ec_send_code_lo				(long);
int	ec_send_code_tagged			(long, long);

void	ec_send_dio						(unsigned long);
void 	ec_send_dio_hi					(unsigned long);
void 	ec_send_dio_lo					(unsigned long);

void	ec_send_matlab_command		(int, int);
void 	ec_send_matlab_command_hi	(int, int);
void 	ec_send_matlab_command_lo	(int, int);

void 	ec_send_matlab_arg			(int, int);
void 	ec_send_matlab_arg_hi		(int, int);
void 	ec_send_matlab_arg_lo		(int, int);

void 	ec_send_hi						(unsigned short);
void 	ec_send_lo						(unsigned short, int);

#endif
