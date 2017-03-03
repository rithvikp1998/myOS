#ifndef PAGING_H
#define PAGING_H

extern void load_page_directory(uint32_t* page_directory);
extern void enable_paging(void);

uint32_t page_directory[1024]__attribute__((aligned(4096)));		
uint32_t first_page_table[1024]__attribute__((aligned(4096)));		

void fill_page_directory(void);
void fill_first_page_table(void);

#endif