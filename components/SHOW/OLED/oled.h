#ifndef __OLED_H_
#define __OLED_H_

#include <stdint.h>

// SSD1306 I2C地址（7位地址，对应STM32中的0x78）
#define OLED_ADDRESS  0x3C
#define OLED_cmd      0x00   // 控制字节：命令
#define OLED_dat      0x40   // 控制字节：数据

// 显示模式定义
#define NORMAL_DISPLAY  0
#define INVERSE_DISPLAY 1

// 字符大小定义
#define FONT_6X8        1
#define FONT_8X16       2

// 函数声明
void OLED_Init(void);
void WriteCmd(uint8_t cmd);
void WriteDat(uint8_t dat);
void OLED_SetPos(uint8_t x, uint8_t y);
void OLED_Fill(uint8_t fill_Data);
void OLED_CLS(void);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t TextSize, uint8_t mode);
void OLED_ShowStr(uint8_t x, uint8_t y, const char ch[], uint8_t TextSize, uint8_t mode);
void OLED_ShowCN(uint8_t x, uint8_t y, uint8_t N, uint8_t mode);
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char N);
void OLED_ShowCentigrade(uint8_t x, uint8_t y);
void OLED_ShowTH(uint8_t x, uint8_t y, uint8_t DATA);
void OLED_ShowFormatStr(uint8_t x, uint8_t y, const char* format, ...);

// 外部字体数据声明（请在 oled_Font.h 中定义实际字模数组）
extern const uint8_t F6x8[][6];
extern const uint8_t F8X16[];
extern const uint8_t F16x16[];
extern const uint8_t BMP2[][128*8];
#endif


