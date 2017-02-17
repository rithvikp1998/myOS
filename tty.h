#include "vga.h"
#include "mylibc.h"

extern uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg);
extern uint16_t vga_entry(unsigned char uc, uint8_t color);

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
	terminal_buffer = (uint16_t*) 0xC00B8000;
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