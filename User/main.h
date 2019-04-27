#ifndef __MAIN_H
#define __MAIN_H

/* 类型重定义 */
#define u8 unsigned char  // 一个字节  8 位
#define u16 unsigned int  // 两个字节 16 位
#define u32 unsigned long // 四个字节 32 位 \
                          // 另外 unsigned short 也是 16 位

/* 电子表工作模式枚举 */
enum display_mode
{
        ClockDisplay = 0,       // 时钟显示模式：显示当前时间。示例 12:20
        ClockSet = 1,           // 时钟设置模式：设置当期时间。
        DateDisplay = 2,        // 日期显示模式：显示当前日期。示例 01.22
        DateSet = 3,            // 日期设置模式：设置当前日期。
        YearDisplay = 4,        // 年份显示模式：显示当前年份。示例 2017
        YearSet = 5,            // 年份设置模式：设置当前年份。
        WeekDisplay = 6,        // 星期显示模式：显示当前是星期几。示例 --3-
        WeekSet = 7,            // 星期设置模式：设置当前是星期几。
        AlarmDisplay = 8,       // 闹钟显示模式：显示闹钟设定时间。示例 07:10
        AlarmSet = 9,           // 闹钟设置模式：设置闹钟响铃时刻。
        TemperatureDisplay = 10 // 温度显示模式：显示当前温度。示例 23.6C
};

/* 数码管显示/闪烁枚举 */
enum flicker_state
{
        flicker_off = 0, // 数码管常亮显示
        flicker_on = 1   // 数码管闪烁显示
};

/* 数码管符号点亮/熄灭枚举 */
enum symbol_state
{
        symbol_off = 0, // 数码管符号点亮
        symbol_on = 1   // 数码管符号熄灭
};

#endif /* __MAIN_H */