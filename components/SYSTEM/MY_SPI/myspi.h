#ifndef __MYSPI_H_
#define __MYSPI_H_

#include<stdint.h>
#include"driver/spi_master.h"

extern spi_device_handle_t lcd144_dev_handle;
extern spi_device_handle_t rc522_dev_handle;

#define LCD_SCL     1
#define LCD_MOSI    2
#define LCD_MISO    42



void SPI_init(void);




#endif
