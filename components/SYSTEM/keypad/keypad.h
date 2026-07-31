#ifndef __KEYPAD_H__
#define __KEYPAD_H__

#include<stdint.h>
#include<stdio.h>

#include "nvs_flash.h"

#include"freertos/FreeRTOS.h"
#include"freertos/task.h"
#include"freertos/timers.h"
#include"freertos/semphr.h"

#include"esp_log.h"
#include"esp_netif.h"
#include"esp_event.h"
#include"esp_attr.h"
#include"esp_rom_sys.h"
#include"esp_adc/adc_oneshot.h"
#include"esp_wifi.h"

#include"myi2c.h"
#include"myuart.h"
#include"myadc.h"
#include"myspi.h"
#include"mygptim.h"
#include"mywifiap.h"
#include"adc_e.h"

#include"driver/gpio.h"
#include"driver/uart.h"
#include"driver/i2c_master.h"
#include"driver/spi_master.h"
#include"driver/gptimer.h"

#include"key.h"
#include"led.h"


/* ---------- 启用外设宏 ---------- */
//定时器
#define TIM1    1
#define TIM2    0  
#define TIM3    0
#define TIM4    0

//显示
#define OLED              1   // 0.96OLED
#define LCD144            1   // 1.44TFT

//UART
#define AS608             1   // 指纹传感器
#define JW01              1   // 二氧化碳传感器

//IIC
#define BH1750            1   // 光照传感器
#define ADXL345           1   // 姿态传感器


//SPI
#define RC522             1   // 刷卡模块

//ADC
#define MQ                0   // MQ系列
#define NOISE             0    // 分贝
#define LIGHT             0    // 光线
#define PM                0    // PM2.5
#define RAIN              0    // 雨量
#define WIND_SPEED        0    // 风速
#define WIND_DIRECTION    0    // 风向
#define WATER_LEVEL       0    // 水位
#define SOIL_HUM          0    // 土壤湿度
#define PH                0    // 酸碱度


//其他模块
#define DHT11             0   // 温湿度传感器
#define SG90              1   // 舵机



#if OLED
    #include"oled.h"
#endif
#if LCD144
    #include"lcd144.h"
#endif
#if DHT11
    #include"dht11.h"
#endif
#if SG90
    #include"sg90.h"
#endif
#if BH1750
    #include"bh1750.h"
#endif
#if ADXL345
    #include"adxl345.h"
#endif
#if RC522
    #include"rc522.h"
#endif
#if AS608
    #include"as608.h"
#endif
#if JW01
    #include"jw01.h"
#endif


void app_init(void);
void KeyScanHandle(void);
void Display(void);
void DISplaysetvalue(void);
void CotorFun(void);
uint8_t AS608_Add_finger(uint16_t page_id);

extern SemaphoreHandle_t client_mutex;
extern char display[32];
extern uint8_t keyNum[5], setn ;


extern uint8_t temperature,hum,smoke,light,noise,rain;
extern uint16_t CO2,lux;
extern uint8_t tempMax,humMax;
extern uint8_t angle;
extern uint8_t finger_Register, finger_Delete;

#endif


