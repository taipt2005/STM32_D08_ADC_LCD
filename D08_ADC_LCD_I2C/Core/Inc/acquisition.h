#ifndef ACQUISITION_H
#define ACQUISITION_H
#include "stm32f1xx_hal.h"
#define ADC_CHANNELS 5U
#define ADC_FRAMES_HALF 10U
HAL_StatusTypeDef acquisition_start(ADC_HandleTypeDef *adc, TIM_HandleTypeDef *timer);
/* Latest complete 10-frame block; out order POT,LDR,TEMP,BAT,VREFINT */
int acquisition_get(float out[ADC_CHANNELS], uint32_t *frames, uint32_t *skipped);
extern volatile uint32_t acquisition_errors;
#endif
