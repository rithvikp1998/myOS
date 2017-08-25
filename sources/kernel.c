/* Not all headers are available right now. But these and some other headers come along with the compiler. */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "gdt.h"
#include "idt.h"
#include "tty.h"
#include "keyboard.h"
#include "pmm.h"
#include "mylibc.h"

extern void scan_all_pci_buses();

#if defined(__linux__)
#error "This is not linux, use a cross-compiler"
#endif

#if !defined(__i386__)
#error "Use a ix86-elf compiler"
#endif

void kernel_main(void){
	terminal_initialize();							/* terminal_initialize function is put at top to */
	terminal_write("Hello, kernel world!\n\n\n");  	/* enable using print statements in other functions */
	get_mbmmap();
	load_gdt();
	idt_init();
	keyboard_init();
	scan_all_pci_buses();
	while(1);
}

/* Compile using "i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra" */