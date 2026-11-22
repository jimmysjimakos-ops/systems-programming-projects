#ifndef E1000_H
#define E1000_H

#include <stdint.h>

/*
Description of the header:

e1000 has a register which holds addresses of other registers that hold the mac address of the NIC  ,
and other registers that are used to feed the NIC space (RX and RT ringbuffers)
*/

//this is the RX ring that uses 32 descriptors meaning an array that holds addresses of 32 array addresses!
typedef struct {
    uint64_t addr; //physical address of the descriptors
    uint16_t length; //the length of bytes the NIC fills in our RX buffer (bytes received)
    uint16_t checksum; //error checking i guess?
    uint8_t status; //field that signals if the descriptor is filled - card sets bit 0 (DD = descriptor done) when filled
    uint8_t errors; //the errors of the checksum?
    uint16_t special; // i have no idea
} __attribute__((packed)) e1000_rx_desc_t;

typedef struct {
    uint64_t addr;     // physical address of the frame to send
    uint16_t length;   // how many bytes to send
    uint8_t  cso;
    uint8_t  cmd;      // what to do with it (bits below)
    uint8_t  status;   // card sets DD (bit 0) when sent, if you asked via RS
    uint8_t  css;
    uint16_t special;
} __attribute__((packed)) e1000_tx_desc_t;

void e1000_init(void);
void e1000_send(const uint8_t *data, uint16_t len);
uint16_t e1000_receive(uint8_t *out);

#endif