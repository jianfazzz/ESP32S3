#ifndef DHT11_H_
#define DHT11_H_

#include <stdint.h>

#define DHT11_PIN   1      // 可根据实际修改

void DHT11_init(void);
int DHT11_Read(uint8_t *temp, uint8_t *humi);

#endif