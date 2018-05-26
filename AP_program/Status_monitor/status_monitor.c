#include "status_monitor.h"

void *status_monitor(void){
	int i = 0;

	//This is just for test, not real usage.
	char message[10] = "012345678";

	while(1){
		i++;
		sleep(1);
		
		if(i==3){
			log_debug("HIHI\n");
			send(sockfd, message, sizeof(message), 0);
			i = 0;
		}
	}
}
