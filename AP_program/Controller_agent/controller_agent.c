#include "controller_agent.h"

int conntected = 0;
int sockfd = 0;

void init_hello(void){
	log_info("Enter %s\n", __FUNC__);
	size_t size = sizeof(Header_t);
	uint8_t *hello_message = (uint8_t *)malloc(sizeof(uint8_t) * size);
	encode_hello(hello_message);
	if(send(sockfd, hello_message, PRE_HEADER_SIZE, 0) != PRE_HEADER_SIZE){
		log_error("Fail to send\n");	
	}
	free(hello_message);
	log_info("Exit %s\n", __FUNC__);
}

void *initial_connection(void){
	while(!conntected){
		log_info("Haven't connect to controller\n");
		init_hello();		
		sleep(1);
	}
	log_info("Exit %s\n", __FUNC__);
}

/*
 * child thread
 * */ 
void *controller_agent(void){
	log_info("Enter %s\n", __FUNC__);
	uint8_t message[25] = {0};

	while(1){
		memset(message, 0, sizeof(25));
		if(recv(sockfd, message, MAX_PAYLOAD, 0)){
			log_info("RECV message!!!\n");
			Header_t output_header;
			parser_header(message, &output_header);
			log_debug("ID: %d\n", (int)output_header.ID);
			log_debug("Type: %d\n", (int)output_header.Type);
			log_debug("Length: %d\n", (int)output_header.Length);
			log_debug("Bitmap: 0x%x\n", (int)output_header.Bitmap);
			int index;
			for(index = 0; index< output_header.Length; index++){
				log_debug("No. %d: %d\n", index,output_header.payload[index]);
			}
			if(output_header.ID != AP_ID){
				log_info("Invalid AP ID %d\n", output_header.ID);
				log_error("Exit with error\n");
			}
			switch(output_header.Type){
				case HELLO_BACK:{
					log_info("Hello back message\n");
					if(parser_hello_back(message) == SUCCESS){
						log_info("good hello back message\n");
						conntected = 1; //connected.
					}
					break;
				}
				case INIT_SET:{
					log_info("Initial setup message\n");
					INIT_SET_parameter_t parameter;
					parser_initial_set(&output_header, &parameter);
					firmware_set(output_header, &parameter);
					break;
				}
				case SET:{
					log_info("Set up message\n");
					SET_parameter_t set_parameter;
					parser_set(&output_header, &set_parameter);
					firmware_set(output_header, &set_parameter);
					break;
				}
				case KEEP_ALIVE:{
					log_info("Keep alive message\n");
					//send keep alive back message to controller immediately
					uint8_t send_message[PRE_HEADER_SIZE] = {0};
					encode_keep_alive(send_message);
					if(send(sockfd, send_message, PRE_HEADER_SIZE, 0) != PRE_HEADER_SIZE){
						log_error("Fail to send\n");
						log_error("Exit with error\n");
					}
					break;
				}
				default:{
					log_info("No good type 0x%x\n", output_header.Type);
					break;
				}
			}	
			
				
		}else{
			log_info("Loss connection, bye ~~~~~\n");
			exit(1);
		}
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

	while(connect(sockfd, (struct sockaddr_in *)&info, sizeof(info)) == -1){
		log_error("Connect error\n");
		sleep(3);
	}

	log_info("Exit %s\n", __FUNC__);
	return sockfd;
	
}

