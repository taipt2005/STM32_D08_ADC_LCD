#include "measurements.h"
#include "d08_config.h"
static float clamp_pct(float v) {return v<0?0:(v>100?100:v);}
int measurements_convert(Measurements *m,const float raw[5]) {
    if(raw[4]<1 || D08_LDR_BRIGHT<=D08_LDR_DARK) return 0;
    for(unsigned i=0;i<5;++i)m->raw[i]=raw[i];
    m->vdda=D08_VREFINT_V*4095.0f/raw[4];
    m->pot_v=raw[0]*m->vdda/4095.0f;
    m->pot_pct=clamp_pct(raw[0]*100.0f/4095.0f);
    m->light_pct=clamp_pct((raw[1]-D08_LDR_DARK)*100.0f/(D08_LDR_BRIGHT-D08_LDR_DARK));
    m->temp_c=(D08_TEMP_V25-raw[2]*m->vdda/4095.0f)/D08_TEMP_SLOPE+25.0f+D08_TEMP_OFFSET;
    m->battery_v=raw[3]*m->vdda/4095.0f*(D08_R_TOP+D08_R_BOTTOM)/D08_R_BOTTOM;
    return 1;
}
