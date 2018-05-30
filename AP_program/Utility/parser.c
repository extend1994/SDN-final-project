#include "parser.h"

PARSER_CODE parser_hello_back(uint8_t *input){
	log_info("Enter %s\n", __FUNC__);
	AP_ID = input[0];
	if(AP_ID == 0){
		log_error("something wrong, back ID\n");
		log_error("Exit %s with fail\n", __FUNC__);
		return FAIL;
	}else{
		log_debug("AP_ID set as %d\n", (int)AP_ID);
		log_info("Exit %s\n", __FUNC__);
		return SUCCESS;
	}

}

PARSER_CODE encode_hello(uint8_t *message){
	log_info("Enter %s\n",__FUNC__);
	Header_t header;
	header.ID = 0;
	header.Type = HELLO;
	header.Length = 0;
	header.Bitmap = 0x00;
	memset(header.payload, 0, MAX_PAYLOAD);
	memset(message, 0, sizeof(uint8_t) * sizeof(Header_t));
	if(!encode_header(message, header)){
		log_error("error: encode_header\n");
		return FAIL;
	}
	log_info("Exit %s\n",__FUNC__);
	return SUCCESS;
}

PARSER_CODE parser_initial_set(Header_t *header, INIT_SET_parameter_t *parameter){
	memset(parameter, 0, sizeof(INIT_SET_parameter_t));
	int payload_count = 0;
	if(get_bit(header->Bitmap, 4)){
		log_debug("Get THSSTA\n");
		parameter->THSSTA = header->payload[payload_count++];
	}

	if(get_bit(header->Bitmap, 3)){
		log_debug("Get THSSNR\n");
		parameter->THSSNR = header->payload[payload_count++];
	}

	if(get_bit(header->Bitmap, 2)){
		log_debug("Get THSPKC\n");
		parameter->THSPKC = header->payload[payload_count++];
	}

	if(get_bit(header->Bitmap, 1)){
		log_debug("Get CHN\n");
		parameter->CHN =  header->payload[payload_count++];
	}

	if(get_bit(header->Bitmap, 0)){
		log_debug("Get PWR\n");
		parameter->PWR =  header->payload[payload_count++];
	}

	log_debug("THSSTA = %d\n", parameter->THSSTA);
	log_debug("THSSNR = %d\n", parameter->THSSNR);
	log_debug("THSPKC = %d\n", parameter->THSPKC);
	log_debug("CHN = %d\n", parameter->CHN);
	log_debug("PWR = %d\n", parameter->PWR);
		
}

PARSER_CODE encode_status_reply(Header_t *header, STATUS_REPLY_CONDITIONS_t status){
	log_info("Enter %s\n", __FUNC__);
	int payload_count = 0;
	if(get_bit(header->Bitmap, 3)){
		header->payload[payload_count++] = (status.SCHRES >> 8) & 0xff;
		header->payload[payload_count++] = status.SCHRES & 0xff;
	}

	if(get_bit(header->Bitmap, 2)){
		header->payload[payload_count++] = status.AVGSNR;
	}

	if(get_bit(header->Bitmap, 1)){
		header->payload[payload_count++] = status.PKTCNT;
	}

	if(get_bit(header->Bitmap, 0)){
		header->payload[payload_count++] = status.NUMSTA;
	}
	int i;

	for(i = 0; i < payload_count; i++){
		log_debug("No.%d: %d\n", i, header->payload[i]);
	}

	log_info("Exit %s\n", __FUNC__);
	return SUCCESS;
}

PARSER_CODE parser_set(Header_t *header, SET_parameter_t *parameter){
	memset(parameter, 0, sizeof(INIT_SET_parameter_t));
	int payload_count = 0;

	if(get_bit(header->Bitmap, 2)){
		log_debug("Get CHN\n");
		parameter->CHN = header->payload[payload_count++];
	}

	if(get_bit(header->Bitmap, 1)){
		log_debug("Get PWR\n");
		parameter->PWR =  header->payload[payload_count++];
	}

	if(get_bit(header->Bitmap, 0)){
		log_debug("Get BEACON\n");
		parameter->BEACON =  header->payload[payload_count++];
	}

	log_debug("CHN = %d\n", parameter->CHN);
	log_debug("PWR = %d\n", parameter->PWR);
	log_debug("BEACON = %d\n", parameter->BEACON);

}

PARSER_CODE parser_keep_alive(void){
	log_info("Enter %s\n", __FUNC__);
	
	log_info("Exit %s\n", __FUNC__);

	return SUCCESS;
}
PARSER_CODE encode_keep_alive(uint8_t *message){
	log_info("Enter %s\n",__FUNC__);
	Header_t header;
	header.ID = 0;
	header.Type = KEEP_ALIVE_BACK;
	header.Length = 0;
	header.Bitmap = 0x00;
	memset(header.payload, 0, MAX_PAYLOAD);
	memset(message, 0, sizeof(uint8_t) * PRE_HEADER_SIZE);
	if(!encode_header(message, header)){
		log_error("error: encode_header\n");
		return FAIL;
	}
	log_info("Exit %s\n",__FUNC__);
	return SUCCESS;
}

PARSER_CODE parser_header(uint8_t *input, Header_t *header){
	log_info("Enter %s\n", __FUNC__);

	memcpy(header, input, sizeof(Header_t) - MAX_PAYLOAD);//move input data to header
	memcpy(header->payload, input + sizeof(Header_t) - MAX_PAYLOAD, header->Length);
	log_debug("ID: %d\n", (int)header->ID);
	log_debug("Type: %d\n", (int)header->Type);
	log_debug("Length: %d\n", (int)header->Length);
	log_debug("Bitmap: 0x%x\n", (int)header->Bitmap);
 	int index;
	for(index = 0; index< header->Length; index++){
		log_debug("No. %d: 0x%x\n", index, header->payload[index]);
	}
	log_info("Exit %s\n", __FUNC__);

	return SUCCESS;
}

PARSER_CODE encode_header(uint8_t *output, Header_t header){
	log_info("Enter %s\n", __FUNC__);
	memset(output, 0, sizeof(Header_t) * sizeof(uint8_t));
	memcpy(output, &header, sizeof(Header_t));
	log_info("Exit %s\n", __FUNC__);
	return  SUCCESS;
}

