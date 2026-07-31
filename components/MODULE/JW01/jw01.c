#include "jw01.h"
#include "myuart.h"
#include "keypad.h"

#define JW01_UART_PORT  UART_NUM_1

static const char *TAG = "JW01";
static uint16_t co2_value = 0;  // 最近一次解析到的 CO2 值

/* ---------- 初始化 ---------- */
void JW01_Init(void) {
    uart_flush_input(JW01_UART_PORT);
    ESP_LOGI(TAG, "JW01 init done (UART1, baud=%d)", JW01_BAUDRATE);
}

/* ---------- 非阻塞轮询：只处理 UART 缓冲区内已有数据 ---------- */
uint16_t JW01_GetCO2(void) {
    uint8_t buf[JW01_FRAME_LEN];
    int n;

    // 持续从缓冲区读，直到空或找到帧头
    while (1) {
        n = uart_read_bytes(JW01_UART_PORT, buf, 1, 0);  // timeout=0，非阻塞
        if (n <= 0) break;  // 缓冲区空了，退出

        if (buf[0] == JW01_FRAME_HEAD) {
            // 找到帧头，读剩余 4 字节（短超时 50ms）
            n = uart_read_bytes(JW01_UART_PORT, buf + 1, JW01_FRAME_LEN - 1,
                                pdMS_TO_TICKS(50));
            if (n >= JW01_FRAME_LEN - 1) {
                co2_value = ((uint16_t)buf[1] << 8) | buf[2];
            }
            break;
        }
        // 不是帧头则丢弃，继续读下一个字节
    }

    return co2_value;
}
