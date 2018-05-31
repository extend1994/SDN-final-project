#include "status_monitor.h"

void *status_monitor(void){
	int i = 0;

	while(1){
		sleep(monitor_period);
		log_info("Let's monitor\n");
		
		Header_t header;
		memset(&header, 0, sizeof(Header_t));
		STATUS_REPLY_CONDITIONS_t status;
		memset(&status, 0, sizeof(STATUS_REPLY_CONDITIONS_t));

		firmware_status_req(&status);
		if(system_config.initialized){
			if(status.NUMSTA <= system_config.THSSTA){
				status.NUMSTA = 0;
			}
			if(status.PKTCNT <= system_config.THSPKC){
				status.PKTCNT = 0;
			}
			if(status.AVGSNR >= system_config.THSSNR){
				status.AVGSNR = 0;
			}

			encode_status_reply_bit_map(&header, &status);
			if(header.Bitmap){
				log_info("Send status reply\n");
				uint8_t message[PRE_HEADER_SIZE+MAX_PAYLOAD];
				encode_status_reply(&header, status);
				encode_header(message, header);
				if(send(sockfd, message, PRE_HEADER_SIZE+header.Length, 0) != PRE_HEADER_SIZE + header.Length){
					log_error("Fail to send\n");
				}
			}
		}else{
			log_info("Haven't initialized by controller\n");
		}	
	}
}
