OBJS = boot.o gdt.o idt.o kernel.o keyboard.o mylibc.o vmm.o pmm.o tty.o
CFLAGS = -c -std=gnu99 -ffreestanding -Wall -Wextra 
CC = cross_compiler/bin/i686-elf-gcc
INCLUDES = 
LIBS = 

myos.bin: ${OBJS}
	${CC} -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib ${OBJS} -lgcc

boot.o: boot.asm
	nasm -felf32 boot.asm

%.o: %.c *.h
	${CC} ${CFLAGS} $< -o $@

clean:
	rm -f *.o *.bin *.iso
