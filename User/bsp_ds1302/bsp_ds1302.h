#ifndef __BSP_DS1302_H
#define __BSP_DS1302_H

#include "main.h"

#define DS1302_SEC      0x80
#define DS1302_MIN      0x82
#define DS1302_HOUR     0x84
#define DS1302_DATE     0x86
#define DS1302_MONTH    0x88
#define DS1302_WEEK     0x8a
#define DS1302_YEAR     0x8c
#define DS1302_CONTROL  0x8e

u8 ds1302_read_register(u8 addr);
void ds1302_write_register(u8 addr, u8 Date);

#endif /* __BSP_DS1302_H */