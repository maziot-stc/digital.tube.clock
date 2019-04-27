#include "bsp_key.h"
#include "bsp_delay.h"
#include "bsp_ds1302.h"
#include "bsp_ds18b20.h"
#include <intrins.h>

/* 这里需要使用 extern 声明其他 c 文件定义的变量 */
extern u8 location;
extern u8 value[4];
extern u8 alarm[4];
extern u8 time0_interrupt_count;

extern enum flicker_state blink;
extern enum symbol_state point;
extern enum symbol_state colon;
extern enum display_mode state;

/** 
 * @Description 按键扫描函数(8个按键同时扫描)
 * @return u8   按键键值
 */
u8 key_scan(void)
{
    /* 保存按键键值的变量，默认值0xff表示没有按键按下 */
    u8 keyValue = 0xff;

    /* 用于松手检测的累加变量 */
    int i = 0;

    /* 这里在 1ms 前后检测两边是为了滤除按键抖动产生的尖峰脉冲 */
    if (GPIO_KEY != 0xff)
    {
        for (i = 0; i < 300; i++)
            _nop_();

        /* 如果 1ms 前后检测都是低电平的话，就说明是真的有按键按下 */
        if (GPIO_KEY != 0xff)
        {
            /* 真的有按键按下，则将按键键值存入 keyValue 中 */
            keyValue = GPIO_KEY;
            /* 这里是松手检测，在 20 条空指令内按键没有放开，程序会一直停在此处 */
            /* 倘若 20 条空指令期间，松开了按键，则会跳出此while循环 */
            /* 也就是说：按键没有松开的话，程序不会去做其他的事情 */
            /* 当然也有松手检测的超时时间，就是我们设置的 20 条空指令 */
            while ((i < 20) && (GPIO_KEY != 0xff))
            {
                _nop_();
                i++;
            }
        }
    }
    /* 返回按键的键值，通过解析这个键值，就能知道是哪一个按键按下 */
    return keyValue;
}

/** 
 * @Description 按键控制函数(8个按键同时控制)
 * @param u8    按键扫描到的键值
 */
void key_control(u8 keyValue)
{
    /* i 用于循环计数，tmp1、tmp2 用于暂存 DS1302 中的数据 */
    u8 i, tmp1 = 0, tmp2 = 0;

    u16 temp = 0;

    /* 根据捕捉的键值解析出按下的键，并作出相应的处理 */
    switch (keyValue)
    {
    /* 1111 1110 P1.0 口对应按键按下 K0 左/年 */
    case (0xfe):
        if (blink == flicker_off)
        {
            tmp1 = ds1302_read_register(DS1302_YEAR);
            value[0] = 2;
            value[1] = 0;
            value[2] = (tmp1 & 0xf0) >> 4;
            value[3] = tmp1 & 0x0f;

            time0_interrupt_count = 0;
            TR0 = 1;

            state = YearDisplay;
            colon = symbol_off;
            point = symbol_off;
        }
        else
        {
            if ((state == ClockSet) || (state == DateSet) || (state == AlarmSet))
            {
                if (location > 0)
                    location--;
            }
            else if (state == YearSet)
            {
                if (location > 2)
                    location--;
            }
        }

        break;

    /* 1111 1101 P1.1 口对应按键按下 K1 右/日期 */
    case (0xfd):
        if (blink == flicker_off)
        {
            tmp1 = ds1302_read_register(DS1302_MONTH);
            tmp2 = ds1302_read_register(DS1302_DATE);
            value[0] = (tmp1 & 0xf0) >> 4;
            value[1] = tmp1 & 0x0f;
            value[2] = (tmp2 & 0xf0) >> 4;
            value[3] = tmp2 & 0x0f;

            time0_interrupt_count = 0;
            TR0 = 1;

            state = DateDisplay;
            colon = symbol_off;
            point = symbol_on;
        }
        else
        {
            if ((state == ClockSet) || (state == DateSet) || (state == AlarmSet) || (state == YearSet))
            {
                if (location < 3)
                    location++;
            }
        }

        break;

    /* 1111 1011 P1.2 口对应按键按下 K2 设置 */
    case (0xfb):
        if (blink == flicker_off)
        {
            TR0 = 0;

            tmp1 = ds1302_read_register(DS1302_HOUR);
            tmp2 = ds1302_read_register(DS1302_MIN);
            value[0] = (tmp1 & 0xf0) >> 4;
            value[1] = tmp1 & 0x0f;
            value[2] = (tmp2 & 0xf0) >> 4;
            value[3] = tmp2 & 0x0f;

            state = ClockSet;
            colon = symbol_on;
            point = symbol_off;
            blink = flicker_on;
            location = 0;
        }
        else
        {
            if (state == ClockSet)
            {
                ds1302_write_register(DS1302_HOUR, (value[0] << 4) | value[1]);
                ds1302_write_register(DS1302_MIN, (value[2] << 4) | value[3]);
            }
            else if (state == YearSet)
            {
                ds1302_write_register(DS1302_YEAR, (value[2] << 4) | value[3]);
            }
            else if (state == DateSet)
            {
                ds1302_write_register(DS1302_MONTH, (value[0] << 4) | value[1]);
                ds1302_write_register(DS1302_DATE, (value[2] << 4) | value[3]);
            }
            else if (state == WeekSet)
            {
                ds1302_write_register(DS1302_WEEK, value[2]);
            }
            else if (state == AlarmSet)
            {
                for (i = 0; i < 4; i++)
                    alarm[i] = value[i];
            }

            state = ClockDisplay;
            colon = symbol_on;
            point = symbol_off;
            blink = flicker_off;
        }

        break;

    /* 1111 0111 P1.3 口对应按键按下 K3 切换/星期 */
    case (0xf7):
        if (blink == flicker_off)
        {
            tmp1 = ds1302_read_register(DS1302_WEEK);
            value[0] = 16;
            value[1] = 16;
            value[2] = tmp1 & 0x0f;
            value[3] = 16;

            time0_interrupt_count = 0;
            TR0 = 1;

            state = WeekDisplay;
            colon = symbol_off;
            point = symbol_off;
        }
        else
        {
            if (state == ClockSet)
            {
                ds1302_write_register(DS1302_HOUR, (value[0] << 4) | value[1]);
                ds1302_write_register(DS1302_MIN, (value[2] << 4) | value[3]);

                tmp1 = ds1302_read_register(DS1302_YEAR);
                value[0] = 2;
                value[1] = 0;
                value[2] = (tmp1 & 0xf0) >> 4;
                value[3] = tmp1 & 0x0f;

                state = YearSet;
                colon = symbol_off;
                point = symbol_off;
                location = 2;
            }
            else if (state == YearSet)
            {
                ds1302_write_register(DS1302_YEAR, (value[2] << 4) | value[3]);

                tmp1 = ds1302_read_register(DS1302_MONTH);
                tmp2 = ds1302_read_register(DS1302_DATE);
                value[0] = (tmp1 & 0xf0) >> 4;
                value[1] = tmp1 & 0x0f;
                value[2] = (tmp2 & 0xf0) >> 4;
                value[3] = tmp2 & 0x0f;

                state = DateSet;
                colon = symbol_off;
                point = symbol_on;
                location = 0;
            }
            else if (state == DateSet)
            {
                ds1302_write_register(DS1302_MONTH, (value[0] << 4) | value[1]);
                ds1302_write_register(DS1302_DATE, (value[2] << 4) | value[3]);

                tmp1 = ds1302_read_register(DS1302_WEEK);
                value[0] = 16;
                value[1] = 16;
                value[2] = tmp1 & 0x0f;
                value[3] = 16;

                state = WeekSet;
                colon = symbol_off;
                point = symbol_off;
                location = 2;
            }
            else if (state == WeekSet)
            {
                ds1302_write_register(DS1302_WEEK, value[2]);

                for (i = 0; i < 4; i++)
                    value[i] = alarm[i];

                state = AlarmSet;
                colon = symbol_on;
                point = symbol_off;
                location = 0;
            }
            else if (state == AlarmSet)
            {
                for (i = 0; i < 4; i++)
                {
                    alarm[i] = value[i];
                }

                tmp1 = ds1302_read_register(DS1302_HOUR);
                tmp2 = ds1302_read_register(DS1302_MIN);
                value[0] = (tmp1 & 0xf0) >> 4;
                value[1] = tmp1 & 0x0f;
                value[2] = (tmp2 & 0xf0) >> 4;
                value[3] = tmp2 & 0x0f;

                state = ClockSet;
                colon = symbol_on;
                point = symbol_off;
                location = 0;
            }
        }

        break;

    /* 1110 1111 P1.4 口对应按键按下 K4 加/闹钟 */
    case (0xef):
        if (blink == flicker_off)
        {
            for (i = 0; i < 4; i++)
                value[i] = alarm[i];

            time0_interrupt_count = 0;
            TR0 = 1;

            state = AlarmDisplay;
            colon = symbol_on;
            point = symbol_off;
        }
        else
        {
            if ((state == ClockSet) || (state == AlarmSet))
            {
                switch (location)
                {
                case (0):
                    if (value[0] < 2)
                        value[0]++;
                    if (value[0] == 2 && value[1] > 3)
                        value[1] = 3;
                    break;
                case (1):
                    if (value[0] < 2 && value[1] < 9)
                        value[1]++;
                    else if (value[0] == 2 && value[1] < 3)
                        value[1]++;
                    break;
                case (2):
                    if (value[2] < 5)
                        value[2]++;
                    break;
                case (3):
                    if (value[3] < 9)
                        value[3]++;
                    break;
                }
            }
            else if (state == YearSet)
            {
                if (value[location] < 9)
                    value[location]++;
            }
            else if (state == DateSet)
            {
                switch (location)
                {
                case (0):
                    if (value[0] < 1)
                        value[0]++;
                    if (value[0] == 1 && value[1] > 2)
                        value[1] = 2;
                    break;
                case (1):
                    if (value[0] < 1 && value[1] < 9)
                        value[1]++;
                    else if (value[0] == 1 && value[1] < 2)
                        value[1]++;
                    break;
                case (2):
                    if (value[2] < 3)
                        value[2]++;
                    if (value[2] == 3 && value[3] > 1)
                        value[3] = 1;
                    break;
                case (3):
                    if (value[2] < 3 && value[3] < 9)
                        value[3]++;
                    else if (value[2] == 3 && value[1] < 1)
                        value[3]++;
                    break;
                }
            }
            else if (state == WeekSet)
            {
                if (value[location] < 7)
                    value[location] += 1;
            }
        }

        break;

    /* 1101 1111 P1.5 口对应按键按下 K5 减/温度 */
    case (0xdf):
        if (blink == flicker_off)
        {
            temp = ds18b20_get_temperature();
            value[0] = (temp >> 4) / 10;
            value[0] = value[0] % 10;
            value[1] = (temp >> 4) % 10;
            value[2] = ((temp & 0x000f) * 625) / 1000;
            value[3] = ((temp & 0x000f) * 625) / 100;
            value[3] = value[3] % 10;

            time0_interrupt_count = 0;
            TR0 = 1;

            state = TemperatureDisplay;
            colon = symbol_off;
            point = symbol_on;
        }
        else
        {
            if (state == DateSet)
            {
                switch (location)
                {
                case (0):
                case (2):
                    if (value[location] > 0)
                        value[location]--;
                    if ((value[location] == 0) && (value[location + 1] == 0))
                        value[location + 1] = 1;
                    break;
                case (1):
                case (3):
                    if ((value[location - 1] == 0) && (value[location] > 1))
                        value[location]--;
                    else if ((value[location - 1] != 0) && value[location] > 0)
                        value[location]--;
                    break;
                }
            }
            else if (state == WeekSet)
            {
                if (value[location] > 1)
                    value[location]--;
            }
            else
            {
                if (value[location] > 0)
                    value[location]--;
            }
        }

        break;
    }
}
