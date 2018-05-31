#include "fw_agent.h"

/*
 * Set parameter to setup firmware
 */
void firmwre_set(Header_t header, void *parameter){
	log_info("Enter %s\n", __FUNC__);
	char *ifname = "wlan1\0";
	SET_paramter_t *set_ptr;
	INITIAL_SET_parameter_t *init_set_ptr;

	switch(header.Type){
		case INIT_SET:{
			init_set_ptr = (SET_paramter_t *)parameter;
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
			break;
		}
		case SET:{
			set_ptr = (INITIAL_SET_parameter_t *)parameter;
			if(get_bit(header.Bitmap, 2)){
				FW_ARGU argument;
				argument.new_channel = set_ptr->CHN;
				FW_Control(ifname, 2, channel, argument);
			}
			if(get_bit(header.Bitmap, 1)){
				FW_ARGU argument;
				argument.new_txpower = set_ptr->PWR;
				FW_Control(ifname, 2, txpower, argument);
			}
			if(get_bit(header.Bitmap, 0)){
				FW_ARGU argument;
				argument.hidden = set_ptr->BEACON;
				FW_Control(ifname, 2, hidden, argument);
			}
			break;
		}
		default:{
			log_error("No good header type\n");
			break;
		}
	}
	log_info("Exit %s\n", __FUNC__);
}

/*
 * Request and get firmware status
 */ 
void firmware_status_req(Header_t header, STATUS_REPLY_CONDITIONS_t *status){
	log_info("Enter %s\n", __FUNC__);
	memset(status, 0, sizeof(STATUS_REPLY_CONDITIONS_t));

	int counter, number_of_AP, number_of_STA
	char *ifname = "wlan1\n";
	const struct iwinfo_ops iw = iwinfo_backend(ifname);
	if (iw == NULL){
	        log_error("iw == NULL\n");
	}else{
	        log_debug("iw != NULL\n");
	}

	//Get scanlist to fill scanned channel results
	scanMember* scanRE = get_scanlist(iw, ifname, &number_of_AP);
	for(counter = 0; counter < number_of_AP; counter++){
		log_debug("channel = %d\n", scanRE[counter].channel);
		set_bit(&status->SCHRES, scanRE[counter].channel);
	}
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
	avg_snr /= number_of_STA;

	status->PKTCNT = (uint32_t)packets;
	status->AVGSNR = (uint8_t)avg_snr;
	status->NUMSTA = (uint8_t)number_of_STA;
    	free(member); 



	log_info("Exit %s\n", __FUNC__);
}


