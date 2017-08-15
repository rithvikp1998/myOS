#ifndef VMM_H
#define VMM_H

#define PDI(x) (((x) >> 22) & 0x3ff)	//Page Directory Index
#define PTI(x) (((x) >> 12) & 0x3ff)	//Page Table Index
#define PPA(x) (*x & ~0xfff)			//Page Physical Address

enum pte_flags{
	
	PTE_PRESENT		= 0x1,
	PTE_WRITABLE	= 0x2,
	PTE_USER		= 0x4,
	PTE_WT 			= 0x8,
	PTE_CD			= 0x10,
	PTE_ACCESSED 	= 0x20,
	PTE_DIRTY  		= 0x40,
	PTE_GLOBAL		= 0x100,
	PTE_PAGE_ADDR 	= 0x7FFFF000
};

enum pde_flags{

	PDE_PRESENT 	= 0x1,
	PDE_WRITABLE 	= 0x2,
	PDE_USER		= 0x4,
	PDE_WT 			= 0x8,
	PDE_CD 			= 0x10,
	PDE_ACCESSED 	= 0x20,
	PDE_4MB			= 0x80,
	PDE_PT_ADDR 	= 0x7FFFF000
};

extern uint32_t* pdir;

void vmm_alloc_page(uint32_t* e);
void vmm_free_page(uint32_t* e);

static inline void pde_add_flags(uint32_t* e, uint32_t flags) {
	*e |= flags;
}

static inline void pde_rem_flags(uint32_t* e, uint32_t flags) {
	*e &= ~flags;
}

static inline void pde_set_frame(uint32_t* e, uint32_t addr) {
	*e = (*e & PDE_PT_ADDR) | addr;
}

static inline uint32_t pde_get_frame (uint32_t* e) {
	return *e & PDE_PT_ADDR;
}

static inline void pte_add_flags(uint32_t* e, uint32_t flags) {
	*e |= flags;
}

static inline void pte_rem_flags(uint32_t* e, uint32_t flags) {
	*e &= ~flags;
}

static inline void pte_set_frame(uint32_t* e, uint32_t addr) {
	*e = (*e & PTE_PAGE_ADDR) | addr;
}

static inline uint32_t pte_get_frame (uint32_t* e) {
	return *e & PTE_PAGE_ADDR;
}

#endif