#ifndef IDT_H
#define IDT_H

#include <stdint.h>

extern void load_idt(uint32_t *idt_ptr);

struct IDT_entry{
	uint16_t lower_offset;
	uint16_t selector;
	uint8_t  zero;
	uint8_t  type_attr;
	uint16_t higher_offset;
};

void idt_init(void);

extern struct IDT_entry IDT[256];

#endif