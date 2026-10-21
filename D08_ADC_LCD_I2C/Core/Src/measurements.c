#include "measurements.h"
#include "d08_config.h"
static float clamp_pct(float v) {return v<0?0:(v>100?100:v);}
static void update_alarms(Measurements *m)
{
    if (m->pot_v > D08_POT_HIGH_V)
        m->alarms |= D08_ALARM_POT;
    else if (m->pot_v <= D08_POT_HIGH_V - D08_POT_HYST_V)
        m->alarms &= (uint8_t)~D08_ALARM_POT;

    if (m->light_pct < D08_LIGHT_LOW_PCT)
        m->alarms |= D08_ALARM_LIGHT;
    else if (m->light_pct >= D08_LIGHT_LOW_PCT + D08_LIGHT_HYST_PCT)
        m->alarms &= (uint8_t)~D08_ALARM_LIGHT;

    if (m->temp_c >= D08_TEMP_HIGH_C)
        m->alarms |= D08_ALARM_TEMP;
    else if (m->temp_c <= D08_TEMP_HIGH_C - D08_TEMP_HYST_C)
        m->alarms &= (uint8_t)~D08_ALARM_TEMP;

    if (m->battery_v < D08_BAT_LOW_V)
        m->alarms |= D08_ALARM_BAT;
    else if (m->battery_v >= D08_BAT_LOW_V + D08_BAT_HYST_V)
        m->alarms &= (uint8_t)~D08_ALARM_BAT;
}

int measurements_convert(Measurements *m,const float raw[5]) {
    if(raw[ADC_IDX_VREF]<1 || D08_LDR_BRIGHT<=D08_LDR_DARK) return 0;
    for(unsigned i=0;i<5;++i)m->raw[i]=raw[i];
    m->vdda=D08_VREFINT_V*4095.0f/raw[ADC_IDX_VREF];
    m->pot_v=raw[ADC_IDX_POT]*m->vdda/4095.0f;
    m->pot_pct=clamp_pct(raw[ADC_IDX_POT]*100.0f/4095.0f);
    m->light_pct=clamp_pct((raw[ADC_IDX_LDR]-D08_LDR_DARK)*100.0f/(D08_LDR_BRIGHT-D08_LDR_DARK));
    m->temp_c=(D08_TEMP_V25-raw[ADC_IDX_TEMP]*m->vdda/4095.0f)/D08_TEMP_SLOPE+25.0f+D08_TEMP_OFFSET;
    m->battery_v=raw[ADC_IDX_VBAT]*m->vdda/4095.0f*(D08_R_TOP+D08_R_BOTTOM)/D08_R_BOTTOM;
    update_alarms(m);
    return 1;
}
