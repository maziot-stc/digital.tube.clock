#include "bsp_ds18b20.h"
#include <intrins.h>

void delay(u16 i)
{
        while(i--);
}

u8 ds18b20_init(void)
{
        u8 status = 0;
        DQ = 1;
        delay(8);
        DQ = 0;
        delay(70);
        DQ = 1;
        delay(14);
        status = DQ;
        delay(10);
        return status;
}

void ds18b20_write_byte(u8 dat)
{
        u8 i ;
        for(i = 0; i < 8; i++)
        {
                DQ = 0;
                DQ = dat & 0x01;
                delay(4);
                DQ = 1;
                dat >>= 1;
        }
        delay(8);
}

u8 ds18b20_read_byte(void)
{
        u8 i, dat = 0;
        DQ = 1;
        _nop_(); 
        for(i = 0; i < 8; i++)
        {
                DQ = 0;
                dat>>=1;
                DQ = 1;
                _nop_();
                _nop_();
                if(DQ == 1)
                        dat |= 0x80;
                delay(4);
                DQ = 1;
        }
        return dat;
}

u16 ds18b20_get_temperature(void)
{
        u8 low = 0;
        u8 high = 0;
        
        ds18b20_init();
        ds18b20_write_byte(0xcc);
        ds18b20_write_byte(0x44);
        ds18b20_init();
        ds18b20_write_byte(0xcc);
        ds18b20_write_byte(0xbe);
        low = ds18b20_read_byte();
        high = ds18b20_read_byte();
        
        return (((high & 0x00ff) << 8) | (low & 0x00ff));
}
