#include "controller_agent.h"

int sockfd = 0;

/*
 * child thread
 * */
void *controller_agent(void){
	log_info("Enter %s\n", __FUNC__);
	uint8_t message[MAX_PAYLOAD];
	while(1){
		recv(sockfd, message, MAX_PAYLOAD, 0);
		memset(message, 0, MAX_PAYLOAD * sizeof(uint8_t));
		log_debug("recv!!!\n");
	}

	log_info("Exit %s\n", __FUNC__);
}

/*
 *  initial everything related to socket
 *  return value: socket ID
 */
int socket_initial(void){
	log_debug("Enter %s\n", __FUNC__);

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		log_error("Fail to create a socket\n");
		log_error("Exit with error %s\n", __FUNC__);
		return -1;
	}

	struct sockaddr_in info;
	memset(&info, 0, sizeof(info));

	info.sin_family = PF_INET;
	info.sin_addr.s_addr = inet_addr(controller_ip);
	info.sin_port = htons(controller_port);

	if(connect(sockfd, (struct sockaddr_in *)&info, sizeof(info)) == -1){
		log_error("Connect error\n");
		log_error("Exit with error %s\n", __FUNC__);
		return -1;
	}

	log_info("Exit %s\n", __FUNC__);
	return sockfd;
	
}

