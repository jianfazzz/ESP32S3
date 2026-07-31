#ifndef __SG90_H_
#define __SG90_H_

#include <stdint.h>

#define PWM   14

extern uint16_t target_ticks;

void SG90_init(void);

void sg90_set_angle(uint8_t angle);

#define SG90_DEVICE(x)  gpio_set_level(PWM, (x))

#endif