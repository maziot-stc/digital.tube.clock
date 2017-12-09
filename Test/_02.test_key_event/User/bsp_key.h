#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include <reg52.h>
#include "main.h"

#define GPIO_KEY P1

u8 Key_Scan(void);
void Key_Control(u8);

void delay(u32 t);

#endif /* __BSP_KEY_H */