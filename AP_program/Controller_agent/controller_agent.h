

#ifndef _CONTROLL_AGENT_H
#define _CONTROLL_AGENT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "funclog.h"
#include "parser.h"

#define controller_ip "10.0.0.1"
#define controller_port 8899

/* 
 * Socket file descriptor
 */
extern int sockfd;

/*
 * To record the system is connected to controller or not
 */
extern int connected;
/*
 * connection thread
 */
void *initial_connection(void);

/*
 * child thread
 * */
void *controller_agent(void);

/*
 *  initiral everything related to socket
 *  return value: socket ID
 */
int socket_initial(void);

#endif
