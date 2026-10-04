#ifndef APP_H
#define APP_H
#include "measurements.h"
extern volatile Measurements d08_watch;
extern volatile uint32_t d08_lcd_errors;
extern volatile uint8_t d08_page;
void app_init(void);
void app_task(void);
#endif
