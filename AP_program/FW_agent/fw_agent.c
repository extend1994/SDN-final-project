#include "fw_agent.h"
/*
 * manage stations
 */
void firmware_station_management(void){
	log_info("Enter %s\n", __FUNC__);

	int counter, number_of_STA;
	char *ifname = "wlan1\0";
	const struct iwinfo_ops *iw;
	while(mtx){
		//busy waiting
		//log_debug("busy waiting\n");

	}
	mtx = 1;
	iw = iwinfo_backend(ifname);
	if (iw == NULL){
	        log_error("iw == NULL\n");
		goto error;
	}else{
	        log_debug("iw != NULL\n");
	}

	//Get STA list to fill avg. SNR, packet count, number of sta
	assocMember *member = get_assoclist(iw,ifname,&number_of_STA);
	log_debug("We have %d users\n", number_of_STA);	
	for(counter = system_config.number_of_STA_upper_bound; counter < number_of_STA; counter++){
		log_debug("New user mac: %s\n", member[counter].mac);
		Disassociation(member[counter].mac);
	}
    	free(member); 
	log_info("Exit %s\n", __FUNC__);
error:
	log_error("Exit %swith error\n", __FUNC__);
	mtx = 0;
	log_debug("release mtx\n");
}
/*
 * Set parameter to setup firmware
 */
void firmware_set(Header_t header, void *parameter){
	log_info("Enter %s\n", __FUNC__);
	char *ifname = "wlan1\0";
	SET_parameter_t *set_ptr;
	INIT_SET_parameter_t *init_set_ptr;
	
	while(mtx){
		//busy waiting
		//log_debug("busy waiting\n");
	}

	mtx = 1;
	switch(header.Type){
		case INIT_SET:
			init_set_ptr = (INIT_SET_parameter_t *)parameter;
			if(get_bit(header.Bitmap, 4)){
				system_config.THSSTA = init_set_ptr->THSSTA;
			}
			if(get_bit(header.Bitmap, 3)){
				system_config.THSSNR = init_set_ptr->THSSNR;
			}
			if(get_bit(header.Bitmap, 2)){
				system_config.THSPKC = init_set_ptr->THSPKC;
			}
			if(get_bit(header.Bitmap, 1)){
				FW_ARGU argument;
				argument.new_channel = init_set_ptr->CHN;
				FW_Control(ifname, 2, channel, argument);
			}
			if(get_bit(header.Bitmap, 0)){
				FW_ARGU argument;
				argument.new_txpower = init_set_ptr->PWR;
				FW_Control(ifname, 2, txpower, argument);
			}
			system_config.initialized = 1;	
			break;
		case SET:
			set_ptr = (SET_parameter_t *)parameter;
			if(get_bit(header.Bitmap, 0)){
				//FW_ARGU argument;
				//argument.hidden = set_ptr->BEACON;
				//FW_Control(ifname, 2, hidden, argument);
				int number_of_STA;
				char *ifname = "wlan1\0";
				const struct iwinfo_ops *iw;
				iw = iwinfo_backend(ifname);
				if (iw == NULL){
					log_error("iw == NULL\n");
				}else{
					log_debug("iw != NULL\n");
				}

				//Get STA list to fill avg. SNR, packet count, number of sta
				assocMember *member = get_assoclist(iw,ifname,&number_of_STA);
				log_debug("We have %d users\n", number_of_STA);	
				free(member);
				system_config.BEACON = set_ptr->BEACON;
				system_config.number_of_STA_upper_bound = number_of_STA;		
			}
		
			if(get_bit(header.Bitmap, 1)){
				FW_ARGU argument;
				argument.new_txpower = set_ptr->PWR;
				FW_Control(ifname, 2, txpower, argument);
			}

			if(get_bit(header.Bitmap, 2)){
				FW_ARGU argument;
				argument.new_channel = set_ptr->CHN;
				FW_Control(ifname, 2, channel, argument);
			}
			break;
		default:
			log_error("No good header type\n");
			break;
	}
	mtx = 0;
	log_debug("release mtx\n");
	log_info("Exit %s\n", __FUNC__);
}

/*
 * Request and get firmware status
 */ 
void firmware_status_req(STATUS_REPLY_CONDITIONS_t *status){
	log_info("Enter %s\n", __FUNC__);
	memset(status, 0, sizeof(STATUS_REPLY_CONDITIONS_t));

	int counter, number_of_AP, number_of_STA;
	char *ifname = "wlan1\0";
	const struct iwinfo_ops *iw;
	while(mtx){
		//busy waiting
		//log_debug("busy waiting\n");

	}
	mtx = 1;
	iw = iwinfo_backend(ifname);
	if (iw == NULL){
	        log_error("iw == NULL\n");
		goto error;
	}else{
	        log_debug("iw != NULL\n");
	}

	//Get scanlist to fill scanned channel results
	scanMember* scanRE = get_scanlist(iw, ifname, &number_of_AP);
	for(counter = 0; counter < number_of_AP; counter++){
		log_debug("channel = %d\n", scanRE[counter].channel);
		if(scanRE[counter].channel <= 8){
			set_bit(&status->SCHRES, scanRE[counter].channel-1);
		}else{
			uint8_t tmp = 0;
			set_bit(&tmp, scanRE[counter].channel - 9);
			status->SCHRES = status->SCHRES | ((uint16_t)(tmp << 8) & 0xff00);
		}
	}
	log_debug("SCHRES = 0x%X\n", status->SCHRES);
        free(scanRE);   
	
	//Get STA list to fill avg. SNR, packet count, number of sta
	int avg_snr = 0;
	int packets = 0;
	assocMember *member = get_assoclist(iw,ifname,&number_of_STA);
	for(counter = 0; counter < number_of_STA ;counter++){
		log_debug("SNR = %f\n", member[counter].snr);
		log_debug("packet count = %d\n", member[counter].num_of_packets);
		packets += (int)member[counter].num_of_packets;
		avg_snr += (int)member[counter].snr;
	}
	if(number_of_STA){
		avg_snr /= number_of_STA;
	}else{
		avg_snr = 0;
	}

	status->PKTCNT = (uint32_t)packets;
	status->AVGSNR = (uint8_t)avg_snr;
	status->NUMSTA = (uint8_t)number_of_STA;
    	free(member); 

	log_info("Exit %s\n", __FUNC__);

error:
	log_error("Exit %swith error\n", __FUNC__);
	
	mtx = 0;
	log_debug("release mtx\n");

}


