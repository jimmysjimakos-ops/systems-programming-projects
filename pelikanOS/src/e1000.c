
#include "e1000.h"
#include "pmm.h"
#include "vmm.h"
#include "string.h"
#include "pci.h"

static volatile e1000_rx_desc_t *RX_descriptor_array;
static volatile e1000_tx_desc_t *TX_descriptor_array;
static uint32_t rx_cur;
static uint32_t tx_cur;
static volatile uint32_t *regs;
static uint8_t mac[6];

static void find_e1000_and_set_mac(){
    uint32_t bar0 = vmm_map_bar0();
    
    regs = (volatile uint32_t *)bar0;
    uint32_t RAL0 = regs[0x5400 / 4];
    uint32_t RAH0 = regs[0x5404 / 4];
    //mac is 6 bytes , 4 from RAL0 and 2 from RAH0 , lowest byte first
    
    mac[0] = RAL0 & 0xFF;
    mac[1] = (RAL0 >> 8) & 0xFF;
    mac[2] = (RAL0 >> 16) & 0xFF;
    mac[3] = (RAL0 >> 24) & 0xFF;
    mac[4] = RAH0 & 0xFF;
    mac[5] = (RAH0 >> 8) & 0xFF;
}

static void RX_ring_init(){
    uint32_t frame = (uint32_t) pmm_alloc_frame();
    if(frame == 0xFFFFFFFF){
        //print error message
        return;
    }
    //set up the descriptor array and allocate buffers (space) for each one
    RX_descriptor_array = (e1000_rx_desc_t *) frame;
    memset(RX_descriptor_array , 0 , 4096);
    for(int i=0; i<32; i+=2){
        uint32_t base = (uint32_t) pmm_alloc_frame();
        if(base ==  0xFFFFFFFF){
            //print message and maybe run a script to free allocated frames
            return;
        }
        RX_descriptor_array[i].addr =  base;
        RX_descriptor_array[i].status = 0;
        RX_descriptor_array[i+1].addr =  base + 2048;
        RX_descriptor_array[i+1].status = 0;
    }
    //show the registers where all this is 

    regs[0x2800 / 4] = (uint32_t) RX_descriptor_array; //RDBAL Receive Descriptor Base Address Low:  low 32 bits of the descriptor ring's physical address
    regs[0x2804 / 4] = 0; //RDBAH Receive Descriptor Base Address High: upper 32 bits (0, we're 32-bit)
    regs[0x2808 / 4] = 32 * 16; //RDLEN Receive Descriptor Length: ring size in BYTES (32 descriptors x 16 bytes), must be a multiple of 128
    regs[0x2810 / 4] = 0; //RDH Receive Descriptor Head: the card's write position, it advances this itself as it fills buffers
    regs[0x2818 / 4] = 31; //RDT Receive Descriptor Tail: how far the card may fill; we write it to hand descriptors back
    regs[0x0100 / 4] = (1 << 1) | (1 << 15) | (1 << 26); //RCTL - basically enables the reciever and accepts broadcast frame , also strips ethernet CRC from received frames
    //with this we setup the RX buffers the NIC needs

}

static void TX_ring_init(){
    uint32_t frame = (uint32_t) pmm_alloc_frame();
    if(frame == 0xFFFFFFFF){
        //print error message
        return;
    }
    //set up the descriptor array and allocate buffers (space) for each one
    TX_descriptor_array = (e1000_tx_desc_t *) frame;
    memset(TX_descriptor_array , 0 , 4096);
    for(int i=0; i<32; i+=2){
        uint32_t base = (uint32_t) pmm_alloc_frame();
        if(base ==  0xFFFFFFFF){
            //print message and maybe run a script to free allocated frames
            return;
        }
        TX_descriptor_array[i].addr =  base;
        TX_descriptor_array[i].status = 0;
        TX_descriptor_array[i+1].addr =  base + 2048;
        TX_descriptor_array[i+1].status = 0;
    }
    //show the registers where all this is 

    regs[0x3800 / 4] = (uint32_t) TX_descriptor_array; //TDBAL ring address low
    regs[0x3804 / 4] = 0; // TDBAH ring address high (0)
    regs[0x3808 / 4] = 32 * 16; //TDLEN ring size in bytes
    regs[0x3810 / 4] = 0; //TDH card's position: next descriptor it will send
    regs[0x3818 / 4] = 0; //TDT your position: "send everything up to here"
    regs[0x0400 / 4] = (1 << 1) | (1 << 3) | (0x10 << 4) | (0x40 << 12); //TCTL 
    //with this we setup the TX buffers the NIC needs
}

static void bus_master_enable(){
    uint32_t cmd = pci_read_config(0, 3, 0, 0x04);
    cmd |= (1<<2);
    pci_write_config(0, 3, 0, 0x04, cmd);
}

void e1000_init(){
    //map registers+set regs and mac
    find_e1000_and_set_mac();
    bus_master_enable();
    RX_ring_init();
    TX_ring_init();
}

void e1000_send(const uint8_t *data , uint16_t len){
    if(len > 2048){
        //print: too much len
        return;
    }
    memcpy((void *) (uint32_t) TX_descriptor_array[tx_cur].addr , (void *) data , len); //this uint32_t conversion avoids an error and is only really for that!
    TX_descriptor_array[tx_cur].length = len;
    TX_descriptor_array[tx_cur].cmd = (1 << 0) | (1 << 1) | (1 << 3);  // first is for signaling end of packet (other packets might be bigger than 2048 - not mine) , second is for Ethernet checksum (CRC) , third for reporting if bytes were sent
    TX_descriptor_array[tx_cur].status = 0;
    tx_cur = (tx_cur + 1) % 32;
    regs[0x3818 / 4] = tx_cur; //TDT - it changed value so the card knows to send everything up to the new value
}

uint16_t e1000_receive(uint8_t *out){
    if(RX_descriptor_array[rx_cur].status & 1){
        uint16_t len = RX_descriptor_array[rx_cur].length;
        memcpy((void *) out , (void *) (uint32_t) TX_descriptor_array[rx_cur].addr , len);
        RX_descriptor_array[rx_cur].status = 0;
        uint32_t done = rx_cur;
        rx_cur = (rx_cur + 1) % 32;
        regs[0x2818 / 4] = done;   //RDT 
        return len;
    }else{
        return 0;
    }
} //check this whole func next