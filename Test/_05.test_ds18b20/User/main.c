#include <reg52.h>
#include "main.h"
#include "bsp_delay.h"
#include "bsp_ds18b20.h"

/* 定义控制数码管的引脚，这里使用宏定义形式是便于以后程序的移植 */
#define Segment P0
#define Digital P2

/* 共阳极数码管段选码、位选码 '0123456789abcdef-' */
u8 SegCode[17] = 
{
        ~0x3f, ~0x06, ~0x5b, ~0x4f,
        ~0x66, ~0x6d, ~0x7d, ~0x07,
        ~0x7f, ~0x6f, ~0x77, ~0x7c,
        ~0x39, ~0x5e, ~0x79, ~0x71,
        ~0x40
};

u8 DigCode[4] = {0x01, 0x02, 0x04, 0x08};

int value[4] = {0, 0, 0, 0};

sbit beep = P3^6;

void main()
{
        int i = 0;
        
        u16 temp = 0;
        
        beep = 0;
        
        while(1)
        {
                for(i = 0; i < 4; i++)
                {
                        Segment = SegCode[value[i]];
                        if(0 == i)
                                Segment = Segment & 0x7f;
                        Digital = DigCode[i];
                        
                        temp = read_temperature();
                        value[0] = (temp >> 4) / 10;
                        value[0] = value[0] % 10;
                        value[1] = (temp >> 4) % 10;
                        value[2] = ((temp & 0x000f) * 625) / 1000;
                        value[3] = ((temp & 0x000f) * 625) / 100;
                        value[3] = value[3] % 10;
                        
                        delay_ms(3);
                }
        }
}
