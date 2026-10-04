#include "acquisition.h"
static ADC_HandleTypeDef *source;
static uint16_t dma_data[2U * ADC_FRAMES_HALF * ADC_CHANNELS];
static volatile uint32_t sums[ADC_CHANNELS], frame_count, skipped_count;
static volatile uint8_t ready;
volatile uint32_t acquisition_errors;
static void publish(unsigned offset) {
    uint32_t local[ADC_CHANNELS] = {0};
    for (unsigned f=0; f<ADC_FRAMES_HALF; ++f)
        for (unsigned c=0; c<ADC_CHANNELS; ++c)
            local[c] += dma_data[offset + f*ADC_CHANNELS + c];
    if (ready) ++skipped_count;
    for (unsigned c=0; c<ADC_CHANNELS; ++c) sums[c]=local[c];
    frame_count += ADC_FRAMES_HALF;
    ready=1;
}
HAL_StatusTypeDef acquisition_start(ADC_HandleTypeDef *adc, TIM_HandleTypeDef *timer) {
    source=adc;
    if (HAL_ADCEx_Calibration_Start(adc)!=HAL_OK) return HAL_ERROR;
    /* Internal sensor startup settling; startup only. */
    HAL_Delay(2);
    if (HAL_ADC_Start_DMA(adc,(uint32_t *)dma_data,
        2U*ADC_FRAMES_HALF*ADC_CHANNELS)!=HAL_OK) return HAL_ERROR;
    return HAL_TIM_Base_Start(timer);
}
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *adc) {
    if (adc==source) publish(0);
}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *adc) {
    if (adc==source) publish(ADC_FRAMES_HALF*ADC_CHANNELS);
}
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *adc) {
    if (adc==source) ++acquisition_errors;
}
int acquisition_get(float out[ADC_CHANNELS],uint32_t *frames,uint32_t *skipped) {
    uint32_t temp[ADC_CHANNELS];
    uint32_t mask=__get_PRIMASK();
    __disable_irq();
    if (!ready) { __set_PRIMASK(mask); return 0; }
    for(unsigned c=0;c<ADC_CHANNELS;++c) temp[c]=sums[c];
    *frames=frame_count; *skipped=skipped_count; ready=0;
    __set_PRIMASK(mask);
    for(unsigned c=0;c<ADC_CHANNELS;++c) out[c]=(float)temp[c]/ADC_FRAMES_HALF;
    return 1;
}
