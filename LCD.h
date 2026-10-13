#ifndef LCD_I2C_H
#define LCD_I2C_H
#include "stm32f1xx_hal.h"
HAL_StatusTypeDef lcd_init(I2C_HandleTypeDef *bus);
/* Rows padded to exactly 16 positions; no clear/flicker per refresh. */
HAL_StatusTypeDef lcd_rows(const char *a,const char *b);
#endif