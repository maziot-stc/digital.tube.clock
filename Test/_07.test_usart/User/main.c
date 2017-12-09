#include <reg52.h>
#include "main.h"
#include "bsp_delay.h"

sbit led_control = P1^4;
sbit mcu_select = P1^0;
sbit key_input = P1^3;

int main()
{
        char led_on = 'o';                              // open 
        char led_off = 'c';                             // close
        char data_select = led_on;                      // 发送机发送数据选择信号
        char control_signal = ' ';                      // 接收机接收到的控制信号
        
        TMOD = 0x20;
        TH1 = 0xfd; 
        TL1 = 0xfd; 
        PCON = 0x00;
        
        if(mcu_select == 1)                             // 选择位为高电平为发送机
        {
                SCON = 0x40;
                TI = 0;
                TR1 = 1;
                while(1)
                {
                        if(key_input == 0)
                        {
                                delay_ms(10);
                                if(key_input == 0)
                                {
                                        if(data_select == led_on)
                                        {
                                                SBUF = led_on;
                                                data_select = led_off;
                                        }
                                        else if(data_select == led_off)
                                        {
                                                SBUF = led_off;
                                                data_select = led_on;
                                        }
                                        
                                        while(key_input == 0);
                                }
                                
                                while(TI == 0);                 // 等待发送完成
                                TI = 0;                         // 清除发送标志
                        }
                        delay_ms(10);
                }
        }
        else                                            // 选择位为低电平为接收机
        {
                SCON = 0x50;
                RI = 0;
                TR1 = 1;
                while(1)
                {
                        if(RI)
                        {
                                control_signal = SBUF;
                                RI = 0;
                                delay_ms(10);
                        }
                        
                        if(control_signal == led_on)
                        {
                                led_control = 0;
                        }
                        else
                        {
                                led_control = 1;
                        }
                }
        }
}