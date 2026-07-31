#ifndef __MYIIC_H_
#define __MYIIC_H_

#include "driver/i2c_master.h"


#define SCL 20      
#define SDA 19


extern i2c_master_dev_handle_t oled_dev_handle;
extern i2c_master_dev_handle_t bh1750_dev_handle;
extern i2c_master_dev_handle_t adxl345_dev_handle;

void IIC_init(void);
void I2C_Scan(void);
void I2C_BusReset(void);


#endif

