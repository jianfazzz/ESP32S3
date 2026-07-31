#ifndef __MYWIFIAP_H_
#define __MYWIFIAP_H_

#include <stdint.h>

// Wi-Fi 配置
#define WIFI_SSID      "ESP32S3_WIFI"
#define WIFI_PASSWORD  "12345678"
#define TCP_PORT       8080
#define MAX_CLIENTS    10

// 定时上报周期（单位：ms）
#define REPORT_PERIOD_MS  500

// 函数声明
void WIFI_AP_init(void);
void tcp_server_task(void *pvParameters);
void report_task(void *pvParameters);
uint16_t ReceiveData_(const char *dataPtr, const char *name);

#endif