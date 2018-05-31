/*
Copyright (c) 2016 National Central University, Studio215 ALL RIGHTS RESERVED

Firmware function and variable definitiion

ver 0.0.2

Author: STEVE SHUNG ; Hsien-Wen, Hu ; Buck Chung

Date: 2016/9/15

Attribute:
Define all the functions which can be used on Access Point.

FW_Control is the main function of control AP, arguments and output return is defiened below:

input :
        ifname : interface name such as "wlan0".
        command : what kind of setting you would like to contorl. 
                  NOTICE !! use FW_COMMAND type.
        argument : A structure can store arguments. The data type of argument is defined in this header file.
output :
        FW_SEC = 0,
        FW_FAIL_invalid_channel = -1,
        FW_FAIL_to_change_channel = -2,
        FW_FAIL_invalid_txpower = -3,
        FW_FAIL_to_change_txpower = -4,
        FW_FAIL_null_iw = -5,
        FW_FAIL_fail_update_ip_table = 6,
        FW_FAIL_Invaild_Parameter = -7,
        FW_FAIL_Exeuation_ERROR = -8,

        FW_FAIL = -100 // this is for no reason fail 
CAUTION : 
    ver 0.0.1 is based on OPENwrt TP-Link WDR-3600
    ver 0.0.2 is based on Xiao Mi mini

*/
#include "FW_control_mi.h"

char *myBSSID;

FW_CODE FW_Control(char *ifname, int radio_id, FW_COMMAND command, FW_ARGU argument){
	log_debug("Enter [%s]", __FUNC__);

	const struct iwinfo_ops *iw;
	iw = iwinfo_backend(ifname);
	if (iw == NULL){
    		log_debug("Fail ! iw == NULL");
	        myBSSID = strdup("");
        	log_debug("Exit [%s]", __FUNC__);
        	return FW_FAIL_null_iw;
    	}

    switch(command){
    	case channel:
            log_debug("FW_Control command : channel");
    		if(argument.new_channel == get_channel(iw, ifname)){//same as now
    			log_debug("Channel change to %d", argument.new_channel);
    			log_debug("Exit [%s]", __FUNC__);
    			return FW_SEC;
    		}else if(check_channel(iw, ifname, argument.new_channel) == INVALID){
    			log_debug("New channel = %d, Invalid channel !!", argument.new_channel);
    			log_debug("Exit [%s]", __FUNC__);
    			return FW_FAIL_invalid_channel;
    		}
    		else{
    			uci_set_power_channel(get_txpower(iw,ifname), argument.new_channel, radio_id);
    		}

    		if(argument.new_channel == get_channel(iw, ifname)){
    			log_debug("Channel change successfully");
    			log_debug("Exit [%s]", __FUNC__);
    			return FW_SEC;
    		}else{
    			log_debug("Fail to change channel");
    			log_debug("Exit [%s]", __FUNC__);
    			return FW_FAIL_to_change_channel;
    		}
    		break;
    	case txpower:
            log_debug("FW_Control command : tx power");
    		if(argument.new_txpower == get_txpower(iw, ifname)){//same as now
    			log_debug("Tx Power change to %d", argument.new_txpower);
    			log_debug("Exit [%s]", __FUNC__);
    			return FW_SEC;
    		}else if(check_txpower(iw, ifname, argument.new_txpower) == INVALID){
    			log_debug("New Tx power = %d, Invalid Tx power !!", argument.new_txpower);
    			log_debug("Exit [%s]", __FUNC__);
    			return FW_FAIL_invalid_txpower;
    		}
    		else{
    			uci_set_power_channel(argument.new_txpower, get_channel(iw, ifname), radio_id);
    		}

    		if(argument.new_txpower == get_txpower(iw, ifname)){
    			log_debug("Tx Power change successfully !");
    			log_debug("Exit [%s]", __FUNC__);
    			return FW_SEC;
    		}else{
    			log_debug("Fail to change Tx Power");
    			log_debug("Exit [%s]", __FUNC__);
    			return FW_FAIL_to_change_txpower;
    		}
    		break;	
	case hidden:
		log_info("FW_Control command : hidden\n");
		hidden_wifi(argument.hidden);
		log_info("Exit %s\n", __FUNC__);
		return FW_SEC;
		break;
    	case disassociation:
            	log_debug("FW_Control command : disassociation");
    		Disassociation(argument.disassociaiton_mac);
    		log_debug("Exit [%s]", __FUNC__);
    		return FW_SEC;
    		break;
    	case ALL:
            log_debug("FW_Control command : all !");
            log_debug("argument.new_channel = %d", argument.new_channel);
            log_debug("argument.new_txpower = %d", argument.new_txpower);
    		/*
    		 * new channel
    		 */
    		if(argument.new_channel == get_channel(iw, ifname)){//same as now
    			log_debug("Channel change to %d", argument.new_channel);
    			log_debug("Exit [%s]", __FUNC__);
    			return FW_SEC;
    		}else if(check_channel(iw, ifname, argument.new_channel) == INVALID){
    			log_debug("New channel = %d, Invalid channel !!", argument.new_channel);
    			log_debug("Exit [%s]", __FUNC__);
    			return FW_FAIL_invalid_channel;
    		}else{
    			uci_set_power_channel(get_txpower(iw,ifname), argument.new_channel, radio_id);
    		}

    		if(argument.new_channel == get_channel(iw, ifname)){
    			log_debug("Channel change successfully");
    			log_debug("Exit [%s]", __FUNC__);
    			return FW_SEC;
    		}else{
    			log_debug("Fail to change channel");
    			log_debug("Exit [%s]", __FUNC__);
    			return FW_FAIL_to_change_channel;
    		}//end of new channel


    		/*
    		 * new tx power
    		 */
    		if(argument.new_txpower == get_txpower(iw, ifname)){//same as now
    			log_debug("Tx Power change to %d", argument.new_txpower);
    			log_debug("Exit [%s]", __FUNC__);
    			return FW_SEC;
    		}else if(check_txpower(iw, ifname, argument.new_txpower) == INVALID){
    			log_debug("New Tx power = %d, Invalid Tx power !!", argument.new_txpower);
    			log_debug("Exit [%s]", __FUNC__);
    			return FW_FAIL_invalid_txpower;
    		}
    		else{
    			uci_set_power_channel(argument.new_txpower, get_channel(iw, ifname), radio_id);
    		}

    		if(argument.new_txpower == get_txpower(iw, ifname)){
    			log_debug("Tx Power change successfully !");
    			log_debug("Exit [%s]", __FUNC__);
    			return FW_SEC;
    		}else{
    			log_debug("Fail to change Tx Power");
    			log_debug("Exit [%s]", __FUNC__);
    			return FW_FAIL_to_change_txpower;
    		}//end of new tx power
    		break;
    	default:
    		log_debug("Invalid command");
    		break;
    }
    log_debug("Exit [%s]", __FUNC__);
    return FW_FAIL;
}


FW_CHECK_RESULT check_channel(const struct iwinfo_ops *iw, const char *ifname, int channel){
    log_info("Enter [%s]", __FUNC__);
    int band = get_band(iw, ifname);
    int valid_five_GHZ_channel[number_of_five_band] = {36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 136, 140, 144, 149, 153, 157, 161};

    if(band == TWO_DOT_FOUR_BAND){//2.4 GHz band
        if(channel >= twoP4_min_channel && channel <= twoP4_max_channel){
            log_debug("Valid");
            log_info("Exit [%s]", __FUNC__);
            return VALID;
        }else{
            log_debug("Invalid");
            log_info("Exit [%s]", __FUNC__);
            return INVALID;
        }
    }else if(band == FIVE_BAND){//5 GHz band
        int counter;
        for(counter = 0; counter < number_of_five_band; counter++){
            log_debug("valid_five_GHZ_channel[%d] = %d", counter, valid_five_GHZ_channel[counter]);
            if(channel == valid_five_GHZ_channel[counter]){
                log_debug("Valid channel = %d", channel);
                log_info("Exit [%s]", __FUNC__);
                return VALID;
            }
        }

        log_debug("Invalid");
        log_info("Exit [%s]", __FUNC__);
        return INVALID;
    }else{
        log_debug("FW control doesn't understand what are you talking about");
        return INVALID;
    }
}

FW_CHECK_RESULT check_txpower(const struct iwinfo_ops *iw, const char *ifname, int txpower){
    log_info("Enter [%s]", __FUNC__);
    if(txpower > min_tx_power && txpower < max_tx_power){
        log_debug("VALID");
        log_info("Exit [%s]", __FUNC__);
        return VALID;
    }else{
        log_debug("INVALID");
        log_info("Exit [%s]", __FUNC__);
        return INVALID;
    }
}

// ----------- FW_Control_restart --------
FW_CODE FW_Control_wireless_restart(){
	if(system("wifi"))
		return FW_SEC;
	else
		return FW_FAIL;
}

FW_CODE FW_Control_network_restart(){
	if(system("/etc/init.d/network restart") && system("/etc/init.d/firewall restart"))
		return FW_SEC;
	else
		return FW_FAIL;
}

FW_CODE FW_Control_system_restart(){
	if(system("reboot"))
		return FW_SEC;
	else
		return FW_FAIL;
}

/*iwinfo get basic ap information function*/
int get_txpower(const struct iwinfo_ops *iw, const char *ifname)
{
	log_debug("Enter %s",__FUNC__);
	iw = iwinfo_backend(ifname);
	int pwr;

    if(iw == NULL){
        log_debug("Fail ! iw == NULL\n");
        return -1;
    }
	
    if (iw->txpower(ifname, &pwr))
		pwr = -1;

	log_debug("Tx power = %d", pwr);
	log_debug("Exit %s\n",__FUNC__);
	return pwr;
}

int get_band(const struct iwinfo_ops *iw, const char *ifname){
	log_debug("Enter %s",__FUNC__);
	int freq;
	iw = iwinfo_backend(ifname);
	
	if (iw == NULL){
        log_debug("Fail ! iw == NULL\n");
        return -1;
    }

	if (iw->frequency(ifname, &freq)){
	 	freq = -1;
	}

	log_debug("frequency = %d", freq);
	freq = freq / GHZ;
	if(freq == 2){//2.4GHz
		log_info("2.4GHz band");
		log_info("Exit [%s]", __FUNC__);
		return TWO_DOT_FOUR_BAND;
	}else if(freq == 5){//5GHz
		log_info("5GHz band");
		log_info("Exit [%s]", __FUNC__);
		return FIVE_BAND;
	}else{
		log_debug("Not valid band ");
		log_info("Exit [%s]", __FUNC__);
		return -1;
	}

}

int get_channel(const struct iwinfo_ops *iw, const char *ifname)
{
	log_debug("Enter %s",__FUNC__);
	iw = iwinfo_backend(ifname);
	int ch;
    if(iw == NULL){
        log_debug("Fail ! iw == NULL\n");
        return -1;
    }

	if (iw->channel(ifname, &ch))
		ch = -1;

	log_debug("channel = %d", ch);
	log_debug("Exit %s\n",__FUNC__);
	return ch;
}

void get_bssid(const struct iwinfo_ops *iw, const char *ifname, char *bssid)
{
	log_debug("Enter %s",__FUNC__);
	log_debug("ifname = %s", ifname);
	iw = iwinfo_backend(ifname);

    if(iw == NULL){
        log_debug("Fail ! iw == NULL\n");
        return -1;
    }

	//char buf[BSSID_LEN];
	bzero(bssid, BSSID_LEN);
	log_debug("Get BSSID");
	
	if (iw->bssid(ifname, bssid)){
		snprintf(bssid, BSSID_LEN, "%s" ,ERR_BSSID);
	}

	log_debug("Exit %s\n",__FUNC__);
	//return buf;
}

void get_ssid(const struct iwinfo_ops *iw, const char *ifname, char *ssid)
{
	log_debug("Enter %s",__FUNC__);
    char buf[IWINFO_ESSID_MAX_SIZE+1] = { 0 };

	iw = iwinfo_backend(ifname);

    if(iw == NULL){
        log_debug("Fail ! iw == NULL\n");
        return -1;
    }

	if (iw->ssid(ifname, ssid)){
		memset(ssid, 0, IWINFO_ESSID_MAX_SIZE+1);
	}
	
	log_debug("Exit %s\n",__FUNC__);
}

void get_hwmodes(const struct iwinfo_ops *iw, const char *ifname, char *hwmode)
{
	log_debug("Enter %s\n",__FUNC__);

    int modes;

    if(iw == NULL){
        log_debug("Fail ! iw == NULL\n");
        return -1;
    }

    if (iw->hwmodelist(ifname, &modes)){
    		log_debug("Fail ! ifname = %s", ifname);
    		log_debug("Exit %s",__FUNC__);
             //modes = -1;
    }else{
    		log_debug("Success! ifname = %s", ifname);
    		format_hwmodes(modes, hwmode);
	        log_debug("Exit %s",__FUNC__);
	}
}

int get_htmode(const struct iwinfo_ops *iw, const char *ifname){
	log_debug("Enter [%s]\n",__FUNC__);
	int i, htmodes;
    
    if(iw == NULL){
        log_debug("Fail ! iw == NULL\n");
        return -1;
    }

	if (iw->htmodelist(ifname, &htmodes))
	{
		log_debug("No HT mode information available");
		log_info("Exit [%s]", __FUNC__);
		return;
	}

	log_info("Exit [%s]", __FUNC__);
	return htmodes;
}


ap_status get_ap_status(const struct iwinfo_ops *iw, const char *ifname)
{
	log_debug("Enter [%s]\n", __FUNC__);

	char ip[IP_LEN];
	ap_status aps;

	/*get ip*/
	//get_IP(ip);

	/*if (ip == NULL){
		aps.ipaddr = strdup("");
	}
	else{
		aps.ipaddr = (char *)malloc(sizeof(char) * IP_LEN);
		snprintf(aps.ipaddr, IP_LEN, "%s", ip);
	}*/

	/*iwinfo get*/
	iw = iwinfo_backend(ifname);
	if (iw == NULL)
	{
		aps.bssid = strdup("");     
		aps.ssid = strdup("");
		aps.channel = 0;
		aps.txpower = 0;		
	}
	else
	{
		char bssid[BSSID_LEN];
		char ssid[IWINFO_ESSID_MAX_SIZE+1];
		get_bssid(iw, ifname, bssid);
		get_ssid(iw,ifname,ssid);

		aps.bssid = (char *)malloc(sizeof(char) * BSSID_LEN);
		aps.ssid = (char *)malloc(sizeof(char) * (IWINFO_ESSID_MAX_SIZE+1));

		snprintf(aps.bssid, BSSID_LEN, "%s",bssid);
		snprintf(aps.ssid, IWINFO_ESSID_MAX_SIZE+1, "%s", ssid);

		aps.channel = get_channel(iw, ifname);
		aps.txpower = get_txpower(iw, ifname);
	}

	log_debug("Exit [%s]\n", __FUNC__);
	return aps;
}



void uci_set_power_channel(int Power, int channel, int radio_id)
{
	log_debug("Enter %s",__FUNC__);
    char uci_set_power[100] = {0};
    char uci_set_channel[100] = {0};
    char* uci_commit = "uci commit wireless";
    char* wifiReboot = "wifi";

    snprintf(uci_set_power, sizeof(uci_set_power),"uci set wireless.radio%d.txpower=%d", radio_id - 1, Power);
    snprintf(uci_set_channel, sizeof(uci_set_channel),"uci set wireless.radio%d.channel=%d",radio_id -1, channel);

    
    
    log_debug("power command = %s, execute !", uci_set_power);
	system(uci_set_power);
    log_debug("channel command = %s, execute", uci_set_channel);
	system(uci_set_channel);
    log_debug("uci_commit command = %s, execute", uci_commit);
	system(uci_commit);
    log_debug("reboot command = %s, execute", wifiReboot);
	system(wifiReboot);
    log_debug("sleep ~~~");
	sleep(5);
	log_debug("Exit %s",__FUNC__);
}

/* hidden wifi
 * input:
 * 	1 = hidden wifi
 * 	0 = broadcast wifi
 */
void hidden_wifi(int status){
	log_info("Enter %s\n", __FUNC__);
	char* uci_commit = "uci commit wireless";
	char* wifiReboot = "wifi";
	char command[100] = {0};
	snprintf(command, sizeof(command), "uci set wireless.@wifi-iface[1].hidden=%d", status);
	system(command);
	system(uci_commit);
	system(wifiReboot);
	sleep(5);
	log_info("Exit %s\n", __FUNC__);
}

void Disassociation(char *mac){
	log_debug("Enter [%s]", __FUNC__);

	char command[100];
	bzero(command, sizeof(command));

	snprintf(command, sizeof(command), "hostapd_cli disassociate %s", mac);
	system(command);

	log_debug("Exit [%s]", __FUNC__);
}

assocMember* get_assoclist(const struct iwinfo_ops *iw, const char *ifname, int *length)
{
	log_debug("Enter [%s]", __FUNC__);
	int i, len, index;
	char buf[IWINFO_BUFSIZE];
    memset(buf, 0, IWINFO_BUFSIZE); //clear buf to 0
	struct iwinfo_assoclist_entry *e;
	//iw = iwinfo_backend(ifname);
	assocMember* assMem;

	if (iw == NULL)
	{
		log_debug("iw == NULL");
		assMem = (assocMember*)malloc(sizeof(assocMember));
        	memset(assMem, 0, sizeof(assocMember));//clear assMem to 0
		//assMem->bssid = strdup(myBSSID);
		//assMem->ssid = strdup("");
		//assMem->mac = strdup("");
		//assMem->rssi = strdup("");
		assMem->rssi = 0;

		*length = 0; 
		return assMem;
	}
	else
	{
		if (iw->assoclist(ifname, buf, &len))
		{	
			log_debug("No information available");
			assMem = (assocMember*)malloc(sizeof(assocMember));
            		memset(assMem, 0, sizeof(assocMember));//clear assMem to 0
			//assMem->bssid = strdup(myBSSID);
			//assMem->ssid = strdup("");
			//assMem->mac = strdup("");
			//assMem->rssi = strdup("");
			assMem->rssi = 0;

			*length = 0;
			log_debug("Exit [%s]\n", __FUNC__); 
			return assMem;
		}
		else if (len <= 0)
		{
			char ssid[IWINFO_ESSID_MAX_SIZE+1];
			get_ssid(iw,ifname, ssid);
			log_debug("No station connected in get assoclist len:%d",len);
			assMem = (assocMember*)malloc(sizeof(assocMember));
            		memset(assMem, 0, sizeof(assocMember));//clear assMem to 0
			//assMem[index].bssid = (char *)malloc(sizeof(char) *  BSSID_LEN);
			//get_bssid(iw, ifname, assMem[index].bssid);
			//assMem->bssid = myBSSID;

			//assMem->ssid = (char *)malloc(sizeof(char) * (IWINFO_ESSID_MAX_SIZE+1));
			//snprintf(assMem->ssid, IWINFO_ESSID_MAX_SIZE+1, "%s",ssid);
			
			//assMem->mac = strdup("");
			//assMem->rssi = strdup("");
			//assMem->rssi = 0;

			*length = 0;
			log_debug("Exit [%s]\n", __FUNC__); 
			return assMem;
		}
		else
		{
			log_debug("There are some users");
			*length = (len / sizeof(struct iwinfo_assoclist_entry)); 
			assMem = (assocMember*)malloc(sizeof(assocMember)*(*length));
            		memset(assMem, 0, sizeof(assocMember)*(*length));//clear assMem to 0
			/*get every assoclistInfo*/
			for (i = 0, index=0; i < len, index < *length; i += sizeof(struct iwinfo_assoclist_entry),index++)
			{
				e = (struct iwinfo_assoclist_entry *)&buf[i];

				char ssid[IWINFO_ESSID_MAX_SIZE+1] = {0};
				get_ssid(iw, ifname, ssid);

				char bssid[BSSID_LEN+1] = {0};
				format_bssid(bssid, e->mac);

				//char rssi[SIGN_LEN];
				//format_signal(e->signal, rssi);
				assMem[index].bssid = (char *)malloc(sizeof(char) *  (BSSID_LEN+1));
                		memset(assMem[index].bssid, 0, sizeof(char) *  (BSSID_LEN+1));
				get_bssid(iw, ifname, assMem[index].bssid);
				//assMem[index].bssid = myBSSID;

				assMem[index].ssid = (char *)malloc(sizeof(char) * (IWINFO_ESSID_MAX_SIZE+1));
		                memset(assMem[index].ssid, 0, sizeof(char) * (IWINFO_ESSID_MAX_SIZE+1));
				snprintf(assMem[index].ssid, IWINFO_ESSID_MAX_SIZE+1, "%s", ssid);

				assMem[index].mac = (char *)malloc(sizeof(char) * (BSSID_LEN+1));
                		memset(assMem[index].mac, 0, sizeof(char) * (BSSID_LEN+1));
				snprintf(assMem[index].mac, BSSID_LEN, "%s", bssid);

				assMem[index].ip = (char *)malloc(sizeof(char) * (IP_LEN+1));
				bzero(assMem[index].ip, IP_LEN+1);
				snprintf(assMem[index].ip, IP_LEN+1, "To be continue...");
				//get_station_ip(assMem[index].mac, assMem[index].ip);
				//assMem[index].rssi = (char *)malloc(sizeof(char) * (SIGN_LEN+1));
				//snprintf(assMem[index].rssi, SIGN_LEN, "%s", rssi);
				assMem[index].rssi = e->signal;
				assMem[index].noise = e->noise;
				assMem[index].snr = (float)e->signal / e->noise;
				assMem[index].num_of_packets = e->rx_packets + e->tx_packets;
			}
			log_debug("Exit [%s]\n", __FUNC__);
			return assMem;
		}
	}
	
}

scanMember* get_scanlist(const struct iwinfo_ops *iw, const char *ifname, int *length)
{
	log_debug("Enter %s\n",__FUNC__);

	int i, x, len;
	iw = iwinfo_backend(ifname);
	char buf[IWINFO_BUFSIZE] = {0};
	struct iwinfo_scanlist_entry *e;
	
	if (iw == NULL)
	{
		*length = 0;
		return NULL;
	}
	else
	{
		if (iw->scanlist(ifname, buf, &len))
		{
			log_debug("Scanning not possible\n\n");
			log_debug("Exit %s\n",__FUNC__);
			return NULL;
		}
		else if (len <= 0)
		{
			log_debug("No scan results\n\n");
			log_debug("Exit %s\n",__FUNC__);
			return NULL;
		}
		else
		{
			*length = len / sizeof(struct iwinfo_scanlist_entry);
			scanMember *sM = (scanMember*)malloc(sizeof(scanMember)*(len / sizeof(struct iwinfo_scanlist_entry)));
            memset(sM, 0, sizeof(scanMember)*(len / sizeof(struct iwinfo_scanlist_entry)));

			for (i = 0, x = 1; i < len; i += sizeof(struct iwinfo_scanlist_entry), x++)
			{
				e = (struct iwinfo_scanlist_entry *) &buf[i];
				char mac[BSSID_LEN]={0};
				format_bssid(mac, e->mac);

				sM[x-1].bssid = (char *)malloc(sizeof(char) * BSSID_LEN);
                memset(sM[x-1].bssid, 0, sizeof(char) * BSSID_LEN);
				snprintf(sM[x-1].bssid, BSSID_LEN, "%s", mac);

				sM[x-1].ssid = (char *)malloc(sizeof(char) * (IWINFO_ESSID_MAX_SIZE+1));
                memset(sM[x-1].ssid, 0, sizeof(char) * (IWINFO_ESSID_MAX_SIZE+1));
				snprintf(sM[x-1].ssid, IWINFO_ESSID_MAX_SIZE+1, "%s", e->ssid);

				sM[x-1].channel = e->channel;
				sM[x-1].rssi = (e->signal - 0x100);	
			}

			log_debug("Exit %s\n",__FUNC__);
			return sM;
		}
	}
}
/*iwinfo format*/
void format_hwmodes(int modes, char *buf)
{
	log_debug("Enter %s",__FUNC__);
	
	bzero(buf, MAX_HWMODE+1);

	if (modes <= 0){
	        snprintf(buf, MAX_HWMODE, "unknown");
	        log_debug("buf = %s", buf);
	}else{
	        snprintf(buf, MAX_HWMODE, "802.11%s%s%s%s%s",
	                 (modes & IWINFO_80211_A) ? "a" : "",
	                 (modes & IWINFO_80211_B) ? "b" : "",
	                 (modes & IWINFO_80211_G) ? "g" : "",
	                 (modes & IWINFO_80211_N) ? "n" : "",
	                 (modes & IWINFO_80211_AC) ? "ac" : "");
	        log_debug("buf = %s", buf);
	}

	log_debug("Exit %s",__FUNC__);
}

char * format_channel(int ch)
{
	log_debug("Enter %s\n",__FUNC__);
	char buf[8];

	if (ch <= 0)
		snprintf(buf, sizeof(buf), "unknown");
	else
		snprintf(buf, sizeof(buf), "%d", ch);


	log_debug("Exit %s\n",__FUNC__);
	return buf;
}

void format_bssid(unsigned char *mac, unsigned char *input)
{
	log_debug("Enter %s",__FUNC__);

	snprintf(mac, BSSID_LEN, "%X:%X:%X:%X:%X:%X",
		input[0], input[1], input[2], input[3], input[4], input[5]);

	log_debug("Exit %s\n",__FUNC__);
}

void format_bssid_without_semicolon(char *input, char *output){
    log_debug("Enter %s",__FUNC__);
    int i = 0; //just a counter.
    int j = 0; //just another counter

    for(i = 0; i < BSSID_LEN; i++){
        if(input[i] != ':'){
            output[j] = input[i];
            j++;
        }else{
            //do nothing.
        }
    }
    //log_debug("output = %s", output);
    log_debug("Exit %s\n",__FUNC__);
}

char * format_ssid(char *ssid)
{
	log_debug("Enter %s\n",__FUNC__);

	char buf[IWINFO_ESSID_MAX_SIZE+3];

	if (ssid && ssid[0])
		snprintf(buf, sizeof(buf), "%s", ssid);
	else
		snprintf(buf, sizeof(buf), "unknown");


	log_debug("Exit %s\n",__FUNC__);
	return buf;
}

void format_signal(int sig, char *output)
{
	log_debug("Enter %s\n",__FUNC__);

	if (!sig){
		snprintf(output, SIGN_LEN, "unknown");
	}else{
		snprintf(output, SIGN_LEN, "%d", sig);
	}
	log_debug("Exit %s\n",__FUNC__);
}
