#include"sg90.h"
#include "keypad.h"
#include "driver/ledc.h"


// LEDC PWM 配置: 50Hz, 14位分辨率
// 20ms 周期 = 16384 级
// 0.5ms(0°) = 410,  2.5ms(180°) = 2048
#define SG90_LEDC_FREQ_HZ     50
#define SG90_LEDC_DUTY_MIN    410    // 0°   对应占空比
#define SG90_LEDC_DUTY_MAX    2048   // 180° 对应占空比

static ledc_channel_config_t sg90_channel = {
    .channel    = LEDC_CHANNEL_0,
    .duty       = SG90_LEDC_DUTY_MIN,
    .gpio_num   = PWM,
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .hpoint     = 0,
    .timer_sel  = LEDC_TIMER_0,
    .flags.output_invert = 0,
};

void SG90_init(void) {
    // 1. 配置 LEDC 定时器: 50Hz, 14位分辨率
    ledc_timer_config_t timer_cfg = {
        .speed_mode      = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_14_BIT,
        .timer_num       = LEDC_TIMER_0,
        .freq_hz         = SG90_LEDC_FREQ_HZ,
        .clk_cfg         = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&timer_cfg);

    // 2. 配置 LEDC 通道
    ledc_channel_config(&sg90_channel);
}

// 角度映射: 0° -> 410, 180° -> 2048 (14位分辨率, 50Hz)
static uint32_t angle_to_duty(uint8_t angle) {
    if (angle > 180) angle = 180;
    return SG90_LEDC_DUTY_MIN + ((uint32_t)angle * (SG90_LEDC_DUTY_MAX - SG90_LEDC_DUTY_MIN)) / 180;
}

void sg90_set_angle(uint8_t angle) {
    uint32_t duty = angle_to_duty(angle);
    ledc_set_duty(sg90_channel.speed_mode, sg90_channel.channel, duty);
    ledc_update_duty(sg90_channel.speed_mode, sg90_channel.channel);
}

// void TEST(void){
//     SG90_init();
//     sg90_set_angle(angle);
// }