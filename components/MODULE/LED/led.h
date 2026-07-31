#ifndef __LED_H_
#define __LED_H_

#include <stdint.h>

#define BEEP   38
#define RELAY1 39


void GPIO_init(void);


#define BEEP_DEVICE(x)  gpio_set_level(BEEP, (x))
#define RELAY1_DEVICE(x)  gpio_set_level(RELAY1, (x))


#endif

