/*
 * This is the main function of AP program
 * This source file will include all the header files in this project
 * Please be careful for memory
 * All rightes reserved.
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "controller_agent.h"
#include "parser.h"
#include "common.h"
#include "status_monitor.h"
#include "funclog.h"


int main(void){
	log_info("Enter %s\n", __FUNC__);
	log_info("Well come to AP program\n");
	log_info("Enjoy the world of networking\n");
	log_info("This program is made by HsienWen Hu and AnnTasi\n");
	log_info("Have Fun !! SDN X wifi\n");


	if(!socket_initial()){
		log_error("socket_initial error\n");
		return -1;
	}
	
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
