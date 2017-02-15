; Multiboot Header File

; Declare constants

MBALIGN 	equ 	1<<0					; shifts 1 by 0 bits to the left(==1)
MEMINFO 	equ 	1<<1					; shifts 1 by 1 bits to the left(==2)
FLAGS 		equ		MBALIGN | MEMINFO 		; bitwise or'd
MAGIC   	equ 	0x1BADB002				; 'Magic number' lets bootloader find this file, it is like a file identifier
CHECKSUM 	equ		-(MAGIC + FLAGS)		; checksum of above, to prove we are multiboot


; Declare a multiboot header that marks the program as a kernel. These are magic values that are documented in the multiboot standard. The bootloader will search for this signature in the first 8 KiB of the kernel file, aligned at a 32-bit boundary. The signature is in its own section so the header can be forced to be within the first 8 KiB of the kernel file.


section .multiboot

align 4										; enforces alignment of the data immediately on a memory address that is a multiple of 4
		dd MAGIC
		dd FLAGS
		dd CHECKSUM


section .bss

align 4
stack_bottom:
resb 16384									; declaring 16 KiB uninitialised storage space
stack_top:


; linker script specifies _start as the entry point to the kernel and the bootloader jumps to this position once the kernel has been loaded


section .text

global gdt_flush
global load_page_directory
global enable_paging
global keyboard_handler
global read_port
global write_port
global load_idt
global set_mbinfo_addr
global _start:function (_start.end - _start); declaring _start as a function symbol with the given symbol size

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

load_page_directory:
		push ebp
		mov ebp, esp
		mov eax, [ebp+8]
		mov cr3, eax
		pop ebp
		ret

enable_paging:
		push ebp
		mov ebp, esp
		mov eax, cr0
		or eax, 0x80000000
		mov cr0, eax
		mov esp, ebp
		pop ebp
		ret

keyboard_handler:
		extern keyboard_handler_main		; 'extern' specifies symbols that the current source file uses but which are defined in other object modules
		call keyboard_handler_main
		iretd

read_port:
		mov edx, [esp+4]
		in al, dx
		ret

write_port:
		mov edx, [esp+4]
		mov al, [esp+8]
		out dx, al
		ret

load_idt:
		mov edx, [esp+4]
		lidt [edx]
		sti
		ret

set_mbinfo_addr:
		extern mbinfo_addr
		mov [mbinfo_addr], ebx				; Multiboot bootloader stores address to multiboot_info struct in ebx
		ret

_start:										; bootloder loaded us into 32-bit protected mode
		mov esp, stack_top					; stack on x86 grows downwards
		extern kernel_main					
		call kernel_main
		cli 								; disables interrupts

.hang:	
		hlt									; wait for the next interrupt to arrive. Since they are disabled, this will lock up the computer
		jmp 	.hang						; Jump to hlt instruction if it ever wakes up

.end:

; compile using "nasm -felf32 boot.asm -o boot.o"