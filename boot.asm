; Multiboot Header File

; Declare constants

MBALIGN 	equ 	1<<0					
MEMINFO 	equ 	1<<1					
FLAGS 		equ		MBALIGN | MEMINFO 		
MAGIC   	equ 	0x1BADB002				; 'Magic number' lets bootloader find this file
CHECKSUM 	equ		-(MAGIC + FLAGS)		; checksum of above, to prove we are multiboot

KERNEL_VIRTUAL_BASE equ 0xC0000000                  ; 3GB
KERNEL_PAGE_NUMBER equ (KERNEL_VIRTUAL_BASE >> 22)  ; Page directory index of kernel's 4MB PTE.

; Declare a multiboot header that marks the program as a kernel. These are magic values that are documented in the multiboot standard. The bootloader will search for this signature in the first 8 KiB of the kernel file, aligned at a 32-bit boundary. The signature is in its own section so the header can be forced to be within the first 8 KiB of the kernel file.

section .data

	align 4096
	
	boot_page_dir:
			dd 0x00000083
			times (KERNEL_PAGE_NUMBER-1) dd 0
			dd 0x00000083
			times (1024-KERNEL_PAGE_NUMBER-1) dd 0

section .text

	align 4

	global gdt_flush
	global load_idt
	global read_port
	global write_port
	global keyboard_handler
	global set_mbinfo_addr
	global start

	extern kernel_main
	extern keyboard_handler_main		

	multiboot_header:
	    	dd MAGIC
	    	dd FLAGS
	    	dd CHECKSUM

	gdt_flush:
			cli
			extern gdt_ptr
			lgdt [gdt_ptr]
			jmp 0x08:complete_flush

	complete_flush:
			mov ax, 0x10
			mov ds, ax
	    	mov es, ax
	    	mov fs, ax
	    	mov gs, ax
	    	mov ss, ax
	    	ret

	load_idt:
			mov edx, [esp+4]
			lidt [edx]
			sti
			ret
	
	load_page_directory:
			mov ecx, (boot_page_dir - KERNEL_VIRTUAL_BASE)
	    	mov cr3, ecx
			ret

	allow_4m_pages:
			mov ecx, cr4
	    	or ecx, 0x00000010                          ; Set PSE bit in CR4 to enable 4MB pages.
	    	mov cr4, ecx
	    	ret

	enable_paging:
			mov ecx, cr0
	    	or ecx, 0x80000000                          ; Set PG bit in CR0 to enable paging.
	    	mov cr0, ecx
			ret

	unmap_identity_paging:
			mov dword [boot_page_dir], 0
	    	invlpg [0]
	    	ret

	read_port:
			mov edx, [esp+4]
			in al, dx
			ret

	write_port:
			mov edx, [esp+4]
			mov al, [esp+8]
			out dx, al
			ret

	keyboard_handler:
			call keyboard_handler_main
			iretd

	set_mbinfo_addr:
			extern mbinfo_addr
	    	add ebx, KERNEL_VIRTUAL_BASE
	    	mov [mbinfo_addr], ebx				; Multiboot bootloader stores address to multiboot_info struct in ebx
			ret

	;Until paging is set up, the code must be position-independent and use physical addresses, not virtual ones!

	start equ (_start - 0xC0000000)

	_start:									; bootloder loaded us into 32-bit protected mode
			call load_page_directory
			call allow_4m_pages
			call enable_paging
			lea ecx, [start_in_hh]			
			jmp ecx

; Start fetching instructions in kernel space. Since eip at this point holds the physical address of this command (approximately 0x00100000) we need to do a long jump to the correct virtual address of start_in_hh which is approximately 0xC0100000.

	start_in_hh:
			mov dword [boot_page_dir], 0
	    	invlpg [0]
			mov esp, stack+16384
			call set_mbinfo_addr		
			call kernel_main
			hlt 							;disables interrupts

section .bss

	align 32

	stack:
		resb 16384			; declaring 16 KiB uninitialised storage space

; compile using "nasm -felf32 boot.asm -o boot.o"