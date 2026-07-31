#include"key.h"
#include "keypad.h"

void KEY_init(void){
    gpio_config_t GPIO_CONFIG={
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << KEY1|1ULL << KEY2|1ULL << KEY3|1ULL << KEY4),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    gpio_config(&GPIO_CONFIG);
}

void KEY_scan(uint8_t keyNum[5])
{
    keyNum[1] = 0;
    keyNum[2] = 0;
    keyNum[3] = 0;
    keyNum[4] = 0;

    if(KEY1_LEVEL == 0) {
    vTaskDelay(20);
        if(KEY1_LEVEL == 0) { 
            keyNum[1] = 1;
        }
    }
    if(KEY2_LEVEL == 0) {
    vTaskDelay(20);
        if(KEY2_LEVEL == 0) { 
            keyNum[2] = 1;
        }
    }
    if(KEY3_LEVEL == 0) {
    vTaskDelay(20);
        if(KEY3_LEVEL == 0) { 
            keyNum[3] = 1;
        }
    }
    if(KEY4_LEVEL == 0) {
    vTaskDelay(20);
        if(KEY4_LEVEL == 0) { 
            keyNum[4] = 1;
        }
    }
}




