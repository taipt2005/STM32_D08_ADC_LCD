#ifndef ADC_DMA_H
#define ADC_DMA_H

#include "main.h"
#include <stdbool.h>

#define ADC_CHANNELS_NUM            4

/* System functions */
void ADC_DMA_Init(void);
void ADC_DMA_Stop(void);
bool ADC_DMA_IsDataReady(void);
void ADC_DMA_GetRawData(uint16_t *pDest);
uint32_t ADC_DMA_GetSampleCount(void);

#endif /* ADC_DMA_H */
