#include <reg52.h>
#include "main.h"
#include "bsp_delay.h"
#include "bsp_key.h"
#include "bsp_ds1302.h"
#include "bsp_ds18b20.h"

/* 定义控制数码管的引脚 */
#define Segment P0                              // 数码管段选控制
#define Digital P2                              // 数码管位选控制

u8 DigCode[4] = {0x01, 0x02, 0x04, 0x08};       // 数码管位选表
u8 SegCode[17] =                                // 共阳数码管段选码
{
        ~0x3f, ~0x06, ~0x5b, ~0x4f, ~0x66,      // 0, 1, 2, 3, 4,
        ~0x6d, ~0x7d, ~0x07, ~0x7f, ~0x6f,      // 5, 6, 7, 8, 9,
        ~0x77, ~0x7c, ~0x39, ~0x5e, ~0x79,      // a, b, c, d, e,
        ~0x71, ~0x40                            // f, -
};

u8 time0_interrupt_count = 0;                   // 中断计数变量
u8 time1_interrupt_count = 0;                   // 中断计数变量
u8 scan_count = 20;                             // 每次闪烁切换之间动态扫描的次数
sbit beep = P3^6;                               // 定义控制 beep 的 I/O 引脚的变量

/* 定义模型变量 */
u8 location = 0;                                // 记录当期是哪一位闪烁
u8 value[4];                                    // 记录当前数码管显示的值
u8 alarm[4] = {0, 8, 0, 0};                     // 记录闹钟响铃的时刻

enum flicker_state blink = flicker_off;         // 控制数码管时候闪烁显示
enum symbol_state point = symbol_off;           // 控制小数点的显示
enum symbol_state colon = symbol_on;            // 控制冒号的显示
enum display_mode state = ClockDisplay;         // 开机默认为时钟显示模式

int main()
{
        u8 i, j;                                // 循环需要用到的临时变量
        u8 tmp1, tmp2;                          // 暂存 DS1302 读取出来的数据
        
        /* 配置外部中断 0, 用于捕捉按键的输入数据 */
        IT0 = 1;                                // 设置外部中断 0 触发方式为下降沿触发
        EX0 = 1;                                // 开启外部中断 0 中断
        
        /* 配置定时器 0 中断, 用于实现 2 秒后自动返回功能 */
        TMOD = 0x01;                            // 定时器 0 工作在方式 1: 16 位定时计数模式
        TH0 = 0x0b;                             // 这里计数初值为 3036 计数 62500 溢出, 64 次就可以得到 4000000 次计数, 即 2 秒
        TL0 = 0xdc;
        ET0 = 1;                                // 开启定时器 0 中断
        
        /* 配置定时器 1 中断, 用于实现 0.5 秒后冒号闪烁功能 */
        TMOD = TMOD | 0x10;                     // 定时器 1 工作在方式 1: 16 位定时计数模式
        TH1 = 0x0b;                             // 这里计数初值为 3036
        TL1 = 0xdc;
        ET1 = 1;                                // 开启定时器 1 中断
        EA = 1;                                 // 开启总中断
        TR1 = 1;                                // 开启定时器 1

        beep = 0;                               // 开机默认关闭蜂鸣器

        ds1302_write_register(DS1302_YEAR, 0x17);
        ds1302_write_register(DS1302_MONTH, 0x05);
        ds1302_write_register(DS1302_DATE, 0x31);
        ds1302_write_register(DS1302_WEEK, 0x03);
        ds1302_write_register(DS1302_HOUR, 0x00);
        ds1302_write_register(DS1302_MIN, 0x00);
        ds1302_write_register(DS1302_SEC, 0x00);

        ds18b20_get_temperature();

        while(1)
        {
                for(j = scan_count; j > 0; j--)
                {
                        for(i = 0; i < 4; i++)
                        {
                                if(state == ClockDisplay)
                                {
                                        tmp1 = ds1302_read_register(DS1302_HOUR);
                                        tmp2 = ds1302_read_register(DS1302_MIN);
                                        value[0] = (tmp1 & 0xf0) >> 4;
                                        value[1] = tmp1 & 0x0f;
                                        value[2] = (tmp2 & 0xf0) >> 4;
                                        value[3] = tmp2 & 0x0f;
                                }

                                Digital = DigCode[i];
                                Segment = SegCode[value[i]];

                                if((point == symbol_on) && (0 == i))
                                        Segment = Segment & 0x7f;

                                if((colon == symbol_on) && (1 == i))
                                        Segment = Segment & 0x7f;

                                delay_us(500);
                        }
                }

                if(blink == flicker_on)
                {
                        for(j = scan_count; j > 0; j--)
                        {
                                for(i = 0; i < 4; i++)
                                {
                                        Digital = DigCode[i];
                                        if(location == i)
                                                Segment = 0xff;
                                        else
                                                Segment = SegCode[value[i]];

                                        if((point == symbol_on) && (0 == i))
                                        {
                                                Segment = Segment & 0x7f;
                                        }

                                        if((colon == symbol_on) && (1 == i))
                                        {
                                                Segment = Segment & 0x7f;
                                        }

                                        delay_us(500);
                                }
                        }
                }

                if(state == ClockDisplay)
                {
                        if((value[0] == alarm[0]) && (value[1] == alarm[1]) && (value[2] == alarm[2]) && (value[3] == alarm[3]))
                                beep = 1;
                        else
                                beep = 0;
                }
        }
}

/**
 * @Descroption 外部中断 0 服务函数
 */
void INT0_Handler() interrupt 0
{
        key_control(key_scan());
}

/**
 * @Descroption 定时器 0 中断服务函数
 */
void TIM0_Handler() interrupt 1
{
        TH0 = 0x0b;                             /* 重新转载计数初值 3036 */
        TL0 = 0xdc;

        time0_interrupt_count++;                /* 每中断一次, 中断计数变量加一 */

        if(time0_interrupt_count >= 64)         /* 定时时间到了 2 秒之后返回时间显示界面 */
        {
                time0_interrupt_count = 0;      // 清空中断计数值
                TR0 = 0;                        // 关闭定时器
                state = ClockDisplay;           // 将电子表工作模式切换始终显示模式
                colon = symbol_on;              // 显示模式下显示冒号
                point = symbol_off;             // 显示模式下不显示点号
        }
}

/**
 * @Descroption 定时器 1 中断服务函数
 */
void TIM1_Handler() interrupt 3
{
        TH1 = 0x0b;                             /* 重新转载计数初值 */
        TL1 = 0xdc;

        time1_interrupt_count++;                /* 每中断一次, 中断计数变量加一 */

        if(time1_interrupt_count >= 16)         /* 定时时间到了 0.5 秒切换冒号的状态, 实现闪烁效果 */
        {
                time1_interrupt_count = 0;      // 清空中断计数值

                if(state == ClockDisplay)
                {
                        if(colon == symbol_on)
                                colon = symbol_off;
                        else
                                colon = symbol_on;
                }
        }
}
