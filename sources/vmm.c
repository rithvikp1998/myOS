#include <stdint.h>
#include "pmm.h"
#include "vmm.h"

uint32_t* pdir;
	
void vmm_alloc_page(uint32_t* e){
	uint32_t p = pmm_alloc_block();
	pte_set_frame(e, p);
	pte_add_flags(e, PTE_PRESENT);
}

void vmm_free_page(uint32_t* e){
	uint32_t p = pte_get_frame(e);
	pmm_free_block(p);
	pte_rem_flags(e, PTE_PRESENT);
}