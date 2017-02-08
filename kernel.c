/* Not all headers are available right now. But these and some other headers come along with the compiler. */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define KERNEL_OFFSET 0x08
#define INTERRUPT_GATE 0x8e

#if defined(__linux__)
#error "This is not linux, use a cross-compiler"
#endif

#if !defined(__i386__)
#error "Use a ix86-elf compiler"
#endif

extern void gdt_flush(void);
extern void load_page_directory(uint32_t* page_directory);
extern void enable_paging(void);
extern void keyboard_handler(void);
extern void write_port(uint16_t port, uint8_t data);
extern void load_idt(uint32_t *idt_ptr);
extern void terminal_initialize(void);
extern void terminal_write(char* data);
extern void keyboard_init(void);

/* Global Descriptor Table - We want to use paging in place of segmentation
   but we can't disable segmentation, so we disable its effect by creating 
   overlapped segment descriptors */

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

void gdt_set_entry(uint8_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran){
    
    /* Setup the descriptor base address */
    gdt[index].base_low = (base & 0xFFFF);
    gdt[index].base_middle = (base >> 16) & 0xFF;
    gdt[index].base_high = (base >> 24) & 0xFF;

    /* Setup the descriptor limits */
    gdt[index].limit_low = (limit & 0xFFFF);
    gdt[index].granularity = ((limit >> 16) & 0x0F);

    /* Finally, set up the granularity and access flags */
    gdt[index].granularity |= (gran & 0xF0);
    gdt[index].access = access;
}

void load_gdt(void){

    /* Setup the GDT pointer and limit */
    gdt_ptr.limit = (sizeof(gdt) - 1);
    gdt_ptr.base = (uint32_t) gdt;

    /* NULL descriptor */
    gdt_set_entry(0, 0, 0, 0, 0);

    /* The second entry is Code Segment. The base address
       is 0, the limit is 4GB, it uses 4KB granularity,
       uses 32-bit opcodes, and is a Code Segment descriptor */
	gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    /* The third entry is Data Segment. It's EXACTLY the
       same as code segment, but the descriptor type in
       this entry's access byte says it's a Data Segment */
    gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    /* Flush out the old GDT and install the new changes */
    gdt_flush();
}

/* Interrupt Descriptor Table */

struct IDT_entry{
	uint16_t lower_offset;
	uint16_t selector;
	uint8_t  zero;
	uint8_t  type_attr;
	uint16_t higher_offset;
};

struct IDT_entry IDT[256];

void idt_init(void){
	uint32_t keyboard_address;
	uint32_t idt_address;
	uint32_t idt_ptr[2];

	/* Fill keyboard interrupt's IDT entry */
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

	/* Fill the IDT descriptor */
	idt_address = (uint32_t) IDT;
	idt_ptr[0] = (sizeof (struct IDT_entry) * 256) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16 ;

	load_idt(idt_ptr);
}

/* Paging */

uint32_t page_directory[1024]__attribute__((aligned(4096)));

/* This sets the following flags to the pages:
   Supervisor: Only kernel-mode can access them
   Write Enabled: It can be both read from and written to
   Not Present: The page table is not present */

void fill_page_directory(void){
	for(uint16_t i=0; i<1024; i++)
		page_directory[i] = 0x00000002;
}

uint32_t first_page_table[1024]__attribute__((aligned(4096)));

/* attributes: supervisor level, read/write, present */

void fill_first_page_table(void){
	for(uint16_t i=0; i<1024; i++)
		first_page_table[i] = (i * 0x1000) | 3; 
	page_directory[0] = ((uint32_t)first_page_table) | 3;
}

void kernel_main(void){
	terminal_initialize();
	load_gdt();
	fill_page_directory();
	fill_first_page_table();
	load_page_directory(page_directory);
	enable_paging();
	idt_init();
	keyboard_init();
	terminal_write("Hello, kernel world!\n\n\n");
	while(1);
}

/* Compile using "i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra" */