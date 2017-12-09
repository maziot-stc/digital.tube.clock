#include "bsp_key.h"
#include "bsp_ds1302.h"
#include <intrins.h>

extern u8 value[4];
extern u8 display_mode;
extern u8 InterruptCount;
extern u8 clock_symbol;
extern u8 point_symbol;

u8 Key_Scan(void)
{
        u8 keyValue = 0xff;
        int i = 0;
        if(GPIO_KEY != 0xff)
        {
                _nop_();
                if(GPIO_KEY != 0xff)
                {
                        keyValue = GPIO_KEY;
                        while((i < 20) && (GPIO_KEY != 0xff))
                        {
                                _nop_();
                                i++;
                        }
                }
        }
        return keyValue;
}

void Key_Control(u8 keyValue)
{
        u8 tmp1 = 0;
        u8 tmp2 = 0;
        
        switch(keyValue)
        {
                /* 1111 1110 P1.0口对应按键按下 K0 左/年 */
                case(0xfe):
                        tmp1 = DS1302_ReadRegister(DS1302_YEAR);
                        value[0] = 2;
                        value[1] = 0;
                        value[2] = (tmp1 & 0xf0) >> 4;
                        value[3] = tmp1 & 0x0f;
                        display_mode = 1;
                        clock_symbol = 0;
                        point_symbol = 0;
                        InterruptCount = 0;
                        TR0 = 1;
                        break;
                
                /* 1111 1101 P1.1口对应按键按下 K1 右/日期 */
                case(0xfd):     
                        tmp1 = DS1302_ReadRegister(DS1302_MONTH);
                        tmp2 = DS1302_ReadRegister(DS1302_DATE);
                        value[0] = (tmp1 & 0xf0) >> 4;
                        value[1] = tmp1 & 0x0f;
                        value[2] = (tmp2 & 0xf0) >> 4;
                        value[3] = tmp2 & 0x0f;
                        display_mode = 1;
                        clock_symbol = 0;
                        point_symbol = 1;
                        InterruptCount = 0;
                        TR0 = 1;
                        break;
                
                /* 1111 1011 P1.2口对应按键按下 K2 设置 */
                case(0xfb):     
                        break;
                
                /* 1111 0111 P1.3口对应按键按下 K3 切换/星期 */
                case(0xf7):
                        tmp1 = DS1302_ReadRegister(DS1302_DAY);
                        value[0] = 16;
                        value[1] = 16;
                        value[2] = tmp1 & 0x07;
                        value[3] = 16;
                        display_mode = 1;
                        clock_symbol = 0;
                        point_symbol = 0;
                        InterruptCount = 0;
                        TR0 = 1;
                        break;
                
                /* 1110 1111 P1.4口对应按键按下 K4 加/闹钟 */
                case(0xef):     
                        break;
                        
                /* 1101 1111 P1.5口对应按键按下 K5 减/温度 */
                case(0xdf):
                        break;
        }
}
