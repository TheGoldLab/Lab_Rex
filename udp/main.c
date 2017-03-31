/* main.c
 *
 * Main function for stand-alone executable that can use UDP to send
 *  messages between Rex and the Mac
 * 
 * NOTE: IP ADDRESSES ARE HARD-WIRED IN THIS FILE
 *      see below for details
 *
 */
#include <stdio.h>
#include "udp.h"

int main(int argc, char **argv)
{
    char buf[256], *msg;
    
    /* help message */
    if(argc == 1) {
        printf("udp: usage\n");
        printf("\t-s <msg>: send message\n");
        printf("\t-h <msg>: send message with handshake\n");
        printf("\t-d: send done message\n");
        printf("\t-g: get message (check once)\n");
        printf("\t-w <optional timeout in ms>: wait for message\n");
        exit(0);
    }
    
    /* Open the connection
     * 
     * THE IP ADDRESSES ARE HARD-WIRED HERE:
     *
     *  First is IP address of this machine (Rex)
     *  Second is IP address of the connected machine (Mac)
     *  Third is the port number = 6665
     *
     * printf("about to open connection\n");
     */
    udp_open("192.168.16.250", "192.168.16.8", 6665);
    
    /* receive message */
    if(!strcmp(argv[1], "-s")) {
        
        if(argc < 3)
            printf("ERROR: provide a message to send\n");
        
        printf("Sending message <%s>\n", argv[2]);
        udp_send(argv[2]);
        
    } else if(!strcmp(argv[1], "-h")) {
        
        if(argc < 3)
            printf("ERROR: provide a message to send\n");
        
        printf("Sending message <%s>\n", argv[2]);
        msg = udp_get(100);
        udp_send(argv[2]);
        
    } else if(!strcmp(argv[1], "-d")) {
        
        udp_send("continue_flag=0");
        
    } else if(!strcmp(argv[1], "-g")) {
        
        if(!(msg = udp_get(0)))
            printf("No message\n");
        else
            printf("Message is <%s>\n", msg);
        
    } else if(!strcmp(argv[1], "-w")) {
        long timeout = -1;
        if(argc == 3)
            timeout = atoi(argv[2]);
        
        if(!(msg = udp_get(timeout)))
            printf("No message\n");
        else
            printf("Message is <%s>\n", msg);
        
    } else if(argc == 3 && !strcmp(argv[1], "-g")) {
        
        if((msg = udp_get(atoi(argv[2])*1000L)) != NULL)
            printf("Got message: <%s>\n", msg);
        else
            printf("no message\n");
        
    } else if(argc == 2 && !strcmp(argv[1], "-t")) {
        
        sprintf(buf, "rRemoteSetup(1,2);");
        udp_get(-1);   /* check once for possible remaining handshake  */
        udp_send(buf); /* send the command */
        if((msg = udp_get(2000)) != NULL)
            printf("return message is <%s>\n", msg);
        
    } else {
        printf("bad argument\n");
    }
    
    udp_close();
    return(0);
}
