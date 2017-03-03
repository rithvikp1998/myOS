#ifndef GDT_H
#define GDT_H

#include <stdint.h>

/* Global Descriptor Table - We want to use paging in place of segmentation
   but we can't disable segmentation, so we disable its effect by creating 
   overlapped segment descriptors */

extern void gdt_flush(void);

/* Packed asks gcc not to add any padding for optimization */

struct gdt_entry{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
}__attribute__((packed));	


struct gdt_ptr_struct{
	uint16_t limit;
	uint32_t base;
}__attribute__((packed));

/* We put 3 entries in our GDT. 
   One for dummy to act as NULL segment,
   One for Code segment,
   One for Data segment */

struct gdt_entry gdt[3];
struct gdt_ptr_struct gdt_ptr;

void gdt_set_entry(uint8_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
void load_gdt(void);

#endif