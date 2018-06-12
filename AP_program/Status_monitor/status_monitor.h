

#ifndef _STATUS_MONITOR_H
#define _STATUS_MONITOR_H

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
#include "controller_agent.h"
#include "fw_agent.h"

#define monitor_period 5
/*
 * Child thread 
 * For status monitor
 * In order to monitor the current status of this AP
 */
void *status_monitor(void);


#endif
