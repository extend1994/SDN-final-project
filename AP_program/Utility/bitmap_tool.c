#include "bitmap_tool.h"
/*
 * get_bit
 * input:
 * 	input: the input byte you would like to get bit from
 * 	position: which bit you would like to get
 * return:
 * 	bit set: TRUE
 * 	bit unset: FALSE
 */
uint8_t get_bit(uint8_t input, int position){
	return (input >> position) & 1;
}

/* set_bit
 * input:
 * 	input: the input byte which you would like to set bit.
 * 	position: which bit you would like to set
 * return:
 * 	no return 
 */
void set_bit(uint8_t *input, int position){
	*input = *input | (1UL << position);
}

/* clear_bit
 * input:
 * 	input: the input byte which you would like to clear bit.
 * 	position: which bit you would like to clear
 * return:
 * 	no return 
 */
void clear_bit(uint8_t *input, int position){
	*input = *input & (1UL << position);
}

