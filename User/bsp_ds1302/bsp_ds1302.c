#include "bsp_ds1302.h"
#include "bsp_delay.h"
#include <reg51.h>
#include <intrins.h>

sbit DS1302_SCLK = P3 ^ 3;
sbit DS1302_DAT = P3 ^ 4;
sbit DS1302_RST = P3 ^ 5;

void ds1302_write_byte(u8 dat)
{
        u8 i;
        for(i = 0; i < 8; i++)
        {
                DS1302_DAT = dat & 0x01;
                DS1302_SCLK = 0;
                _nop_();
                DS1302_SCLK = 1;;
                dat >>= 1;
        }
}

u8 ds1302_read_byte(void)
{
        u8 i, dat = 0;
        for(i = 0; i < 8; i++)
        {
                if(DS1302_DAT)
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

u8 ds1302_read_register(u8 addr)
{
        u8 dat;
        addr = addr | 0x01;
        DS1302_RST = 0;
        DS1302_SCLK = 0;
        DS1302_RST = 1;
        ds1302_write_byte(addr);
        dat = ds1302_read_byte();
        DS1302_SCLK = 1;
        DS1302_RST = 0;
        DS1302_SCLK = 0;
        return (dat);
}

void ds1302_write_register(u8 addr, u8 dat)
{
        DS1302_RST = 0;
        DS1302_SCLK = 0;
        DS1302_RST = 1;
        ds1302_write_byte(addr);
        ds1302_write_byte(dat);
        DS1302_SCLK = 1;
        DS1302_RST = 0;
        DS1302_SCLK = 0;
}
