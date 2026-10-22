#include "button.h"
static GPIO_PinState previous,stable;
static uint32_t changed;
void button_init(void) {
    previous=stable=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
    changed=HAL_GetTick();
}
int button_pressed(uint32_t now) {
    GPIO_PinState raw=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
    if(raw!=previous) {previous=raw;changed=now;}
    if((uint32_t)(now-changed)>=30 && stable!=raw) {
        stable=raw;
        return stable==GPIO_PIN_RESET;
    }
    return 0;
}