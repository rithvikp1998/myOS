#include <stdint.h>
#include "idt.h"

void idt_init(void){
	uint32_t idt_address;
	uint32_t idt_ptr[2];

	/* Fill the IDT descriptor */
	idt_address = (uint32_t) IDT;
	idt_ptr[0] = (sizeof (struct IDT_entry) * 256) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16 ;

	load_idt(idt_ptr);
}