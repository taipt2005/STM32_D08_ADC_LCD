#include "adc_sensor.h"

static volatile uint16_t adc_buf[ADC_NUM_CH];    /* DMA circular */
static uint16_t latest[ADC_NUM_CH];              /* Ban chup on dinh cua vong quet gan nhat */
static volatile uint8_t  new_data;
static volatile uint32_t sample_count;

HAL_StatusTypeDef ADC_Sensor_Init(ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim3)
{
    if (hadc->Init.NbrOfConversion != ADC_NUM_CH) return HAL_ERROR;   /* sai CubeMX */
 
    new_data = 0;
    sample_count = 0;
 
    if (HAL_ADCEx_Calibration_Start(hadc) != HAL_OK) return HAL_ERROR;
 
    if (HAL_ADC_Start_DMA(hadc, (uint32_t *)adc_buf, ADC_NUM_CH) != HAL_OK) return HAL_ERROR;
 
    return HAL_TIM_Base_Start(htim3);
}

/* DMA chup xong 1 vong quet 5 kenh (moi lan Timer kich)*/
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance != ADC1) return;

    for (int ch = 0; ch < ADC_NUM_CH; ch++) latest[ch] = adc_buf[ch];
    sample_count++;
    new_data = 1;
}

uint8_t ADC_Sensor_IsNewData(void)
{
    uint8_t r = new_data;
    new_data = 0;
    return r;
}

void ADC_Sensor_GetRaw(uint16_t out[ADC_NUM_CH])
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();                  
    for (int ch = 0; ch < ADC_NUM_CH; ch++) out[ch] = latest[ch];
    if (!primask) __enable_irq();
}

uint32_t ADC_Sensor_GetSampleCount(void)
{
    return sample_count;
}