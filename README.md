# myOS


## Intro

As 2017 New Year Resolution, I set out to make a Single Board Computer(SBC) from scratch and then write an OS for it. 
The SBC is ARM based and the design files can be found [here.](https://www.github.com/rithvikp1998/SBC) Though my initial
plan was to write an OS for the board(i.e. ARM based), I decided to write an x86 based OS first because of abundantly available
resources and then use what I've learnt to build a smaller ARM based OS. 

## Building the code

To compile the code, you need a cross compiler which can be built by following the instructions given 
[here.](http://wiki.osdev.org/GCC_Cross-Compiler) Make sure that you've built the compiler in the same directory
as the OS's source code. 

After the build is over, simply run ``` make ``` to compile and use a virtual machine to run
the generated ``` .bin ``` file. For qemu, this is the commad ```  qemu-system-i386 -kernel myos.bin ``` 

## Other Resources

[OSDev](http://wiki.osdev.org)

[Brokenthorn](http://www.brokenthorn.com/Resources/)

[Little OS book](https://littleosbook.github.io/)
