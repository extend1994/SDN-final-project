/*
Copyright (c) 2016 National Central University, Studio215 ALL RIGHTS RESERVED

Firmware function and variable definitiion

ver 0.0.1

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

*/
#ifndef _Firmware_Control
#define _Firmware_Control

#include <sys/socket.h> 
#define WITH_THREADING
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <memory.h>
#include <time.h>
#include <uci.h>
#include <pthread.h>
#include "iwinfo/lua.h"
#include <netdb.h>
#include <sys/socket.h> 
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "funclog.h"


#define WLAN "wlan"
#define WIRELESS "wireless"

#define GHZ 1000 //1GHz = 1000MHz
#define TWO_DOT_FOUR_BAND 1
#define FIVE_BAND 2

#define BSSID_LEN 18 //The length of BSSID
#define ERR_BSSID "00:00:00:00:00:00" //When get_bssid occur error, function will return ERR_BSSID

#define SIGN_LEN 10
#define IP_LEN 16

#define twoP4_min_channel 1
#define twoP4_max_channel 11

#define number_of_five_band 19

#define min_tx_power 0
#define max_tx_power 21

#define MAX_INTERFACE_NAME_LEN 30
#define MAX_HWMODE 12

#define IF_PROTO_LEN 10
#define COUNTRY_ID 4
#define WORKING_MODE_LEN 10
#define SSID_LEN 32
#define MAC_LEN 24
#define L4_PROTO_LEN 5
#define L4_PROTO_LEN 5
#define SITE_LEN 50

#define APif_LEN 10


#ifndef __FUNC__
	#define __FUNC__ (char *)__FUNCTION__// get function name
#endif

typedef struct scanMember
{
	char *bssid;
	char *ssid;
	int channel;
	int rssi;
} scanMember;

typedef struct assocMember
{
	char *bssid;
	char *ssid;
	char *mac;
	//char *rssi;
	int rssi;
	int noise;
	float snr;
	int num_of_packets;
	char *ip;
} assocMember;

/*uci_ap_info*/
typedef struct ap_status
{
	char *bssid;
	char *ssid;
	int channel;
	int txpower;
	char *ipaddr;
} ap_status;

typedef struct _fw_argu_{
	int new_channel;
	int new_txpower;
	char *disassociaiton_mac;
	int hidden;
}FW_ARGU;

typedef enum{
	channel = 1,
	txpower = 2,
	disassociation = 3,
	hidden = 4,
	ALL = 5
}FW_COMMAND;

typedef enum{
	FW_SEC = 0,
	FW_FAIL_invalid_channel = -1,
	FW_FAIL_to_change_channel = -2,
	FW_FAIL_invalid_txpower = -3,
	FW_FAIL_to_change_txpower = -4,
	FW_FAIL_null_iw = -5,
	FW_FAIL_fail_update_ip_table = -6,
	FW_FAIL_to_query = -7,
	FW_FAIL_to_check_ssid_para = -8,
	FW_FAIL = -100 // this is for no reason fail
}FW_CODE;

typedef enum{
	INVALID = -1,
	VALID = 0
}FW_CHECK_RESULT;

char *myBSSID;

/*
 * Checker
 */
FW_CHECK_RESULT check_channel(const struct iwinfo_ops *iw, const char *ifname, int channel);
FW_CHECK_RESULT check_txpower(const struct iwinfo_ops *iw, const char *ifname, int txpower);

/*
 * FW control function
 */
FW_CODE FW_Control(char *ifname, int radio_id, FW_COMMAND command, FW_ARGU argument);
FW_CODE FW_Control_wireless_restart();
FW_CODE FW_Control_network_restart();
FW_CODE FW_Control_system_restart();

/*
 * iwinfo
 */
void get_bssid(const struct iwinfo_ops *iw, const char *ifname, char*bssid);
void get_ssid(const struct iwinfo_ops *iw, const char *ifname, char *ssid);
int get_band(const struct iwinfo_ops *iw, const char *ifname);
int get_channel(const struct iwinfo_ops *iw, const char *ifname);
int get_txpower(const struct iwinfo_ops *iw, const char *ifname);
void get_hwmodes(const struct iwinfo_ops *iw, const char *ifname, char *hwmode);
int get_htmode(const struct iwinfo_ops *iw, const char *ifname);
char * format_channel(int ch);
void format_bssid(unsigned char *mac, unsigned char *input);
void format_bssid_without_semicolon(char *input, char *output);
char * format_ssid(char *ssid);
void format_signal(int sig, char *output);
void format_hwmodes(int modes, char *buf);

void lookup_phy(const struct iwinfo_ops *iw, const char *section);

scanMember* get_scanlist(const struct iwinfo_ops *iw, const char *ifname, int* length);

assocMember* get_assoclist(const struct iwinfo_ops *iw, const char *ifname, int* length);

/*uci*/
void uci_set_power_channel(int Power, int channel, int radio_id);
void Disassociation(char *mac);
/* hidden wifi
 * input:
 * 	1 = hidden wifi
 * 	0 = broadcast wifi
 */
void hidden_wifi(int status);
ap_status get_ap_status(const struct iwinfo_ops *iw, const char *ifname);

#endif
