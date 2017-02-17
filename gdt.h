/* Global Descriptor Table - We want to use paging in place of segmentation
   but we can't disable segmentation, so we disable its effect by creating 
   overlapped segment descriptors */

extern void gdt_flush(void);

/* Packed asks gcc not to add any padding for optimization */

struct gdt_entry{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
}__attribute__((packed));	


struct gdt_ptr_struct{
	uint16_t limit;
	uint32_t base;
}__attribute__((packed));

/* We put 3 entries in our GDT. 
   One for dummy to act as NULL segment,
   One for Code segment,
   One for Data segment */

struct gdt_entry gdt[3];
struct gdt_ptr_struct gdt_ptr;

void gdt_set_entry(uint8_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran){

    /* Setup the descriptor base address */
    gdt[index].base_low = (base & 0xFFFF);
    gdt[index].base_middle = (base >> 16) & 0xFF;
    gdt[index].base_high = (base >> 24) & 0xFF;

    /* Setup the descriptor limits */
    gdt[index].limit_low = (limit & 0xFFFF);
    gdt[index].granularity = ((limit >> 16) & 0x0F);

    /* Finally, set up the granularity and access flags */
    gdt[index].granularity |= (gran & 0xF0);
    gdt[index].access = access;
}

void load_gdt(void){

    /* Setup the GDT pointer and limit */
    gdt_ptr.limit = (sizeof(gdt) - 1);
    gdt_ptr.base = (uint32_t) gdt;

    /* NULL descriptor */
    gdt_set_entry(0, 0, 0, 0, 0);

    /* The second entry is Code Segment. The base address
       is 0, the limit is 4GB, it uses 4KB granularity,
       uses 32-bit opcodes, and is a Code Segment descriptor */
    gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    /* The third entry is Data Segment. It's EXACTLY the
       same as code segment, but the descriptor type in
       this entry's access byte says it's a Data Segment */
    gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    /* Flush out the old GDT and install the new changes */
    gdt_flush();
}