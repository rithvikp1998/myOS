#include <stdint.h>
#include "multiboot.h"
#include "pmm.h"
#include "mylibc.h"
#include "tty.h"

/*  Get the pointer to the first instance, grab whatever address and 
	length information you want, and finally skip to the next memory 
	map instance by adding size+sizeof(mmap->size) to the pointer */

/* mem_bitmap is an array of uint64_t serving as bitmap to manage physical memory

   0 - Free RAM
   1 - Used/Reserved RAM

   The bitmap is entirely set to 1s, then the free RAM blocks are marked with 0.
   This is not done the other way round because there are some memory regions
   that are not listed in GRUB memory map structure */

void get_mbmmap(void){
	/* Bitmap init */

	for(uint16_t i=0;i<16*1024;i++)
		mem_bitmap[i]=0xFFFFFFFFFFFFFFFF;

	/* Bitmap fill */

	memory_map_t* mmap = (memory_map_t*)(mbinfo_addr -> mmap_addr + 0xC0000000);
	for(uint32_t i=mmap->base_addr_low; i<mmap->base_addr_low+mmap->length_low; i+=4096)
		if(mmap->type==1 && i>640*1024)
			mem_bitmap[i/(4096*64)] &= 0 << ((i/4096) % 64);
	while((uint32_t)mmap < mbinfo_addr->mmap_addr + 0xC0000000 + mbinfo_addr->mmap_length){
		mmap = (memory_map_t*)((uint32_t)mmap + mmap->size + sizeof(mmap->size));
		for(uint32_t i=mmap->base_addr_low; i<mmap->base_addr_low+mmap->length_low; i+=4096)
			if(mmap->type==1 && i>640*1024)
				mem_bitmap[i/(4096*64)] &= 0 << ((i/4096) % 64);
	}
}