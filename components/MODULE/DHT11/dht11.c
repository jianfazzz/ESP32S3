#include"dht11.h"
#include "keypad.h"

/* -------------------- 底层操作 -------------------- */
static inline void pin_out(uint8_t level)
{
    gpio_set_level(DHT11_PIN, level);
}

static inline int pin_in(void)
{
    return gpio_get_level(DHT11_PIN);
}

/* -------------------- 初始化 -------------------- */
void DHT11_init(void)
{
    gpio_config_t io_conf = {
        .intr_type    = GPIO_INTR_DISABLE,
        .mode         = GPIO_MODE_INPUT_OUTPUT_OD,    
        .pin_bit_mask = (1ULL << DHT11_PIN),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en   = GPIO_PULLUP_ENABLE,    
    };
    gpio_config(&io_conf);
    gpio_set_level(DHT11_PIN, 1);

}

/* -------------------- 起始信号 + 应答 -------------------- */
static int dht11_start(void)
{
    int retry;

    pin_out(0);
    vTaskDelay(pdMS_TO_TICKS(20));

    pin_out(1);
    esp_rom_delay_us(25);

    retry = 0;
    while (pin_in() == 1 && retry < 500) {
        retry++;
        esp_rom_delay_us(1);
    }
    if (retry >= 500) {
        // printf("[DHT11] 应答低电平超时\n");
        return 1;
    }

    retry = 0;
    while (pin_in() == 0 && retry < 1000) {
        retry++;
        esp_rom_delay_us(1);
    }
    if (retry >= 1000) {
        // printf("[DHT11] 应答高电平超时（总线持续为低，等待了 %d us）\n", retry);
        return 2;
    }

    retry = 0;
    while (pin_in() == 1 && retry < 1000) {
        retry++;
        esp_rom_delay_us(1);
    }
    if (retry >= 1000) {
        // printf("[DHT11] 应答高电平结束超时\n");
        return 3;
    }

    return 0;
}

/* -------------------- 读位 -------------------- */
static uint8_t dht11_read_bit(void)
{
    int retry = 0;

    while (pin_in() == 0 && retry < 200) {
        retry++;
        esp_rom_delay_us(1);
    }

    esp_rom_delay_us(40);
    uint8_t bit = (pin_in() == 1) ? 1 : 0;

    retry = 0;
    while (pin_in() == 1 && retry < 200) {
        retry++;
        esp_rom_delay_us(1);
    }
    return bit;
}

static uint8_t dht11_read_byte(void)
{
    uint8_t data = 0;
    for (int i = 0; i < 8; i++) {
        data <<= 1;
        data |= dht11_read_bit();
    }
    return data;
}

int DHT11_Read(uint8_t *temp, uint8_t *humi)
{
    if (temp == NULL || humi == NULL) return -1;

    int ret = dht11_start();
    if (ret != 0) return ret;

    uint8_t buf[5];
    for (int i = 0; i < 5; i++) buf[i] = dht11_read_byte();

    if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4]) {
        *humi = buf[0];
        *temp = buf[2];
        return 0;
    }
    // printf("[DHT11] 校验失败\n");
    return 4;
}

// void TEST(void){
//     uint8_t temperature,hum;
//     DHT11_init();
//     DHT11_Read(&temperature,&hum); 
// }
