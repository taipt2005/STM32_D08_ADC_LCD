/**
 * @file    adc_sensor.h
 * @brief   SV1 - ADC scan 5 kenh, kich bang Timer (tan so co dinh), doc bang DMA.
 *
 * Thu tu kenh (= Rank trong CubeMX, phai khop):
 *   0 POT (PA0, IN0)   1 LDR (PA1, IN1)   2 VBAT (PA2, IN2, qua cau phan ap)
 *   3 TEMP (IN16, nhiet noi noi bo)       4 VREF (IN17, Vrefint)
 *
 * Luong: TIM3 TRGO -> ADC1 (scan 5 kenh, 1 vong/lan kich) -> DMA circular -> RAM
 *
 * TIM3 chi dung rieng de kich ADC, duoc tich hop tu bat luon Timer trong ADC_Sensor_Init()
 */
 
#ifndef ADC_SENSOR_H
#define ADC_SENSOR_H

#include "stm32f1xx_hal.h"

#define ADC_NUM_CH       5
#define ADC_IDX_POT      0
#define ADC_IDX_LDR      1
#define ADC_IDX_VBAT     2
#define ADC_IDX_TEMP     3
#define ADC_IDX_VREF     4   /* Vrefint - dung de tu can chinh Vdda */

/** Hieu chuan ADC, chay ADC+DMA, roi bat TIM3 de bat dau kich ADC dinh ky.
 *  @return HAL_OK; HAL_ERROR neu CubeMX cau hinh sai so kenh (khac 5) */
HAL_StatusTypeDef ADC_Sensor_Init(ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim3);

/** 1 neu co mau moi tinh tu lan goi truoc (tu xoa co khi doc) */
uint8_t ADC_Sensor_IsNewData(void);

/** Doc ban chup raw moi nhat cua 5 kenh (0..4095) */
void ADC_Sensor_GetRaw(uint16_t out[ADC_NUM_CH]);

/** Tong so vong quet da lay tu luc Init*/
uint32_t ADC_Sensor_GetSampleCount(void);

#endif /* ADC_SENSOR_H */