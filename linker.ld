/* Linker links different object files to create final kernel image */

/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */

ENTRY(start)
OUTPUT_FORMAT(elf32-i386)

/* Tell where the various sections of the object files will be put in the final 
   kernel image. */

SECTIONS
{
  /* Begin putting sections at 1 MiB, a conventional place for kernels to be
     loaded at by the bootloader. */
  
  . = 0xC0100000;

  /* First put the multiboot header, as it is required to be put very early
     in the image or the bootloader won't recognize the file format */

  .text : AT(ADDR(.text) - 0xC0000000)
  {
    *(.text)
    *(.rodata)
  }

  /* Read-only data. */
  /*
  .rodata BLOCK(4K) : ALIGN(4K)
  {
    *(.rodata)
  }
  */
  /* Read-write data (initialized) */
  
  .data ALIGN(4K) : AT(ADDR(.data) - 0xC0000000)
  {
    *(.data)
  }

  /* Read-write data (uninitialized) and stack */
  
  .bss : AT(ADDR(.bss) - 0xC0000000)
  {
    _sbss = .;
    *(COMMON)
    *(.bss)
    _ebss = .;
  }

  /* The compiler may produce other sections, by default it will put them in
     a segment with the same name. Simply add stuff here as needed. */
}

/* Compile with "i686-elf-gcc -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc" */