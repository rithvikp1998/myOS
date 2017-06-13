#ifndef PMM_H
#define PMM_H

#include "multiboot.h"

multiboot_info_t* mbinfo_addr;
uint32_t mem_bitmap[1024*32];	/* Each uint32_t can address 32 x 4K = 128K */
								/* For a max of 4G physical memory, we need */
void get_mbmmap(void);			/* 4G/128K = 1024 x 32 long long ints */
uint32_t pmm_alloc_block(void);	
void pmm_free_block(uint32_t address);

#endif