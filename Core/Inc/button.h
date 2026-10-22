#ifndef BUTTON_H
#define BUTTON_H
#include "stm32f1xx_hal.h"
void button_init(void);
int button_pressed(uint32_t now);
#endif