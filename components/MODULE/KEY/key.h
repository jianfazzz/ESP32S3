#ifndef __KEY_H_
#define __KEY_H_

#include <stdint.h>

#define KEY1 15
#define KEY2 16
#define KEY3 17
#define KEY4 18

#define KEY1_LEVEL gpio_get_level(KEY1)
#define KEY2_LEVEL gpio_get_level(KEY2)
#define KEY3_LEVEL gpio_get_level(KEY3)
#define KEY4_LEVEL gpio_get_level(KEY4)

void KEY_init(void);
void KEY_scan(uint8_t keyNum[5]);


#endif

