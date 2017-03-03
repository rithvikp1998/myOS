#ifndef PMM_H
#define PMM_H

#include "multiboot.h"

multiboot_info_t* mbinfo_addr;

void get_mbmmap(void);

#endif