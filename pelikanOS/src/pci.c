#include "pci.h"
#include "io.h"
#include "vga.h"
#include "itoa.h"

/*
bit 31     : enable (must be 1)
bits 30-24 : reserved (0)
bits 23-16 : bus number      (0-255)
bits 15-11 : device/slot     (0-31)
bits 10-8  : function        (0-7)
bits 7-2   : register offset / 4  (dword-aligned)
bits 1-0   : always 0 (dword alignment)
*/

uint32_t pci_read_config(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset){
    uint32_t address = (1u << 31) | (bus << 16) | (device << 11) | (func << 8) | (offset & 0xFC);
    outl(0xCF8 , address);
    return inl(0xCFC);
}

void pci_write_config(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset , uint32_t value){
    uint32_t address = (1u << 31) | (bus << 16) | (device << 11) | (func << 8) | (offset & 0xFC);
    outl(0xCF8 , address);
    outl(0xCFC, value);
}

void pci_scan(void){ //for each bus , device , func  , print the device id
    for(uint32_t bus = 0; bus < 256; bus++){
        for(uint8_t device = 0; device < 32; device++){
            for(uint8_t func = 0; func < 8; func++){
                uint32_t dword = pci_read_config(bus , device , func , 0);
                uint16_t vendor = dword & 0xFFFF;
                if(vendor == 0xFFFF){ //skip everything thats has this id
                    continue;
                }
                uint16_t dev_id = (dword >> 16) & 0xFFFF;
                char buf1[20];
                char buf2[20];
                char buf3[20];
                char buf4[20];
                char buf5[20];

                itoa(vendor, buf1);
                itoa(dev_id, buf2);
                itoa(bus, buf3);
                itoa(device, buf4);
                itoa(func, buf5);

                print(buf1);
                print(":");
                print(buf2);
                print(" for ");
                print("bus : ");
                print(buf3);
                print(" ");
                print("device : ");
                print(buf4);
                print(" ");
                print("func : ");
                print(buf5);
                print("\n");
            }
        }
    }
}

uint32_t vmm_map_bar0(){
    uint32_t bar0 = pci_read_config(0, 3, 0, 0x10) & 0xFFFFFFF0;
    for(uint32_t off = 0; off < 0x20000; off += 4096){
        vmm_map_page(bar0 + off, bar0 + off); 
    } //for each 4KBs , identity map the frames starting from bar0's address up to + 128KBs
    return bar0;
}
