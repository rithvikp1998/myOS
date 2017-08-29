OBJS = boot.o gdt.o idt.o kernel.o keyboard.o mylibc.o vmm.o pmm.o pci.o tty.o vga.o
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

run-qemu: myos.bin
	qemu-system-i386 -kernel myos.bin -drive id=disk,file=hdd.img,if=none -device ahci,id=ahci -device ide-drive,drive=disk,bus=ahci.0

myos.iso: myos.bin
	cp myos.bin isodir/boot/
	grub-mkrescue -o $@ isodir

run-vbox: myos.iso
	VBoxManage startvm myOS

