#ifndef PMM_H
#define PMM_H

#include "multiboot.h"

multiboot_info_t* mbinfo_addr;
uint64_t mem_bitmap[1024*16];	/* Each uint64_t can address 64 x 4K = 256K */
void get_mbmmap(void);			/* For a max of 4G physical memory, we need */
								/* 4G/256K = 1024 x 16 long long ints */
#endif