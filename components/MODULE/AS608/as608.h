#ifndef __AS608_H_
#define __AS608_H_

#include <stdint.h>

#define AS608_TCH     11

/* ---------- 包标识 ---------- */
#define AS608_PID_CMD      0x01
#define AS608_PID_ACK      0x07

/* ---------- 指令码 ---------- */
#define AS608_CMD_GENIMG       0x01  // 采集指纹图像
#define AS608_CMD_IMG2TZ       0x02  // 生成特征
#define AS608_CMD_MATCH        0x03  // 精确比对
#define AS608_CMD_SEARCH       0x04  // 搜索指纹库
#define AS608_CMD_REGMODEL     0x05  // 合成模板
#define AS608_CMD_STORE        0x06  // 储存模板
#define AS608_CMD_LOADCHAR     0x07  // 读出模板
#define AS608_CMD_DELETCHAR    0x0C  // 删除模板
#define AS608_CMD_EMPTY        0x0D  // 清空指纹库
#define AS608_CMD_READSYSPARA  0x0F  // 读系统参数

/* ---------- 应答码 ---------- */
#define AS608_OK               0x00  // 操作成功
#define AS608_ERR              0x01  // 收包错误
#define AS608_NOFINGER         0x02  // 无手指
#define AS608_IMGCLEAR_FAIL    0x03  // 图像不清晰
#define AS608_NOT_MATCH        0x08  // 比对不成功
#define AS608_NOT_FOUND        0x09  // 未搜索到

/* ---------- API ---------- */
void    AS608_Init(void);
uint8_t AS608_GetFingerPressed(void);          // 读取 TCH 引脚状态
uint8_t AS608_GenImg(void);                     // 采集指纹图像
uint8_t AS608_Img2Tz(uint8_t buffer_id);        // 生成特征 (buffer 1/2)
uint8_t AS608_RegModel(void);                   // 合成模板
uint8_t AS608_Store(uint8_t buffer_id, uint16_t page_id);  // 储存模板
uint8_t AS608_Search(uint8_t buffer_id, uint16_t start, uint16_t count,
                     uint16_t *page_id, uint16_t *score);   // 搜索指纹库
uint8_t AS608_Empty(void);                      // 清空指纹库
uint8_t AS608_DeleteChar(uint16_t page_id, uint16_t count); // 删除模板
uint8_t AS608_GetFingerCount(uint16_t *count);  // 获取已存指纹数
uint8_t AS608_SelfTest(void);                    // 自动扫描波特率并打印信息
uint8_t AS608_PollFinger(uint16_t *page_id, uint16_t *score); // 短超时轮询搜索指纹

#endif
