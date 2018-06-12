#include <stdio.h>
#include <netinet/in.h>

#include "FW_control_mi.h"

int main(int argc, char const *argv[])
{
    const struct iwinfo_ops *iw;
    char *ifname = "wlan1\0";
    char BSSID[BSSID_LEN];
    char SSID[IWINFO_ESSID_MAX_SIZE+1];
    int a, counter = 0;

    iw = iwinfo_backend(ifname);

    /*Setting Global BSSID*/
    if (iw == NULL){
        printf("iw == NULL");
        myBSSID = strdup("");
    }else{
        printf("iw != NULL\n");
        //snprintf(BSSID, sizeof(BSSID), "%s", get_bssid(iw,ifname));
        //get_bssid(iw, ifname, BSSID);
        //get_ssid(iw, ifname, SSID);
        //printf("BSSID = %s\n", BSSID);
        //printf("SSID = %s\n", SSID);
        //printf("Channel = %d\n", get_channel(iw, ifname));
        //printf("Tx power = %d\n", get_txpower(iw, ifname));
    }

    ap_status ap = get_ap_status(iw, ifname);
    printf("\n--------------------------------\n");
    printf("AP status : \n");
    printf("\tBSSID = %s\n", ap.bssid);
    printf("\tSSID = %s\n", ap.ssid);
    printf("\tChannel = %d\n", ap.channel);
    printf("\tTx power = %d\n", ap.txpower);
    printf("--------------------------------\n\n\n");

    assocMember *member = get_assoclist(iw,ifname,&a);
    printf("\n--------------------------------\n");
    printf("There are %d user\n",a);

    for(counter = 0; counter < a ; counter++){
        printf("User No.%d :\n",counter+1);
        printf("\tMAC = %s\n",member[counter].mac);
        printf("\tRSSI = %d\n",member[counter].rssi);
        printf("\tBSSID = %s\n",member[counter].bssid);
        printf("\tssid = %s\n",member[counter].ssid);
        printf("\tIP = %s\n",member[counter].ip );
    }

    printf("--------------------------------\n\n\n");
    free(member);

    scanMember* scanRE = get_scanlist(iw, ifname, &a);
    printf("\n--------------------------------\n");
    printf("Scanned Resault : \n");

    for(counter = 0; counter < a ; counter++){
        printf("User No.%d :\n",counter+1);
        printf("\tChannel : %d\n", scanRE[counter].channel);
        printf("\tRSSI = %d\n",scanRE[counter].rssi);
        printf("\tBSSID = %s\n",scanRE[counter].bssid);
    }
    printf("--------------------------------\n\n\n");
    free(scanRE);

    FW_ARGU argument;//the struct to store arguments which you want to change.
    argument.new_channel = 4;
    printf("FW control = %d\n", FW_Control(ifname, 2, channel, argument));
    
    argument.new_txpower = 20;//the struct to store arguments which you want to change.
    printf("FW control = %d\n", FW_Control(ifname, 2, txpower, argument));    

    argument.hidden = 0;
    printf("FW control = %d\n", FW_Control(ifname, 2, hidden, argument));

    ap = get_ap_status(iw, ifname);
    printf("\n--------------------------------\n");
    printf("AP status : \n");
    printf("\tBSSID = %s\n", ap.bssid);
    printf("\tSSID = %s\n", ap.ssid);
    printf("\tChannel = %d\n", ap.channel);
    printf("\tTx power = %d\n", ap.txpower);
    printf("--------------------------------\n\n\n");
    printf("End\n\n");
    return 0;
}
