#include <stdint.h>
#include "multiboot.h"
#include "pmm.h"

/*  Get the pointer to the first instance, grab whatever address and 
	length information you want, and finally skip to the next memory 
	map instance by adding size+sizeof(mmap->size) to the pointer */

void get_mbmmap(void){
	memory_map_t* mmap = (memory_map_t*)(mbinfo_addr -> mmap_addr + 0xC0000000);
	while((uint32_t)mmap < mbinfo_addr->mmap_addr + 0xC0000000 + mbinfo_addr->mmap_length)
		mmap = (memory_map_t*)((uint32_t)mmap + mmap->size + sizeof(mmap->size));
}