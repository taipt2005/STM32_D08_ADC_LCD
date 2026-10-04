#include "app.h"

#include "acquisition.h"

#include "measurements.h"

#include "button.h"

#include "lcd_i2c.h"

#include "telemetry.h"

#include "d08_config.h"

#include <stdio.h>

extern ADC_HandleTypeDef hadc1;

extern TIM_HandleTypeDef htim3;

extern I2C_HandleTypeDef hi2c1;

extern UART_HandleTypeDef huart1;

extern void Error_Handler(void);

static Measurements m;

volatile Measurements d08_watch;

static uint8_t page,valid,lcd_ok;

static uint32_t last_lcd,last_page;

/* Debug Watch variables */

volatile uint32_t d08_lcd_errors;

volatile uint8_t d08_page;

static void render(void) {

    char a[40],b[40];

    unsigned pv=(unsigned)(m.pot_v*1000),bv=(unsigned)(m.battery_v*1000);

    int tc=(int)(m.temp_c*10);

    unsigned at=(unsigned)(tc<0?-tc:tc);

    switch(page) {

    case 0:

        snprintf(a,sizeof a,"1.BIEN TRO %3u%%",(unsigned)m.pot_pct);

        snprintf(b,sizeof b,"U=%u.%03uV",pv/1000,pv%1000);break;

    case 1:

        snprintf(a,sizeof a,"2.ANH SANG");

        snprintf(b,sizeof b,"Tuong doi:%3u%%",(unsigned)m.light_pct);break;

    case 2:

        snprintf(a,sizeof a,"3.NHIET DO CHIP");

        snprintf(b,sizeof b,"T=%s%u.%u C",tc<0?"-":"",at/10,at%10);break;

    case 3:

        snprintf(a,sizeof a,"4.DIEN AP PIN");

        snprintf(b,sizeof b,"U=%u.%03uV",bv/1000,bv%1000);break;

    default:

        snprintf(a,sizeof a,"P%u.%02uV L%3u%%",pv/1000,(pv%1000)/10,(unsigned)m.light_pct);

        snprintf(b,sizeof b,"T%s%u.%u B%u.%02u",tc<0?"-":"",at/10,at%10,bv/1000,(bv%1000)/10);break;

    }

    if(lcd_rows(a,b)!=HAL_OK){++d08_lcd_errors;lcd_ok=0;}

}

void app_init(void) {

    button_init();telemetry_init(&huart1);

    lcd_ok=(lcd_init(&hi2c1)==HAL_OK);

    if(!lcd_ok)++d08_lcd_errors;

    else if(lcd_rows("D08 ADC 4 KENH","Dang lay mau...")!=HAL_OK)lcd_ok=0;

    if(acquisition_start(&hadc1,&htim3)!=HAL_OK)Error_Handler();

    last_page=last_lcd=HAL_GetTick();

}

void app_task(void) {

    uint32_t now=HAL_GetTick();float raw[5];

    if(acquisition_get(raw,&m.frames,&m.skipped)) {

        valid=(uint8_t)measurements_convert(&m,raw);

        if(valid) { d08_watch=m; telemetry_send(&m,now); }

    }

    int advance=button_pressed(now);

    if(D08_AUTO_PAGE && (uint32_t)(now-last_page)>=2000)advance=1;

    if(advance){page=(page+1)%5;d08_page=page;last_page=now;last_lcd=now-200;}

    if(valid && lcd_ok && (uint32_t)(now-last_lcd)>=200) {

        last_lcd=now;render();

    }

}