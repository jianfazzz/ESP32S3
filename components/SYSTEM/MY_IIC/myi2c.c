#include"myi2c.h"
#include "keypad.h"

i2c_master_bus_handle_t bus_handle;
i2c_master_dev_handle_t oled_dev_handle,bh1750_dev_handle,adxl345_dev_handle;

void IIC_init(void) {
    // 配置 I2C 总线
    i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_NUM_0,                // 使用 I2C0
        .sda_io_num = SDA,
        .scl_io_num = SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,    // 默认时钟源
        .glitch_ignore_cnt = 7,
        .intr_priority = 0,
        .trans_queue_depth = 0,               // 同步模式，无需队列
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &bus_handle));

    #if OLED
         // 添加 OLED 设备
        i2c_device_config_t dev_config = {
            .dev_addr_length = I2C_ADDR_BIT_LEN_7,
            .device_address = OLED_ADDRESS,       // OLED 地址
            .scl_speed_hz = 400000,               // 400kHz
            .scl_wait_us = 0,                     // 使用默认超时
            .flags.disable_ack_check = false,
        };
        i2c_master_bus_add_device(bus_handle, &dev_config, &oled_dev_handle);
    #endif
    #if BH1750
        // 添加设备
        i2c_device_config_t bh1750_dev_config = {
            .dev_addr_length = I2C_ADDR_BIT_LEN_7,
            .device_address = BH1750_ADDRESS,       //BH1750地址
            .scl_speed_hz = 100000,               // 100kHz
            .scl_wait_us = 0,                     // 使用默认超时
            .flags.disable_ack_check = false,
        };
        i2c_master_bus_add_device(bus_handle, &bh1750_dev_config, &bh1750_dev_handle);
    #endif
    #if ADXL345
        i2c_device_config_t adxl345_dev_config = {
            .dev_addr_length = I2C_ADDR_BIT_LEN_7,
            .device_address = ADXL345_ADDRESS,       //ADXL345地址
            .scl_speed_hz = 100000,               // 100kHz
            .scl_wait_us = 0,                     // 使用默认超时
            .flags.disable_ack_check = false,
        };
        i2c_master_bus_add_device(bus_handle, &adxl345_dev_config, &adxl345_dev_handle);
    #endif
}


// 扫描 bus2 上的所有 I2C 设备地址, 用于排查地址问题
void I2C_Scan(void)
{
    ESP_LOGI("I2C", "scanning bus (SCL=%d SDA=%d)...", SCL, SDA);
    uint8_t found = 0;
    for (uint16_t addr = 1; addr < 0x78; addr++) {
        esp_err_t ret = i2c_master_probe(bus_handle, addr, 100);
        if (ret == ESP_OK) {
            ESP_LOGI("I2C", "  device found @ 0x%02X", addr);
            found++;
        }
    }
    ESP_LOGI("I2C", "scan done, %u device(s) found", found);
}

// I2C 总线软件复位: 发送 9 个 SCL 时钟释放被拉低的 SDA
void I2C_BusReset(void)
{
    ESP_LOGW("I2C", "bus reset on SCL=%d SDA=%d", SCL, SDA);
    gpio_set_direction(SCL, GPIO_MODE_OUTPUT);
    gpio_set_direction(SDA, GPIO_MODE_OUTPUT);
    gpio_set_level(SDA, 1);
    vTaskDelay(pdMS_TO_TICKS(1));
    for (int i = 0; i < 9; i++) {
        gpio_set_level(SCL, 1);
        vTaskDelay(pdMS_TO_TICKS(1));
        gpio_set_level(SCL, 0);
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    gpio_set_level(SDA, 1);
    vTaskDelay(pdMS_TO_TICKS(1));
    gpio_set_level(SCL, 1);
    vTaskDelay(pdMS_TO_TICKS(1));
}


