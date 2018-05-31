/*
 * fw_agent
 * firmware agent
 * Used as the bridge between firmware and other functions
 * All rights reserved.
 */

#ifndef _FW_AGENT_H
#define _FW_AGENT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <memory.h>
#include <time.h>
#include <uci.h>
#include <pthread.h>
#include "iwinfo/lua.h"
#include <netdb.h>
#include <sys/socket.h> 
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "funclog.h"
#include "FW_control_mi.h"


/*
 * Set parameter to setup firmware
 */
void firmwre_set(Header_t header, void *parameter);

/*
 * Request and get firmware status
 */ 
void firmware_status_req(Header_t heder, STATUS_REPLY_CONDITIONS_t *status);

#endif
