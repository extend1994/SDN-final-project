#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "controller_agent.h"
#include "parser.h"
#include "common.h"
#include "status_monitor.h"

int main(void){
	int status = socket_initial();

	pthread_t agent, monitor;

	if(pthread_create(&agent, NULL, controller_agent, NULL)){
		log_error("Fail to create pthread\n");
	}else{
		log_debug("pthread_create agent\n");
	}
	
	if(pthread_create(&monitor, NULL, status_monitor, NULL)){
		log_error("Fail to create pthread\n");
	}else{
		log_debug("pthread_create agent\n");
	}

	pthread_join(agent, NULL);
	pthread_join(monitor, NULL);
	

	return 0;
}
