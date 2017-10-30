#include <conio.h>
#include <cbm.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "bcd2dec.h"

struct relfile
{
    uint8_t fd;
    uint8_t cmdfd;
    uint8_t devnr;
    uint8_t sa;
    uint8_t recsize;
};

#define RELFILENAME_MAX (FILENAME_MAX + 4) // FOO,L,\xFF\0

uint8_t relfile_open(struct relfile *rf, char* name)
{
    char namebuf[RELFILENAME_MAX];

    if(strlen(name) > FILENAME_MAX)
        return ENOENT;
    snprintf(namebuf,RELFILENAME_MAX,"%s,l,%c",name,rf->recsize);
    cbm_open(rf->fd, rf->devnr, rf->sa, namebuf);
    if(_oserror)
        return _oserror;

    cbm_open(rf->cmdfd,rf->devnr,15,NULL);
    if(_oserror)
        return _oserror;
}

uint8_t relfile_is_split_record(struct relfile *rf, uint16_t recnum)
{
    return ((recnum * rf->recsize) % 254) < rf->recsize;
}

uint8_t relfile_pos(struct relfile *rf, uint16_t recnum, uint8_t offset)
{
    char cmdbuf[5]; // 'P' (SA) (RECLO) (RECHI) (OFFSET)

    cmdbuf[0]=80; //P
    cmdbuf[1]=rf->sa + 0x60;
    cmdbuf[2]=(uint8_t)(recnum & 0x00FF);
    cmdbuf[3]=(uint8_t)(recnum >> 8);
    cmdbuf[4]=offset;

    //cprintf("CMD: %2X %2X %2X %2X %2X\r\n", cmdbuf[0], cmdbuf[1], cmdbuf[2], cmdbuf[3], cmdbuf[4]);

    cbm_write(rf->cmdfd,cmdbuf, 5);
    return _oserror;
}

uint8_t relfile_read(struct relfile *rf, uint8_t *buf)
{
    cbm_read(rf->fd, buf, rf->recsize);
    return _oserror;
}

uint8_t relfile_write(struct relfile *rf, uint8_t *buf)
{
    cbm_write(rf->fd, buf, rf->recsize);
    return _oserror;
}

uint8_t relfile_clear(struct relfile *rf)
{
    cbm_write(rf->fd, "\xFF", 1);
    return _oserror;
}

void relfile_close(struct relfile *rf)
{
    cbm_close(rf->cmdfd);
    cbm_close(rf->fd);
}

void c128_perror(uint8_t c, char *msg) {
  cprintf("\r\nError (Code %d) while: %s\r\nOS Error = %d\r\n", c, msg,
          _oserror);
}

int main(void)
{
    char bla[25];
    struct relfile rf;
    uint8_t c=0;
    uint8_t start_sec, start_tenth;
    uint8_t end_sec, end_tenth;
    uint8_t i,j;

    videomode(VIDEOMODE_80x25);
    fast();

    __asm__("jsr initsystime");

    rf.fd=2;
    rf.devnr=8;
    rf.sa=3;
    rf.recsize=16;

    if(relfile_open(&rf, "reldata"))
    {
        c128_perror(c, "relfile_open");
        return 1;
    }

    start_tenth = CIA1.tod_10;
    start_sec = CIA1.tod_sec;

    for(i=1;i<=50;i++)
    {
        //cgetc();
        /*
        cprintf("P%d\r",i);
        if(relfile_pos(&rf, i, 0))
        {
            c128_perror(c, "relfile_pos1");
            return 2;
        }
        */
        if(relfile_is_split_record(&rf, i))
        {
            cprintf("\r\nSPLIT: %d!\r\n",i);
        }
        /*
        if(relfile_pos(&rf, i, 0))
        {
            c128_perror(c, "relfile_pos2");
            return 2;
        }
        cprintf("C%d\r",i);
        if(relfile_clear(&rf))
        {
            c128_perror(c, "relfile_clear");
            return 2;
        }
        */
        srand(i);
        for(j=0;j<rf.recsize;j++)
            bla[j]=rand() & 0x00ff;

        cprintf("W%d\r",i);
        if(relfile_pos(&rf, i, 0))
        {
            c128_perror(c, "relfile_pos3");
            return 2;
        }
        if(relfile_write(&rf, bla))
        {
            c128_perror(c, "relfile_write");
            return 2;
        }
    }

    relfile_close(&rf);
    end_tenth = CIA1.tod_10;
    end_sec = CIA1.tod_sec;
    cprintf("\r\n%d.%d -> %d.%d\r\n",start_sec,start_tenth,end_sec,end_tenth);


    if(relfile_open(&rf, "reldata"))
    {
        c128_perror(0, "relfile_open");
        return 3;
    }
    start_tenth = CIA1.tod_10;
    start_sec = CIA1.tod_sec;

    for(i=1;i<=50;i++)
    {
        cprintf("P%d\r",i);
        if(relfile_pos(&rf, i, 0))
        {
            c128_perror(0, "relfile_pos");
            return 1;
        }
        if(relfile_is_split_record(&rf, i))
        {
            cprintf("\r\nSPLIT!\r\n");
        }
        cprintf("R%d\r",i);
        if(relfile_read(&rf, bla))
        {
            c128_perror(0, "relfile_read");
            return 1;
        }

        cprintf("?%d\r",i);
        srand(i);
        for(j=0;j<rf.recsize;j++)
        {
            uint8_t exp = rand() & 0x00ff;
            if(bla[j] != exp)
            {
                cprintf("\r\nMISMATCH(%2d): IS: %2X EXP: %2X\r\n",j,bla[j],exp);
                cgetc();
            }
        }
    }

    relfile_close(&rf);
    end_tenth = CIA1.tod_10;
    end_sec = CIA1.tod_sec;
    cprintf("\r\n%d.%d -> %d.%d\r\n",start_sec,start_tenth,end_sec,end_tenth);

    return 0;
}

