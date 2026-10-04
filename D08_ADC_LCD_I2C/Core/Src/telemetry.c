#include "telemetry.h"
#include <stdio.h>
static UART_HandleTypeDef *port;
static char buffer[220];
void telemetry_init(UART_HandleTypeDef *uart) {port=uart;}
void telemetry_send(const Measurements *m,uint32_t now) {
    /* Do not modify buffer while interrupt-driven TX still owns it. */
    if(port->gState!=HAL_UART_STATE_READY)return;
    int n=snprintf(buffer,sizeof buffer,
        "%lu,%lu,%lu,%ld,%ld,%ld,%ld,%ld,%u,%u,%u,%u,%u\r\n",
        (unsigned long)now,(unsigned long)m->frames,(unsigned long)m->skipped,
        (long)(m->pot_v*1000),(long)(m->light_pct*10),
        (long)(m->temp_c*100),(long)(m->battery_v*1000),(long)(m->vdda*1000),
        (unsigned)m->raw[0],(unsigned)m->raw[1],(unsigned)m->raw[2],
        (unsigned)m->raw[3],(unsigned)m->raw[4]);
    if(n>0 && n<(int)sizeof buffer)
        (void)HAL_UART_Transmit_IT(port,(uint8_t *)buffer,(uint16_t)n);
}
