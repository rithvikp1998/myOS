/* Paging */		

extern void load_page_directory(uint32_t* page_directory);
extern void enable_paging(void);
		
uint32_t page_directory[1024]__attribute__((aligned(4096)));		
		
/* This sets the following flags to the pages:		
   Supervisor: Only kernel-mode can access them		
   Write Enabled: It can be both read from and written to		
   Not Present: The page table is not present */		
		
void fill_page_directory(void){		
	for(uint16_t i=0; i<1024; i++)		
		page_directory[i] = 0x00000002;		
}		
		
uint32_t first_page_table[1024]__attribute__((aligned(4096)));		
		
/* attributes: supervisor level, read/write, present */		
		
void fill_first_page_table(void){		
	for(uint16_t i=0; i<1024; i++)		
		first_page_table[i] = (i * 0x1000) | 3; 		
	page_directory[0] = ((uint32_t)first_page_table) | 3;		
}