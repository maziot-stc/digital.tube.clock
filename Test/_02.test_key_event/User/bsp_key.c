#include "bsp_key.h"
#include <intrins.h>

extern u8 value[4];                             // 外部变量申明

/** 
 * @Description 按键扫描函数(8个按键同时扫描)
 * @return u8   按键键值
 */
u8 Key_Scan(void)
{
        /* 保存按键键值的变量，默认值0xff表示没有按键按下 */
        u8 keyValue = 0xff;

        /* 用于松手检测的累加变量 */
        int i = 0;

        /* 这里在 1ms 前后检测两边是为了滤除按键抖动产生的尖峰脉冲 */
        if(GPIO_KEY != 0xff)
        {
                _nop_();

                /* 如果 1ms 前后检测都是低电平的话，就说明是真的有按键按下 */
                if(GPIO_KEY != 0xff)
                {
                        /* 真的有按键按下，则将按键键值存入 keyValue 中 */
                        keyValue = GPIO_KEY;

                        /* 这里是松手检测，在20ms内按键没有放开，程序会一直停在此处 */
                        /* 倘若 50ms 期间，松开了按键，则会跳出此while循环 */
                        /* 也就是说：按键没有松开的话，程序不会去做其他的事情 */
                        /* 当然也有松手检测的超时时间，就是我们设置的20ms */
                        while((i < 20) && (GPIO_KEY != 0xff))
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
void Key_Control(u8 keyValue)
{
        /* 根据捕捉的键值解析出按下的键，并作出相应的处理 */
        switch(keyValue)
        {
                /* 1111 1110 P1.0口对应按键按下 K0 左/年 */
                case(0xfe):
                        value[0] = 1;
                        break;
                
                /* 1111 1101 P1.1口对应按键按下 K1 右/日期 */
                case(0xfd):     
                        value[0] = 2;
                        break;
                
                /* 1111 1011 P1.2口对应按键按下 K2 设置 */
                case(0xfb):     
                        value[0] = 3;
                        break;
                
                /* 1111 0111 P1.3口对应按键按下 K3 切换/星期 */
                case(0xf7):
                        value[0] = 4;
                        break;
                
                /* 1110 1111 P1.4口对应按键按下 K4 加/闹钟 */
                case(0xef):     
                        value[0] = 5;
                        break;
                        
                /* 1101 1111 P1.5口对应按键按下 K5 减/温度 */
                case(0xdf):
                        value[0] = 6;
                        break;
        }
}
