OBJS = boot.o gdt.o idt.o kernel.o keyboard.o mylibc.o vmm.o pmm.o pci.o tty.o
CFLAGS = -c -std=gnu99 -ffreestanding -Wall -Wextra 
CC = cross_compiler/bin/i686-elf-gcc
INCLUDES = 
LIBS = 
SOURCES_DIR = sources
HEADERS_DIR = headers
CFLAGS += -Iheaders

myos.bin: ${OBJS}
	${CC} -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib ${OBJS} -lgcc

boot.o: sources/boot.asm
	nasm -felf32 sources/boot.asm -o $@

%.o: ${SOURCES_DIR}/%.c ${HEADERS_DIR}/*.h
	${CC} ${CFLAGS} $< -o $@

clean:
	rm -f *.o *.bin *.iso
