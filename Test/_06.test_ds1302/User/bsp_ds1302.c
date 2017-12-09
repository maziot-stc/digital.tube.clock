#include "bsp_ds1302.h"
#include "bsp_delay.h"

#include <reg51.h>
#include <intrins.h>

sbit DS1302_IO   = P3 ^ 4;
sbit DS1302_SCLK = P3 ^ 3;
sbit DS1302_RST  = P3 ^ 5;

void DS1302_Write(u8 dat)
{
        u8 i;
        for(i = 0; i < 8; i++)
        {
                DS1302_IO = dat & 0x01;
                DS1302_SCLK = 0;
                _nop_();
                DS1302_SCLK = 1;;
                dat >>= 1;
        }
}

u8 DS1302_Read()
{
        u8 i, dat = 0;
        for(i = 0; i < 8; i++)
        {
                if(DS1302_IO)
                {
                        dat |= 0x80;
                }
                DS1302_SCLK = 1;
                _nop_();
                DS1302_SCLK = 0;
                dat >>= 1;
        }
        return dat;
}

u8 DS1302_ReadRegister(u8 addr)
{
        u8 dat;
        addr = addr | 0x01;
        DS1302_RST = 0;
        DS1302_SCLK = 0;
        DS1302_RST = 1;
        DS1302_Write(addr);
        dat = DS1302_Read();
        DS1302_SCLK = 1;
        DS1302_RST = 0;
        DS1302_SCLK = 0;
        return (dat);
}

void DS1302_WriteRegister(u8 addr, u8 dat)
{
        DS1302_RST = 0;
        DS1302_SCLK = 0;
        DS1302_RST = 1;
        DS1302_Write(addr);
        DS1302_Write(dat);
        DS1302_SCLK = 1;
        DS1302_RST = 0;
        DS1302_SCLK = 0;
}
