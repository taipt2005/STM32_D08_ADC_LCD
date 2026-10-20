#include "adc_sensor.h"

static volatile uint16_t adc_buf[ADC_NUM_CH];    /* DMA circular */
static uint16_t latest[ADC_NUM_CH];              /* Ban chup on dinh cua vong quet gan nhat */
static volatile uint8_t  new_data;
static volatile uint32_t sample_count;
static uint32_t running_sum[ADC_NUM_CH];
static unsigned average_count;
static volatile uint32_t average_sum[ADC_NUM_CH], average_frames, skipped_blocks;
static volatile uint8_t average_ready;
static ADC_HandleTypeDef *source;
volatile uint32_t adc_sensor_errors;

HAL_StatusTypeDef ADC_Sensor_Init(ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim3)
{
    if (hadc->Init.NbrOfConversion != ADC_NUM_CH) return HAL_ERROR;   /* sai CubeMX */
 
    source = hadc;
    new_data = 0;
    sample_count = 0;
    average_count = 0;
    average_ready = 0;
    average_frames = skipped_blocks = adc_sensor_errors = 0;
    for (unsigned ch = 0; ch < ADC_NUM_CH; ++ch) {
        latest[ch] = 0;
        running_sum[ch] = average_sum[ch] = 0;
    }
 
    if (HAL_ADCEx_Calibration_Start(hadc) != HAL_OK) return HAL_ERROR;
 
    HAL_Delay(2); /* Internal sensor settling, startup only. */

    if (HAL_ADC_Start_DMA(hadc, (uint32_t *)adc_buf, ADC_NUM_CH) != HAL_OK) return HAL_ERROR;
 
    if (HAL_TIM_Base_Start(htim3) != HAL_OK) {
        (void)HAL_ADC_Stop_DMA(hadc);
        return HAL_ERROR;
    }
    return HAL_OK;
}

/* DMA chup xong 1 vong quet 5 kenh (moi lan Timer kich)*/
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc != source) return;

    for (int ch = 0; ch < ADC_NUM_CH; ch++) {
        latest[ch] = adc_buf[ch];
        running_sum[ch] += latest[ch];
    }
    sample_count++;
    new_data = 1;

    /* Extension: keep averaging in the callback so LCD work loses no scans. */
    if (++average_count == ADC_AVERAGE_FRAMES) {
        if (average_ready) ++skipped_blocks;
        for (unsigned ch = 0; ch < ADC_NUM_CH; ++ch) {
            average_sum[ch] = running_sum[ch];
            running_sum[ch] = 0;
        }
        average_frames = sample_count;
        average_count = 0;
        average_ready = 1;
    }
}

uint8_t ADC_Sensor_IsNewData(void)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    uint8_t r = new_data;
    new_data = 0;
    __set_PRIMASK(primask);
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

uint8_t ADC_Sensor_GetAverage(float out[ADC_NUM_CH],
                             uint32_t *frames, uint32_t *skipped)
{
    uint32_t local[ADC_NUM_CH];
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    if (!average_ready) {
        __set_PRIMASK(primask);
        return 0;
    }
    for (unsigned ch = 0; ch < ADC_NUM_CH; ++ch)
        local[ch] = average_sum[ch];
    *frames = average_frames;
    *skipped = skipped_blocks;
    average_ready = 0;
    __set_PRIMASK(primask);
    for (unsigned ch = 0; ch < ADC_NUM_CH; ++ch)
        out[ch] = (float)local[ch] / ADC_AVERAGE_FRAMES;
    return 1;
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc == source) ++adc_sensor_errors;
}
