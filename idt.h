/* Interrupt Descriptor Table */

extern void load_idt(uint32_t *idt_ptr);

struct IDT_entry{
	uint16_t lower_offset;
	uint16_t selector;
	uint8_t  zero;
	uint8_t  type_attr;
	uint16_t higher_offset;
};

struct IDT_entry IDT[256];

void idt_init(void){
	uint32_t idt_address;
	uint32_t idt_ptr[2];

	/* Fill the IDT descriptor */
	idt_address = (uint32_t) IDT;
	idt_ptr[0] = (sizeof (struct IDT_entry) * 256) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16 ;

	load_idt(idt_ptr);
}