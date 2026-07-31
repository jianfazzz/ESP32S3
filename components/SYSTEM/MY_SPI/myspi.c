#include"myspi.h"
#include "keypad.h"
#include "rc522.h"

spi_device_handle_t lcd144_dev_handle,rc522_dev_handle;

static const char *TAG = "SPI";

void SPI_init(void){
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = LCD_MOSI,
        .miso_io_num = LCD_MISO,
        .sclk_io_num = LCD_SCL,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 16384,
        .flags = SPICOMMON_BUSFLAG_MASTER,
    };
    spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO);

    #if LCD144
         // 添加 LCD 设备
        spi_device_interface_config_t lcd144_dev_cfg = {
            .clock_speed_hz = 10 * 1000 * 1000,
            .mode = 0,
            .spics_io_num = LCD_CS,
            .queue_size = 7,
            .pre_cb = NULL,
        };
        spi_bus_add_device(SPI2_HOST, &lcd144_dev_cfg, &lcd144_dev_handle);
    #endif
    #if RC522
         // 添加 RC522 设备
        spi_device_interface_config_t rc522_dev_cfg = {
            .clock_speed_hz = 4 * 1000 * 1000,
            .mode = 0,
            .spics_io_num = RC522_SDA,
            .queue_size = 7,
            .pre_cb = NULL,
        };
        spi_bus_add_device(SPI2_HOST, &rc522_dev_cfg, &rc522_dev_handle);
    #endif
}



