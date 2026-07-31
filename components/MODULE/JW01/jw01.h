#ifndef __JW01_H_
#define __JW01_H_

#include <stdint.h>

/* ---------- 协议定义 ---------- */
#define JW01_FRAME_HEAD   0x2C    // 帧头
#define JW01_FRAME_LEN    5       // 帧长度：HEAD + 4字节数据
#define JW01_TIMEOUT_MS   3000    // 读取超时

/* ---------- API ---------- */
void     JW01_Init(void);
uint16_t JW01_GetCO2(void);     // 获取当前 CO2 值（ppm）
uint8_t  JW01_ReadFrame(uint16_t *co2);  // 阻塞读取一帧数据

#endif
