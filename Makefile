OBJS = boot.o kernel.o keyboard.o tty.o
CFLAGS = -c -std=gnu99 -ffreestanding -Wall -Wextra 
CC = cross_compiler/bin/i686-elf-gcc
INCLUDES = 
LIBS = 

myos.bin: ${OBJS}
	${CC} -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib ${OBJS} -lgcc

boot.o: boot.asm
	nasm -felf32 boot.asm

kernel.o: kernel.c
	${CC} ${CFLAGS} kernel.c

keyboard.o: keyboard.c
	${CC} ${CFLAGS} keyboard.c

tty.o: tty.c vga.h mylibc.h
	${CC} ${CFLAGS} tty.c

clean:
	rm *.o *.bin