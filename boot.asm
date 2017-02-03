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
global _start:function (_start.end - _start); declaring _start as a function symbol with the given symbol size
_start:										; bootloder loaded us into 32-bit protected mode
		mov esp, stack_top					; stack on x86 grows downwards
		extern kernel_main					; 'extern' specifies symbols that the current source file uses but which are defined in other object modules
		call kernel_main


; Put the computer into an infinite loop


		cli 								; disables interrupts


.hang:	
		hlt									; wait for the next interrupt to arrive. Since they are disabled, this will lock up the computer
		jmp 	.hang						; Jump to hlt instruction if it ever wakes up

.end:										; compile using "nasm -felf32 boot.asm -o boot.o"