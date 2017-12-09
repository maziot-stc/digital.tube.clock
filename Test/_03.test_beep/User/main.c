#include <reg52.h>
#include "main.h"
#include "bsp_delay.h"

sbit beep = P3^6;

int main()
{
        while(1)
        {
                beep = 1;
                delay_ms(1000);
                beep = 0;
                delay_ms(1000);
        }

}