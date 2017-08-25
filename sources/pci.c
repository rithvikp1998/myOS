#include <stdint.h>
#include <stdbool.h>
#include "mylibc.h"
#include "tty.h"
#include "pci.h"

uint32_t pci_read_port(uint32_t bus, uint8_t device, uint8_t function, 
					   uint8_t offset)
{
	uint32_t id = 	(1 << 31)
					| (bus << 16)
					| (device << 11)
					| (function << 8)
					| (offset & 0xfc);

	asm_write_port(PCI_COMMAND_PORT, id);
	uint32_t result = asm_read_port(PCI_DATA_PORT);
	return result >> (8 * (offset % 4));
}

void pciWritePort(uint32_t bus, uint8_t device, uint8_t function, 
				  uint8_t offset, uint32_t value)
{
	uint32_t id = 	(1 << 31)
					| (bus << 16)
					| (device << 11)
					| (function << 8)
					| (offset & 0xfc);

	asm_write_port(PCI_COMMAND_PORT, id);
	asm_write_port(PCI_DATA_PORT, value);
}

bool device_has_functions(uint8_t bus, uint8_t device)
{
	return pci_read_port(bus, device, 0, 0x0E) & (1<<7);
}

pci_device_descriptor get_device_descriptor(uint8_t bus, uint8_t device, 
											uint8_t function)
{
	pci_device_descriptor dev;
	
	dev.bus = bus;
	dev.device = device;
	dev.function = function;
	
	dev.vendor_id = pci_read_port(bus, device, function, 0x00);
	dev.device_id = pci_read_port(bus, device, function, 0x02);

	dev.class_id = pci_read_port(bus, device, function, 0x0B);
	dev.subclass_id = pci_read_port(bus, device, function, 0x0A);
	dev.interface_id = pci_read_port(bus, device, function, 0x09);

	dev.interrupt_register = pci_read_port(bus, device, function, 0x3C);
	dev.revision = pci_read_port(bus, device, function, 0x08);
	
	return dev;
}

void scan_all_pci_buses(void)
{
	uint16_t bus;
	uint8_t device, function;
	for(bus=0; bus<256; bus++) {
		for(device=0; device<32; device++) {
			int num_functions = device_has_functions((bus & 0xFF),device) ? 8 : 1 ;
			for(function=0; function<num_functions; function++) {
				pci_device_descriptor dev = get_device_descriptor((bus & 0xFF), device, function);
				if(dev.vendor_id == 0x0000 || dev.vendor_id == 0xFFFF)
					break;
				printx((bus & 0xFF));
				terminal_putchar(' ');
				printx(device);
				terminal_putchar(' ');
				printx(dev.vendor_id);
				terminal_putchar(' ');
				printx(dev.device_id);
				terminal_putchar('\n');
			}
		}
	}
}
