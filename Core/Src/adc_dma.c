#include "adc_dma.h"
#include <string.h>

extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim3;

static uint16_t s_adc_dma_buffer[ADC_CHANNELS_NUM];
static uint16_t s_adc_safe_buffer[ADC_CHANNELS_NUM];
static volatile bool s_is_data_ready = false;
static volatile uint32_t s_sample_counter = 0;

/* Initialize ADC calibration, start DMA transfer in circular mode and enable Timer3 TRGO */
void ADC_DMA_Init(void) {
    HAL_ADCEx_Calibration_Start(&hadc1);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)s_adc_dma_buffer, ADC_CHANNELS_NUM);
    HAL_TIM_Base_Start(&htim3);
}

/* Stop Timer3 trigger and disable ADC DMA transfer */
void ADC_DMA_Stop(void) {
    HAL_TIM_Base_Stop(&htim3);
    HAL_ADC_Stop_DMA(&hadc1);
}

/* Check if a new conversion cycle is ready */
bool ADC_DMA_IsDataReady(void) {
    return s_is_data_ready;
}

/* Copy verified 4-channel data to output buffer and clear ready flag */
void ADC_DMA_GetRawData(uint16_t *pDest) {
    if (pDest != NULL) {
        memcpy(pDest, (const void*)s_adc_safe_buffer, sizeof(s_adc_safe_buffer));
        s_is_data_ready = false;
    }
}

/* Return total completed conversion cycles */
uint32_t ADC_DMA_GetSampleCount(void) {
    return s_sample_counter;
}

/* DMA conversion complete interrupt callback: latch data buffer and set ready flag */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    if (hadc->Instance == ADC1) {
        for (int i = 0; i < ADC_CHANNELS_NUM; i++) {
            s_adc_safe_buffer[i] = s_adc_dma_buffer[i];
        }
        s_is_data_ready = true;
        s_sample_counter++;
    }
}
