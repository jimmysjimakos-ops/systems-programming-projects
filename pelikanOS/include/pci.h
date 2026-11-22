#ifndef PCI_H
#define PCI_H

#include <stdint.h>

uint32_t pci_read_config(uint8_t bus , uint8_t device , uint8_t func , uint8_t offset);
void pci_scan(void);
uint32_t vmm_map_bar0();
void pci_config_write();
#endif