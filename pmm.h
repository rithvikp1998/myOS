/* Physical Memory Manager */

#include "multiboot.h"

extern void set_mbinfo_addr(void);
 
multiboot_info_t* mbinfo_addr;

typedef struct mbmmap{
	uint32_t size;
	uint64_t base_addr_low;
	uint64_t base_addr_high;
	uint64_t length_low;
	uint64_t length_high;
	uint32_t type;
} mbmmap_t;

/*  Get the pointer to the first instance, grab whatever address and 
	length information you want, and finally skip to the next memory 
	map instance by adding size+sizeof(mmap->size) to the pointer */

void get_mbmmap(void){
	set_mbinfo_addr();
	mbmmap_t* mmap = (mbmmap_t*)(mbinfo_addr -> mmap_addr);
	while((uint32_t)mmap < mbinfo_addr->mmap_addr + mbinfo_addr->mmap_length)
		mmap = (mbmmap_t*)((uint32_t)mmap + mmap->size + sizeof(mmap->size));
}