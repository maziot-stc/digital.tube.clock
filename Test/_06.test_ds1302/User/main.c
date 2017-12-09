#include <reg52.h>
#include "main.h"
#include "bsp_delay.h"
#include "bsp_key.h"
#include "bsp_ds1302.h"

#define Segment P0
#define Digital P2

u8 display_mode = 0;
u8 InterruptCount = 0;
u8 clock_symbol = 1;
u8 point_symbol = 0;
sbit beep = P3^6;
u8 value[4] = {0, 0, 0, 0};

u8 DigCode[4] = {0x01, 0x02, 0x04, 0x08};
u8 SegCode[17] = 
{
        0x3f, 0x06, 0x5b, 0x4f,
        0x66, 0x6d, 0x7d, 0x07,
        0x7f, 0x6f, 0x77, 0x7c,
        0x39, 0x5e, 0x79, 0x71,
        0x40
};

int main()
{
        u8 i;
        u8 tmp1, tmp2;
      
        beep = 0;
        
        DS1302_WriteRegister(DS1302_YEAR,  0x17);
        DS1302_WriteRegister(DS1302_MONTH, 0x05);
        DS1302_WriteRegister(DS1302_DATE,  0x17);
        DS1302_WriteRegister(DS1302_DAY,   0x03);
        DS1302_WriteRegister(DS1302_HR,    0x16);
        DS1302_WriteRegister(DS1302_MIN,   0x21);
        DS1302_WriteRegister(DS1302_SEC,   0x00);
        
        IT0 = 1;
        IE = IE | 0x81;
        
        TMOD = 0x01;
        TH0 = 0x0b;
        TL0 = 0xdc;
        IE = IE | 0x82;
        TR0 = 0;
        
        while(1)
        {
                for(i = 0; i < 4; i++)
                {
                        if(0 == display_mode)
                        {
                                tmp1 = DS1302_ReadRegister(DS1302_HR);
                                tmp2 = DS1302_ReadRegister(DS1302_MIN);
        
                                value[0] = (tmp1 & 0xf0) >> 4;
                                value[1] = tmp1 & 0x0f;
                                value[2] = (tmp2 & 0xf0) >> 4;
                                value[3] = tmp2 & 0x0f;
                        }
                        
                        Digital = DigCode[i];
                        Segment = ~SegCode[value[i]];
                        
                        if((1 == point_symbol) && (0 == i))
                        {
                                Segment = Segment & 0x7f;
                        }
                        
                        if((1 == clock_symbol) && (1 == i))
                        {
                                Segment = Segment & 0x7f;
                        }
                        
                        delay_us(100);
                }
        }
}

void INT0_Handler() interrupt 0
{
        Key_Control(Key_Scan());
}

void TIM0_Handler() interrupt 1
{
        TH0 = 0x0b;
        TL0 = 0xdc;
        
        InterruptCount++;
        
        if(InterruptCount >= 64)
        {
                InterruptCount = 0;
                TR0 = 0;
                display_mode = 0;
                clock_symbol = 1;
                point_symbol = 0;
        }
}
