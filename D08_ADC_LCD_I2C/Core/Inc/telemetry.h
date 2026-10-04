#ifndef TELEMETRY_H
#define TELEMETRY_H
#include "stm32f1xx_hal.h"
#include "measurements.h"
void telemetry_init(UART_HandleTypeDef *uart);
void telemetry_send(const Measurements *m,uint32_t now);
#endif
