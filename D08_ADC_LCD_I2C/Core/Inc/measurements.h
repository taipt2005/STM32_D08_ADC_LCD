#ifndef MEASUREMENTS_H
#define MEASUREMENTS_H
#include <stdint.h>
#include "../Src/Module/adc_sensor.h"
/* Alarm bit mask; several channels may be active at the same time. */
#define D08_ALARM_POT     (1U << 0)
#define D08_ALARM_LIGHT   (1U << 1)
#define D08_ALARM_TEMP    (1U << 2)
#define D08_ALARM_BAT     (1U << 3)

typedef struct {
    float raw[ADC_NUM_CH], vdda, pot_v, pot_pct, light_pct, temp_c, battery_v;
    uint32_t frames, skipped;
    uint8_t alarms; /* Zero-initialize before the first conversion. */
} Measurements;
int measurements_convert(Measurements *m,const float raw[ADC_NUM_CH]);
#endif
