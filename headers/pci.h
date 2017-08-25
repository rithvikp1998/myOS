#ifndef PCI_H
#define PCI_H

#define PCI_COMMAND_PORT 0xCF8
#define PCI_DATA_PORT 0xCFC

typedef struct _pci_device_descriptor {
	uint32_t port_base;
	uint32_t interrupt_register;

	uint8_t bus;
	uint8_t device;
	uint8_t function;

	uint16_t vendor_id;
	uint16_t device_id;

	uint8_t class_id;
	uint8_t subclass_id;
	uint8_t interface_id;

	uint8_t revision;

} pci_device_descriptor;

static inline void asm_write_port(uint16_t port, uint32_t value)
{
	asm volatile("outl %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint32_t asm_read_port(uint16_t port)
{
    uint32_t result;
    asm volatile("inl %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

uint32_t pci_read_port(uint32_t bus, uint8_t device, uint8_t function, 
					   uint8_t offset);
void pciWritePort(uint32_t bus, uint8_t device, uint8_t function, 
				  uint8_t offset, uint32_t value);
bool device_has_functions(uint8_t bus, uint8_t device);
pci_device_descriptor get_device_descriptor(uint8_t bus, uint8_t device, 
											uint8_t function);
void scan_all_pci_buses(void);

#endif // PCI_H