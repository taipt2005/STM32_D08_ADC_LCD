#include "acquisition.h"

/* Adapter for the existing application; the ADC driver belongs to SV1. */
volatile uint32_t acquisition_errors;
HAL_StatusTypeDef acquisition_start(ADC_HandleTypeDef *adc, TIM_HandleTypeDef *timer)
{
    acquisition_errors = 0;
    return ADC_Sensor_Init(adc, timer);
}

int acquisition_get(float out[ADC_CHANNELS], uint32_t *frames, uint32_t *skipped)
{
    acquisition_errors = adc_sensor_errors;
    return ADC_Sensor_GetAverage(out, frames, skipped);
}
