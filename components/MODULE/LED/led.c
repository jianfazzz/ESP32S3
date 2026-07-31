#include"led.h"
#include "keypad.h"


void GPIO_init(void){
    gpio_config_t GPIO_CONFIG={
        .intr_type = GPIO_INTR_DISABLE ,
        .mode =GPIO_MODE_INPUT_OUTPUT ,
        .pin_bit_mask = (1ull << BEEP | 1ull << RELAY1 ),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    gpio_config(&GPIO_CONFIG);
}

