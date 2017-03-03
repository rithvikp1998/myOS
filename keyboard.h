#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define KERNEL_OFFSET 0x08
#define INTERRUPT_GATE 0x8e

extern void write_port(uint16_t port, uint8_t data);
extern char read_port(uint16_t port);
extern void keyboard_handler(void);
extern void terminal_putchar(char c);
extern void terminal_backspace(void);

uint32_t keyboard_address;

void fill_kb_idt_entry(void);
void keyboard_init(void);
void keyboard_handler_main(void);

#endif