#ifndef PMM_H
#define PMM_H

#include "multiboot.h"

multiboot_info_t* mbinfo_addr;
uint64_t mem_bitmap[1024*16];	/* Each uint64_t can address 64 x 4K = 256K */
								/* For a max of 4G physical memory, we need */
void get_mbmmap(void);			/* 4G/256K = 1024 x 16 long long ints */
uint32_t pmm_alloc_block(void);	
void pmm_free_block(uint32_t address);

#endif