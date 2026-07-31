#ifndef __LCD144_H_
#define __LCD144_H_

#include <stdint.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;

// LCD 引脚定义
#define LCD_RST     41
#define LCD_DC      40
#define LCD_CS      39
// #define LCD_BLK     14   接3.3v，屏幕常量，此处不定义

// 显示方向（0/1 竖屏，2/3 横屏）
#define USE_HORIZONTAL  2

#if USE_HORIZONTAL==0 || USE_HORIZONTAL==1
#define LCD_W 128
#define LCD_H 128
#else
#define LCD_W 128
#define LCD_H 128
#endif

// 颜色定义（RGB565）
#define WHITE       0xFFFF
#define BLACK       0x0000
#define BLUE        0x001F
#define BRED        0xF81F
#define GRED        0xFFE0
#define GBLUE       0x07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define YELLOW      0xFFE0
#define BROWN       0xBC40
#define BRRED       0xFC07
#define GRAY        0x8430
#define DARKBLUE    0x01CF
#define LIGHTBLUE   0x7D7C
#define GRAYBLUE    0x5458
#define LIGHTGREEN  0x841F
#define LGRAY       0xC618
#define LGRAYBLUE   0xA651
#define LBBLUE      0x2B12

// 函数声明
void LCD_GPIO_init(void);
void LCD_Init(void);
void LCD_WriteCmd(uint8_t dat);
void LCD_WriteDat(uint16_t dat);
void LCD_WriteDat8(uint8_t dat);
void LCD_WriteDateMultiple(uint8_t *data, uint16_t len);
void LCD_SetWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void LCD_Address_Set(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_CLS(void);
void LCD_Fill(u16 xsta, u16 ysta, u16 xend, u16 yend, u16 color);
void LCD_DrawPoint(u16 x, u16 y, u16 color);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void Draw_Circle(u16 x0, u16 y0, u8 r, u16 color);

void LCD_ShowChinese(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode);
void LCD_ShowChinese16x16(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode);
void LCD_ShowChinese24x24(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode);
void LCD_ShowChinese32x32(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode);
void LCD_ShowChar(u16 x, u16 y, u8 num, u16 fc, u16 bc, u8 sizey, u8 mode);
void LCD_ShowString(u16 x, u16 y, const u8 *p, u16 fc, u16 bc, u8 sizey, u8 mode);
u32 mypow(u8 m, u8 n);
void LCD_ShowIntNum(u16 x, u16 y, u16 num, u8 len, u16 fc, u16 bc, u8 sizey);
void LCD_ShowFloatNum1(u16 x, u16 y, float num, u8 len, u16 fc, u16 bc, u8 sizey);
void LCD_ShowPicture(u16 x, u16 y, u16 length, u16 width, const u8 pic[]);

#endif

