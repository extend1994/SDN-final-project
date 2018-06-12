
#ifndef __COMMON_H_
#define __COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX PRE_HEADER_SIZE + MAX_PAYLOAD + 5

uint8_t AP_ID;
uint8_t mtx;

typedef struct{
	uint8_t THSSTA;//Threshold of station number
	uint8_t THSSNR;//Threshold of SNR
	uint32_t THSPKC;//Threshold of packet counts
	uint8_t initialized;
	uint8_t BEACON;//BEACON: 1 --> kick new users
		       //BEACON: 0 --> do not kick new users
	uint8_t number_of_STA_upper_bound;//When packet count is above thresshold,
				       //this number will be set as the current number of
				       //stations.
}SYSTEM_CONFIG;

extern SYSTEM_CONFIG system_config;

#endif
