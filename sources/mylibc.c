#include <stdint.h>
#include <stddef.h>
#include "mylibc.h"
#include "tty.h"

/* size_t is unsigned and can hold any non-negative integer, i.e. it can 
   hold very small and very large values. When we use this, the compiler 
   is free to choose any unsigned int that is big enough to hold this value
   but not bigger than what's needed */

size_t strlen(const char* str){
	size_t len = 0;
	while(str[len])
		len++;
	return len;
}

/* World's most inefficient print functions for integers, 
   I need to replace this soon */

void printx(uint64_t data){
	if(data==0)
		terminal_putchar('0');
	else{
		char hex_array[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
							 'A', 'B', 'C', 'D', 'E', 'F'};
		char data_str[16];
		uint8_t pos=0;
		while(data>0){
			data_str[15-pos] = (hex_array[data%16]);
			data=data/16;
			pos++;
		}
		while(pos>0){
			terminal_putchar(data_str[16-pos]);
			pos--;
		}
	}
}

void printd(uint64_t data){
	if(data==0)
		terminal_putchar('0');
	else{
		char hex_array[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
		char data_str[20];
		uint8_t pos=0;
		while(data>0){
			data_str[19-pos] = (hex_array[data%10]);
			data=data/10;
			pos++;
		}
		while(pos>0){
			terminal_putchar(data_str[20-pos]);
			pos--;
		}
	}
}

void printb(uint64_t data){
	if(data==0)
		terminal_putchar('0');
	else{
		char hex_array[2] = {'0', '1'};
		char data_str[64];
		uint8_t pos=0;
		while(data>0){
			data_str[63-pos] = (hex_array[data%2]);
			data=data/2;
			pos++;
		}
		while(pos>0){
			terminal_putchar(data_str[64-pos]);
			pos--;
		}
	}
}