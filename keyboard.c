#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

extern void write_port(uint16_t port, uint8_t data);
extern char read_port(uint16_t port);
extern void terminal_putchar(char c);
extern void terminal_backspace(void);

uint8_t keyboard_map[128] =
{
    0,  27, 
    '1', '2', '3', '4', '5', '6', '7', '8',	'9', '0', '-', '=', '\b', 
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*',
    0, ' ', 

};

void keyboard_init(void){
	/* 0xFD is 11111101 - enables only IRQ1 (keyboard)*/
	write_port(0x21 , 0xFD);
}

void keyboard_handler_main(void){
	uint8_t status;
	int8_t keycode;
	
	/* End of interrupt */
	write_port(0x20, 0x20);
	status = read_port(KEYBOARD_STATUS_PORT);
	
	/* Lowest bit of status will be set if buffer is not empty */
	if (status & 0x01) {
		keycode = read_port(KEYBOARD_DATA_PORT);
		if(keycode < 0)
			return;
		else if(keycode==14){
			terminal_backspace();
		}
		else{
			terminal_putchar(keyboard_map[(uint8_t) keycode]);
		}
	}
}