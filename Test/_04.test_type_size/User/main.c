#include <reg52.h>
#include "main.h"
#include "bsp_delay.h"

/* 定义控制数码管的引脚，这里使用宏定义形式是便于以后程序的移植 */
#define Segment P0
#define Digital P2

u8 value[4] = {1, 9, 9, 5};                     // 记录当前数码管显示的值

/* 共阳极数码管段选码、位选码 '0123456789abcdef-' */
u8 SegCode[17] = 
{
        0x3f, 0x06, 0x5b, 0x4f,
        0x66, 0x6d, 0x7d, 0x07,
        0x7f, 0x6f, 0x77, 0x7c,
        0x39, 0x5e, 0x79, 0x71,
        0x40
};

u8 DigCode[4] = {0x01, 0x02, 0x04, 0x08};

sbit beep = P3^6;

int main()
{
        u8 i;
        
        unsigned long  t0 = 0;
        unsigned int   t1 = 0;
        unsigned short t2 = 0;
        unsigned char  t3 = 0;
        
        /* 关闭蜂鸣器 */
        beep = 0;
        
//        printf("unsigned long:  %d\n", sizeof(t0));
//        printf("unsigned int:   %d\n", sizeof(t1));
//        printf("unsigned short: %d\n", sizeof(t2));
//        printf("unsigned char:  %d\n", sizeof(t3));
        
        /* 通过数码管显示 */
        value[0] = sizeof(t0);
        value[1] = sizeof(t1);
        value[2] = sizeof(t2);
        value[3] = sizeof(t3);
        
        while(1)
        {
                for(i = 0; i < 4; i++)
                {
                        Segment = ~SegCode[value[i]];
                        Digital = DigCode[i];
                        delay_ms(1);
                }
        }
}