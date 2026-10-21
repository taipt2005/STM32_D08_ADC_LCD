#ifndef D08_CONFIG_H
#define D08_CONFIG_H
/* PCF8574: P0=RS P1=RW P2=EN P3=BL P4..7=D4..7 */
#define LCD_ADDR_7BIT 0x27U
#define D08_AUTO_PAGE 0 /* 0: button only; 1: automatic every 2 s */
#define D08_VREFINT_V 1.200f /* Replace with measured calibration value */
#define D08_R_TOP  47000.0f
#define D08_R_BOTTOM 5100.0f
#define D08_TEMP_V25 1.430f
#define D08_TEMP_SLOPE 0.0043f
#define D08_TEMP_OFFSET 0.0f
#define D08_LDR_DARK 0.0f /* averaged raw ADC at dark calibration */
#define D08_LDR_BRIGHT 4095.0f /* averaged raw ADC at bright calibration */
/* Alarm thresholds: tune these values for the demo. */
#define D08_POT_HIGH_V       3.0f
#define D08_LIGHT_LOW_PCT   20.0f
#define D08_TEMP_HIGH_C     30.0f
#define D08_BAT_LOW_V       10.0f

/* Hysteresis: clear only after the reading has recovered. */
#define D08_POT_HYST_V       0.10f
#define D08_LIGHT_HYST_PCT   5.0f
#define D08_TEMP_HYST_C      2.0f
#define D08_BAT_HYST_V       0.30f
#endif
