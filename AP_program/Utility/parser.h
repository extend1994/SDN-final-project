/*
 *  SDN-final-project
 *  parser.h
 *  The main purpose of this program is parsing messages
 *  All rights reserved
 */
#ifndef _PARSER_H
#define _PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "funclog.h"
#include "common.h"
#include "bitmap_tool.h"

#define MAX_PAYLOAD 16
#define PRE_HEADER_SIZE 4
#define MAX PRE_HEADER_SIZE + MAX_PAYLOAD + 5

typedef enum{
	SUCCESS=1,
	FAIL=0
}PARSER_CODE;

#define	HELLO 0x01
#define	HELLO_BACK 0x02
#define	INIT_SET 0x03
#define	STATUS_REPLY 0x04
#define	SET 0x05
#define	KEEP_ALIVE 0x06
#define	KEEP_ALIVE_BACK 0x07

#pragma pack(push)
#pragma pack(1)
typedef struct header{
	uint8_t ID;
	uint8_t Type;
	uint8_t Length;
	uint8_t Bitmap;
	uint8_t payload[MAX_PAYLOAD];
}Header_t;
#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)
typedef struct initial_set_parameter{
	uint8_t THSSTA;//Threshold of station number
	uint8_t THSSNR;//Threshold of SNR
	uint8_t THSPKC;//Threshold of packet counts
	uint8_t BEACON;//connection permission
	uint8_t CHN;//Channel
	uint8_t PWR;//Power
}INIT_SET_parameter_t;
#pragma pack(pop)

typedef struct initial_set_parameter SET_parameter_t;

#pragma pack(push)
#pragma pack(1)
typedef struct status_reply_conditions{
	uint16_t SCHRES;//scanned channel results
	uint8_t AVGSNR;//STA average SNR(to know who is interference)
	uint32_t PKTCNT;//packet counts
	uint8_t NUMSTA;//number of STA
}STATUS_REPLY_CONDITIONS_t;
#pragma pack(pop)

PARSER_CODE parser_hello_back(uint8_t *input);
PARSER_CODE encode_hello(uint8_t *message);

PARSER_CODE parser_initial_set(Header_t *header, INIT_SET_parameter_t *parameter);

PARSER_CODE encode_status_reply_bit_map(Header_t *header, STATUS_REPLY_CONDITIONS_t *status);
PARSER_CODE encode_status_reply(Header_t *heder, STATUS_REPLY_CONDITIONS_t status);

PARSER_CODE parser_set(Header_t *header, SET_parameter_t *set_parameter);

PARSER_CODE parser_keep_alive(void);
PARSER_CODE encode_keep_alive(uint8_t *message);

PARSER_CODE parser_header(uint8_t *input, Header_t *header);
PARSER_CODE encode_header(uint8_t *output, Header_t header);

#endif
