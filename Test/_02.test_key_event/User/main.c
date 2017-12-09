#include <reg52.h>
#include "main.h"
#include "bsp_delay.h"
#include "bsp_key.h"

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
        /* 配置外部中断0，用于捕捉按键的输入数据 */
        IT0 = 1;                                // 设置外部中断0触发方式为下降沿触发
        IE = IE | 0x81;                         // 开启总中断和外部中断0中断
        
        beep = 0;
        
        while(1)
        {
                Digital = DigCode[0];
                Segment = ~SegCode[value[0]];
                delay_ms(10);
        }
}

/**
 * @Descroption 外部中断0服务函数
 */ 
void INT0_Handler() interrupt 0
{
        Key_Control(Key_Scan());
}