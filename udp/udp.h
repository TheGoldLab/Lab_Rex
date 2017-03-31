/* UDP.H
*
*	header file for UDP.c, which contains a few c-routines
*	to be called from REX to communicate to another
*	machine via udp/ip protocols
*
*	BSH 6 Jan 2006
*/

#ifndef UDP_H_
#define UDP_H_

/* PUBLIC ROUTINE PROTOTYPES */

void udp_open		(const char *, 
						const char *, int);	// Initialize UDP socket
void udp_send		(char *);				// Send a string
void udp_waitSend	(char *);				// Check handshake before send
int  udp_check		(int);					// Is incoming message available?
void udp_read		(char *, int);			// Read any incoming message
char *udp_get		(long);					// Get message (timeout argument)
void udp_close		(void);					// Cleanup UDP socket 

#endif /* UDP_H_ */
