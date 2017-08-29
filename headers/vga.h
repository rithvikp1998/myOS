#ifndef VGA_H
#define VGA_H

#include <stdbool.h>

/* Hardware color constants */
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

enum vga_ports {
	ATTR_CONTROLLER_INDEX_PORT 		= 0x3C0,
	ATTR_CONTROLLER_WRITE_PORT 		= 0x3C0,
	ATTR_CONTROLLER_READ_PORT 		= 0x3C1,
	MISC_WRITE_PORT					= 0x3C2,
	SEQUENCER_INDEX_PORT 			= 0x3C4,
	SEQUENCER_DATA_PORT 			= 0x3C5,
	MISC_READ_PORT					= 0x3CC,
	GRAPHICS_CONTROLLER_INDEX_PORT 	= 0x3CE,
	GRAPHICS_CONTROLLER_DATA_PORT 	= 0x3CF,
	CRTC_INDEX_PORT 				= 0x3D4,
	CRTC_DATA_PORT 					= 0x3D5,
	ATTR_CONTROLLER_RESET_PORT 		= 0x3DA
};

extern uint8_t g_320x200x256[];

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg){
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color){
	return (uint16_t) uc | (uint16_t) color << 8;
}

static inline void asm_write_port(uint16_t port, uint32_t value)
{
	asm volatile("outl %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint32_t asm_read_port(uint16_t port)
{
    uint32_t result;
    asm volatile("inl %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

uint8_t* vga_get_frame_buffer();
void vga_fill_pixel(uint16_t x, uint16_t y, uint8_t color_index);
bool vga_set_mode(uint16_t width, uint16_t height, uint16_t color_depth);
void vga_fill_screen(uint16_t width, uint16_t height, uint16_t color_depth, 
					 uint8_t color_index);
#endif