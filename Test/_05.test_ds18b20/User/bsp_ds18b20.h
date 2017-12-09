#ifndef __BSP_DS18B20_H
#define __BSP_DS18B20_H

#include "main.h"
#include "reg52.h"

sbit DQ = P3^7;

void delay(u16 i);
u8 ds18b20_init();
u16 read_temperature();

#endif /* __BSP_DS18B20_H */