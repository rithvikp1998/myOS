/* Not all headers are available right now. But these and some other headers come along with the compiler. */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(__linux__)
#error "This is not linux, use a cross-compiler"
#endif

#if !defined(__i386__)
#error "Use a ix86-elf compiler"
#endif

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

void terminal_write(const char* data){
	size_t size = strlen(data);
	for(size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void kernel_main(void){
	terminal_initialize();
	terminal_write("Hello, kernel world!\n\n\nGoodbye, cruel kernel world!\n");
}

/* Compile using "i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra" */