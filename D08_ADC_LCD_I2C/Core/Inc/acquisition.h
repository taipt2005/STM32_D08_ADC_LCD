#ifndef ACQUISITION_H
#define ACQUISITION_H
#include "../Src/Module/adc_sensor.h"
#define ADC_CHANNELS ADC_NUM_CH
#define ADC_FRAMES_HALF ADC_AVERAGE_FRAMES
HAL_StatusTypeDef acquisition_start(ADC_HandleTypeDef *adc, TIM_HandleTypeDef *timer);
/* Latest complete 10-frame block; out order POT,LDR,VBAT,TEMP,VREFINT */
int acquisition_get(float out[ADC_CHANNELS], uint32_t *frames, uint32_t *skipped);
extern volatile uint32_t acquisition_errors;
#endif
