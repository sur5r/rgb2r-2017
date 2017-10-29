
#include <conio.h>
#include <cbm.h>
#include <stdint.h>

#include "bcd2dec.h"

uint8_t buffer[256];

int main(void)
{
    int c,i;

    videomode(VIDEOMODE_80x25);
    fast();

    __asm__("jsr initsystime");

    c = cbm_open((uint8_t)1, (uint8_t)8, (uint8_t)8, "data,s,a");
    cprintf("[%2d] open\r\n", bcd2dec(CIA1.tod_sec));
    if(c)
    {
        return 1;
    }
    for(i=0; i< 32; i++)
    {
        c = cbm_write((uint8_t)1, buffer, 256);
        cprintf("[%2d] write\r\n", bcd2dec(CIA1.tod_sec));
        if(c != 256)
        {
            return 1;
        }
    }
    cbm_close(1);
    cprintf("[%2d] close\r\n",bcd2dec(CIA1.tod_sec));
    return 0;
}
