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

extern void keyboard_handler(void);
extern void write_port(uint16_t port, uint8_t data);
extern void load_idt(uint32_t *idt_ptr);
extern void terminal_initialize(void);
extern void terminal_write(char* data);
extern void keyboard_init(void);

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

void kernel_main(void){
	terminal_initialize();
	terminal_write("Hello, kernel world!\n\n\n");
	idt_init();
	keyboard_init();
	while(1);
}

/* Compile using "i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra" */