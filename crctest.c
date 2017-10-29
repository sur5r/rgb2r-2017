
#include <conio.h>
#include <cbm.h>
#include <stdint.h>

#include "bcd2dec.h"
#include "crc16.h"

#define BUFSZ 16

uint8_t buffer[BUFSZ] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};

int main(void)
{
    uint16_t i=0;
    uint16_t crc=0xFFFF;
    uint8_t sec, tenth;

    videomode(VIDEOMODE_80x25);
    fast();

    __asm__("jsr initsystime");

    for(i=0; i<BUFSZ*256; i++)
    {
        crc = crc16_ccitt(crc, buffer[i%256]);
    }

    tenth = CIA1.tod_10;
    sec = CIA1.tod_sec;

    cprintf("Finished after %d.%d\r\n", bcd2dec(sec), bcd2dec(tenth));

    return 0;
}

