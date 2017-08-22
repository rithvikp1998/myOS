#include <stdint.h>
#include "keyboard.h"
#include "idt.h"

uint8_t keyboard_map[128] =
{
    0,  27, 
    '1', '2', '3', '4', '5', '6', '7', '8',	'9', '0', '-', '=', '\b', 
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*',
    0, ' ', 

};

uint32_t keyboard_address;

void fill_kb_idt_entry(void){
	keyboard_address = (uint32_t) keyboard_handler;
	IDT[0X21].lower_offset = keyboard_address & 0xffff;
	IDT[0x21].selector = KERNEL_OFFSET;
	IDT[0x21].zero = 0;
	IDT[0x21].type_attr = INTERRUPT_GATE;
	IDT[0x21].higher_offset = (keyboard_address & 0xffff0000) >> 16;

	/* In x86 protected mode, the first 32 interrupts are reserved for CPU
	   exceptions, so everything is offset by 0x20. For keyboard on RQ1, 
	   the value is 0x20 + 1 = 0x21 */

	/*		  Command 	    Data
		
		PIC1 	0x20		0x21
		PIC2	0xA0		0xA1

	*/

	/* ICW1 - Initialization */
	write_port(0x20, 0x11);
	write_port(0xA0, 0x11);
	/* ICW2 - Remap offset */
	write_port(0x21, 0x20);
	write_port(0xA1, 0x28);
	/* ICW3 - Setup cascading */
	write_port(0x21, 0x00);
	write_port(0xA1, 0x00);
	/* ICW4 - Environment Info */
	write_port(0x21, 0x01);
	write_port(0xA1, 0x01);
	/* Initialization Finished */

	/* Mask interrupts, 1 for disabling, 0 for enabling */
	write_port(0x21 , 0xff);
	write_port(0xA1 , 0xff);
}

void keyboard_init(void){
	fill_kb_idt_entry();
	
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
		else if(keycode==14)
			terminal_backspace();
		else
			terminal_putchar(keyboard_map[(uint8_t) keycode]);
	}
}