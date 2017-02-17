OBJS = boot.o kernel.o
CFLAGS = -c -std=gnu99 -ffreestanding -Wall -Wextra 
CC = cross_compiler/bin/i686-elf-gcc
INCLUDES = 
LIBS = 

myos.bin: ${OBJS}
	${CC} -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib ${OBJS} -lgcc

boot.o: boot.asm
	nasm -felf32 boot.asm

kernel.o: kernel.c *.h
	${CC} ${CFLAGS} kernel.c

clean:
	rm *.o *.bin