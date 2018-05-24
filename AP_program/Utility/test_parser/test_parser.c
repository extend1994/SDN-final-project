#include <stdio.h>
#include <stdint.h>
#include "parser.h"
#include "common.h"

int main(int argv, char **argc){

	Header_t header, output_header;
	uint8_t *message = (uint8_t *)malloc(sizeof(uint8_t) * sizeof(Header_t));
	printf("------------------------------initial set------------------------------\n");	
	header.ID = 7;
	header.Type = 0x03;
	header.Length = 2;
	header.Bitmap = 0x09;
	header.payload[0] = 0x43;
	header.payload[1] = 0x11;
	encode_header(message, header);

	parser_header(message, &output_header);

	log_debug("ID: %d\n", (int)output_header.ID);
	log_debug("Type: %d\n", (int)output_header.Type);
	log_debug("Length: %d\n", (int)output_header.Length);
	log_debug("Bitmap: 0x%x\n", (int)output_header.Bitmap);
 	int index;
	for(index = 0; index< output_header.Length; index++){
		log_debug("No. %d: %d\n", index,output_header.payload[index]);
	}

	INIT_SET_parameter_t parameter;
	parser_initial_set(&output_header, &parameter);
	log_debug("THSSTA = %d\n", parameter.THSSTA);
	log_debug("THSSNR = %d\n", parameter.THSSNR);
	log_debug("THSPKC = %d\n", parameter.THSPKC);
	log_debug("CHN = %d\n", parameter.CHN);
	log_debug("PWR = %d\n", parameter.PWR);
	printf("------------------------------initial set-----------------------------\n\n");
	
	printf("*********************************set**********************************\n");
	header.ID = 7;
	header.Type = 0x05;
	header.Length = 3;
	header.Bitmap = 0x07;
	header.payload[0] = 0x0a;
	header.payload[1] = 20;
	header.payload[2] = 0;
	encode_header(message, header);	
	
	parser_header(message, &output_header);
	log_debug("ID: %d\n", (int)output_header.ID);
	log_debug("Type: %d\n", (int)output_header.Type);
	log_debug("Length: %d\n", (int)output_header.Length);
	log_debug("Bitmap: 0x%x\n", (int)output_header.Bitmap);
	for(index = 0; index< output_header.Length; index++){
		log_debug("No. %d: %d\n", index,output_header.payload[index]);
	}

	SET_parameter_t set_parameter;
	parser_set(&output_header, &set_parameter);
	log_debug("CHN = %d\n", set_parameter.CHN);
	log_debug("PWR = %d\n", set_parameter.PWR);
	log_debug("BEACON = %d\n", set_parameter.BEACON);
	printf("*********************************set**********************************\n");
	printf("============================status reply==============================\n");
	header.ID = 7;
	header.Type = 0x04;
	header.Length = 3;
	header.Bitmap = 0x09;

	STATUS_REPLY_CONDITIONS_t status;
	memset(&status, 0, sizeof(STATUS_REPLY_CONDITIONS_t));
	status.SCHRES = 0x3101;
	status.NUMSTA = 3;
	encode_status_reply(&header, status);
	encode_header(message, header);

	for(index = 0; index < header.Length; index++){
		log_debug("%x\n", header.payload[index]);
	}

	encode_header(message, header);

	for(index = 0; index < header.Length + 5; index++){
		log_debug("%x\n", message[index]);
	}
	parser_header(message, &output_header);
	log_debug("ID: %d\n", (int)output_header.ID);
	log_debug("Type: %d\n", (int)output_header.Type);
	log_debug("Length: %d\n", (int)output_header.Length);
	log_debug("Bitmap: 0x%x\n", (int)output_header.Bitmap);
	for(index = 0; index< output_header.Length; index++){
		log_debug("No. %d: %d\n", index,output_header.payload[index]);
	}
	printf("===========================status reply==============================\n");	
	return 0;
}
