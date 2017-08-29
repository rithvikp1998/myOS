#include <stdint.h>
#include <stdbool.h>

#include "vga.h"

uint8_t g_320x200x256[] = // Source: OSDev
{
/* Misc */
	0x63,
/* Seq */
	0x03, 0x01, 0x0F, 0x00, 0x0E,
/* CRTC */
	0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
	0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x9C, 0x0E, 0x8F, 0x28,	0x40, 0x96, 0xB9, 0xA3,
	0xFF,
/* Graphics Controller */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
	0xFF,
/* Attribute Controller */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x41, 0x00, 0x0F, 0x00,	0x00
};

bool vga_set_mode(uint16_t width, uint16_t height, uint16_t color_depth) 
{
	/* Only 320 x 200 x 256 mode is supported as of now */ 
	if(width != 320 || height != 200 || color_depth != 256)
		return false;

	uint8_t i = 0; // Counting variable
	uint8_t* values = g_320x200x256;

	/* Misc */
	for(i=0; i<4; i++)
	{
		asm_write_port(MISC_WRITE_PORT, i);
		asm_write_port(MISC_READ_PORT, *(values++));
	}

	/* Seq */
	for(i=0; i<5; i++)
	{
		asm_write_port(SEQUENCER_INDEX_PORT, i);
		asm_write_port(SEQUENCER_DATA_PORT, *(values++));
	}

	/* CRTC */
	asm_write_port(CRTC_INDEX_PORT, 0x03);
	asm_write_port(CRTC_DATA_PORT, asm_read_port(CRTC_DATA_PORT) | 0x80);
	asm_write_port(CRTC_INDEX_PORT, 0x11);
	asm_write_port(CRTC_DATA_PORT, asm_read_port(CRTC_DATA_PORT) & ~0x80);

	values[0x03] = values[0x03] | 0x80;
	values[0x11] = values[0x11] & ~0x80;

	for(i=0; i<25; i++)
	{
		asm_write_port(CRTC_INDEX_PORT, i);
		asm_write_port(CRTC_DATA_PORT, *(values++));
	}

	/* Graphics Controller */
	for(i=0; i<9; i++)
	{
		asm_write_port(GRAPHICS_CONTROLLER_INDEX_PORT, i);
		asm_write_port(GRAPHICS_CONTROLLER_DATA_PORT, *(values++));
	}

	/* Attribute Controller */
	for(i=0; i<21; i++)
	{
		asm_read_port(ATTR_CONTROLLER_RESET_PORT);
		asm_write_port(ATTR_CONTROLLER_INDEX_PORT, i);
		asm_write_port(ATTR_CONTROLLER_WRITE_PORT, *(values++));
	}	

	asm_read_port(ATTR_CONTROLLER_RESET_PORT);
	asm_write_port(ATTR_CONTROLLER_INDEX_PORT, 0x20);

	return true;
}

uint8_t* vga_get_frame_buffer()
{
    asm_write_port(GRAPHICS_CONTROLLER_INDEX_PORT, 0x06);
    uint8_t seg_num = asm_read_port(GRAPHICS_CONTROLLER_DATA_PORT) & (3<<2);
    switch(seg_num)
    {
        default:
        case 0<<2: return (uint8_t*)(0x00000 + 0xC0000000);
        case 1<<2: return (uint8_t*)(0xA0000 + 0xC0000000);
        case 2<<2: return (uint8_t*)(0xB0000 + 0xC0000000);
        case 3<<2: return (uint8_t*)(0xB8000 + 0xC0000000);
    }
}

void vga_fill_pixel(uint16_t x, uint16_t y, uint8_t color_index)
{
	uint8_t* pixel_address = vga_get_frame_buffer() + 320 * y + x;
	*pixel_address = color_index;
}

void vga_fill_screen(uint16_t width, uint16_t height, uint16_t color_depth, 
					 uint8_t color_index)
{
	bool ans = vga_set_mode(width, height, color_depth);
	if(!ans)
		return;
	for(uint16_t i = 0; i<width; i++)
		for(uint16_t j = 0; j<height; j++)
			vga_fill_pixel(i, j, color_index);
}