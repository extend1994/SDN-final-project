
#ifndef __COMMON_H_
#define __COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint8_t AP_ID;

typedef struct{
	uint8_t THSSTA;//Threshold of station number
	uint8_t THSSNR;//Threshold of SNR
	uint8_t THSPKC;//Threshold of packet counts
}SYSTEM_CONFIG;

extern SYSTEM_CONFIG system_config;

#endif
