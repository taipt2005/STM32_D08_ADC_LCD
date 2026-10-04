#ifndef MEASUREMENTS_H
#define MEASUREMENTS_H
#include <stdint.h>
typedef struct {
    float raw[5], vdda, pot_v, pot_pct, light_pct, temp_c, battery_v;
    uint32_t frames, skipped;
} Measurements;
int measurements_convert(Measurements *m,const float raw[5]);
#endif
