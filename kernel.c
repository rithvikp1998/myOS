/* Not all headers are available right now. But these and some other headers come along with the compiler. */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "kb_map.h"

#define KERNEL_OFFSET 0x08
#define INTERRUPT_GATE 0x8e
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

#if defined(__linux__)
#error "This is not linux, use a cross-compiler"
#endif

#if !defined(__i386__)
#error "Use a ix86-elf compiler"
#endif

extern void keyboard_handler(void);
extern char read_port(uint16_t port);
extern void write_port(uint16_t port, uint8_t data);
extern void load_idt(uint32_t *idt_ptr);


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

/* Hardware color constants */

/* A variable that has been declared as having an enumerated type can be 
   assigned any of the enumerators as a value */

enum vga_color{
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

/* inline function suggests compiler to use the whole function definition 
   everywhere the function is called to save function call overhead time */

/* uint8_t stands for unsigned 8-bit integer(typedef for unsigned char), 
   uint16_t for unsigned 16-bit integer */

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg){
	return fg | bg << 4;	//bitwise or'd and shifted left by 4 bits
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color){
	return (uint16_t) uc | (uint16_t) color << 8;	//bitwise or'd and shifted left by 4 bits
}

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

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

void terminal_initialize(void){
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

void terminal_putchar(char c){
	const size_t index = terminal_row * VGA_WIDTH + terminal_column;
	if(c=='\n'){
		terminal_row++;
		terminal_column = 0;
	}
	else{
		terminal_buffer[index] = vga_entry(c, terminal_color);
		if(++terminal_column == VGA_WIDTH){
			terminal_column = 0;
			if(++terminal_row == VGA_HEIGHT)
				terminal_row = 0;
		}
	}
}

void terminal_backspace(void){
	if(terminal_column==0){
		terminal_row--;
		terminal_column=VGA_WIDTH;
	}
	else{
		terminal_column--;
	}
	size_t index = terminal_row * VGA_WIDTH + terminal_column;
	terminal_buffer[index] = vga_entry(' ', terminal_color);
}

void terminal_write(const char* data){
	size_t size = strlen(data);
	for(size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

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

void kernel_main(void){
	terminal_initialize();
	terminal_write("Hello, kernel world!\n\n\n");
	idt_init();
	keyboard_init();
	while(1);
}

/* Compile using "i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra" */