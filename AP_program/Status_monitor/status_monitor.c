#include "status_monitor.h"

void *status_monitor(void){
	int i = 0;
	uint32_t last_packet = 0;
	uint32_t PKTCNT_delta = 0;

	while(1){
		sleep(monitor_period);
		log_info("Let's monitor\n");
		
		Header_t header;
		memset(&header, 0, sizeof(Header_t));
		STATUS_REPLY_CONDITIONS_t status;
		memset(&status, 0, sizeof(STATUS_REPLY_CONDITIONS_t));
		
		firmware_status_req(&status);
		log_debug("system_config.number_of_STA_upper_bound = %d\n", system_config.number_of_STA_upper_bound);
		log_debug("system_config.BEACON = %d\n", system_config.BEACON);

		if(system_config.number_of_STA_upper_bound && system_config.BEACON){
			firmware_station_management();
		}

		if(system_config.initialized){
			//log_debug("system_config.THSSTA = %d\n", system_config.THSSTA);
			//log_debug("system_config.THSPKC = %d\n", system_config.THSPKC);
			//log_debug("system_config.THSSNR = %d\n", system_config.THSSNR);
			PKTCNT_delta = (uint32_t)((uint32_t)status.PKTCNT - last_packet);
			log_debug("last count = %d, now count = %d, delta = %d\n", last_packet, status.PKTCNT, PKTCNT_delta);
			
			if(PKTCNT_delta <= system_config.THSPKC){
				log_info("PKTCNT_delta = %d\n", PKTCNT_delta);
				status.PKTCNT = 0;
			}else{
				log_info("PKTCNT_delta = %d\n", PKTCNT_delta);
				if(status.PKTCNT > last_packet && !system_config.BEACON){
					status.PKTCNT = (uint8_t)PKTCNT_delta;
				}else{
					status.PKTCNT = 0;
				}
			}
			
			if(status.NUMSTA <= system_config.THSSTA){
				log_info("status.NUMSTA = %d\n", status.NUMSTA);
				status.NUMSTA = 0;
			}

			if(status.AVGSNR >= system_config.THSSNR){
				log_info("status.AVGSNR = %d\n", status.AVGSNR);
				status.AVGSNR = 0;
			}

			if(status.NUMSTA == 0 && status.PKTCNT == 0 && status.AVGSNR == 0){
				status.SCHRES = 0;
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
			last_packet = status.PKTCNT;
		}else{
			log_info("Haven't initialized by controller\n");
		}
	}
}
