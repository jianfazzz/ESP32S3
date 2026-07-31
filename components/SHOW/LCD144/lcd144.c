#include "lcd144.h"
#include "lcd144_Font.h"
#include "keypad.h"


// 毫秒延时
static void delay_ms(uint32_t ms) {
    vTaskDelay(pdMS_TO_TICKS(ms));
}

uint8_t LCD144_transfer_byte(uint8_t data){
    spi_transaction_t t = {0};
    t.length = 8;
    t.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA;
    t.tx_data[0] = data;
    spi_device_polling_transmit(lcd144_dev_handle, &t);
    return t.rx_data[0];
}

void LCD144_write_data(uint8_t *data, uint32_t len){
    if (len == 0 || data == NULL) return;

    uint32_t offset = 0;
    while (offset < len) {
        uint32_t chunk = (len - offset > 16384) ? 16384 : (len - offset);
        spi_transaction_t t = {0};
        t.length = 8 * chunk;
        t.tx_buffer = data + offset;
        esp_err_t ret = spi_device_polling_transmit(lcd144_dev_handle, &t);
        offset += chunk;
    }
}

// GPIO 初始化
void LCD_GPIO_init(void) {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;

    // DC, RST, BLK 引脚
    io_conf.pin_bit_mask = (1ULL << LCD_DC) | (1ULL << LCD_RST) ;//| (1ULL << LCD_BLK)
    gpio_config(&io_conf);

    // 初始电平
    gpio_set_level(LCD_DC, 1);
    gpio_set_level(LCD_RST, 1);
    // gpio_set_level(LCD_BLK, 1);
}

// // SPI 初始化（调用 spi.c 中的函数）
// extern void SPI_init(void);

// 底层写命令（8 位）
void LCD_WriteCmd(uint8_t dat) {
    gpio_set_level(LCD_DC, 0);            // DC 低表示命令
    LCD144_transfer_byte(dat);
    gpio_set_level(LCD_DC, 1);            // 恢复 DC 高，准备数据
}

// 底层写数据（8 位）
void LCD_WriteDat8(uint8_t dat) {
    LCD144_transfer_byte(dat);
}

// 底层写数据（16 位，颜色值）
void LCD_WriteDat(uint16_t dat) {
    uint8_t buf[2] = { (uint8_t)(dat >> 8), (uint8_t)dat };
    LCD144_write_data(buf, 2);
}

// 批量写 16 位数据（用于图片等）
void LCD_WriteDateMultiple(uint8_t *data, uint16_t len) {
    for (uint16_t i = 0; i < len; i++) {
        LCD_WriteDat(data[i]);
    }
}

// 设置显示窗口（简化版，兼容原代码）
void LCD_SetWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    LCD_WriteCmd(0x2A);
    LCD_WriteDat(0x00);
    LCD_WriteDat(x0);
    LCD_WriteDat(0x00);
    LCD_WriteDat(x1);

    LCD_WriteCmd(0x2B);
    LCD_WriteDat(0x00);
    LCD_WriteDat(y0);
    LCD_WriteDat(0x00);
    LCD_WriteDat(y1);

    LCD_WriteCmd(0x2C);
}

// 设置显示地址（带方向偏移）
void LCD_Address_Set(u16 x1, u16 y1, u16 x2, u16 y2) {
    if (USE_HORIZONTAL == 0) {
        LCD_WriteCmd(0x2A);
        LCD_WriteDat(x1 + 2);
        LCD_WriteDat(x2 + 2);
        LCD_WriteCmd(0x2B);
        LCD_WriteDat(y1 + 1);
        LCD_WriteDat(y2 + 1);
    } else if (USE_HORIZONTAL == 1) {
        LCD_WriteCmd(0x2A);
        LCD_WriteDat(x1 + 2);
        LCD_WriteDat(x2 + 2);
        LCD_WriteCmd(0x2B);
        LCD_WriteDat(y1 + 3);
        LCD_WriteDat(y2 + 3);
    } else if (USE_HORIZONTAL == 2) {
        LCD_WriteCmd(0x2A);
        LCD_WriteDat(x1 + 1);
        LCD_WriteDat(x2 + 1);
        LCD_WriteCmd(0x2B);
        LCD_WriteDat(y1 + 2);
        LCD_WriteDat(y2 + 2);
    } else { // 默认 3
        LCD_WriteCmd(0x2A);
        LCD_WriteDat(x1 + 3);
        LCD_WriteDat(x2 + 3);
        LCD_WriteCmd(0x2B);
        LCD_WriteDat(y1 + 2);
        LCD_WriteDat(y2 + 2);
    }
    LCD_WriteCmd(0x2C);
}

// LCD 初始化序列（ST7735S）
void LCD_Init(void) {
    LCD_GPIO_init();

    gpio_set_level(LCD_RST, 0);
    delay_ms(100);
    gpio_set_level(LCD_RST, 1);
    delay_ms(100);

    // gpio_set_level(LCD_BLK, 1);
    // delay_ms(100);

    LCD_WriteCmd(0x11); // Sleep out
    delay_ms(120);

    // 帧速率设置
    LCD_WriteCmd(0xB1); LCD_WriteDat8(0x02); LCD_WriteDat8(0x35); LCD_WriteDat8(0x36);
    LCD_WriteCmd(0xB2); LCD_WriteDat8(0x02); LCD_WriteDat8(0x35); LCD_WriteDat8(0x36);
    LCD_WriteCmd(0xB3); LCD_WriteDat8(0x02); LCD_WriteDat8(0x35); LCD_WriteDat8(0x36);
    LCD_WriteDat8(0x02); LCD_WriteDat8(0x35); LCD_WriteDat8(0x36);

    LCD_WriteCmd(0xB4); LCD_WriteDat8(0x03);

    // 电源序列
    LCD_WriteCmd(0xC0); LCD_WriteDat8(0xA2); LCD_WriteDat8(0x02); LCD_WriteDat8(0x84);
    LCD_WriteCmd(0xC1); LCD_WriteDat8(0xC5);
    LCD_WriteCmd(0xC2); LCD_WriteDat8(0x0D); LCD_WriteDat8(0x00);
    LCD_WriteCmd(0xC3); LCD_WriteDat8(0x8D); LCD_WriteDat8(0x2A);
    LCD_WriteCmd(0xC4); LCD_WriteDat8(0x8D); LCD_WriteDat8(0xEE);

    LCD_WriteCmd(0xC5); LCD_WriteDat8(0x0A);

    // 显示方向
    LCD_WriteCmd(0x36);
    if (USE_HORIZONTAL == 0)      LCD_WriteDat8(0x08);
    else if (USE_HORIZONTAL == 1) LCD_WriteDat8(0xC8);
    else if (USE_HORIZONTAL == 2) LCD_WriteDat8(0x78);
    else                          LCD_WriteDat8(0xA8);

    // Gamma 校正
    LCD_WriteCmd(0xE0);
    LCD_WriteDat8(0x12); LCD_WriteDat8(0x1C); LCD_WriteDat8(0x10); LCD_WriteDat8(0x18);
    LCD_WriteDat8(0x33); LCD_WriteDat8(0x2C); LCD_WriteDat8(0x25); LCD_WriteDat8(0x28);
    LCD_WriteDat8(0x28); LCD_WriteDat8(0x27); LCD_WriteDat8(0x2F); LCD_WriteDat8(0x3C);
    LCD_WriteDat8(0x00); LCD_WriteDat8(0x03); LCD_WriteDat8(0x03); LCD_WriteDat8(0x10);

    LCD_WriteCmd(0xE1);
    LCD_WriteDat8(0x12); LCD_WriteDat8(0x1C); LCD_WriteDat8(0x10); LCD_WriteDat8(0x18);
    LCD_WriteDat8(0x2D); LCD_WriteDat8(0x28); LCD_WriteDat8(0x23); LCD_WriteDat8(0x28);
    LCD_WriteDat8(0x28); LCD_WriteDat8(0x26); LCD_WriteDat8(0x2F); LCD_WriteDat8(0x3B);
    LCD_WriteDat8(0x00); LCD_WriteDat8(0x03); LCD_WriteDat8(0x03); LCD_WriteDat8(0x10);

    LCD_WriteCmd(0x3A); LCD_WriteDat8(0x05); // 16 位色
    LCD_WriteCmd(0x29); // Display on
}

// ============== 绘图函数（完全照搬原 STM32 逻辑） ==============

void LCD_CLS(void){
    LCD_Fill(0,0,LCD_W,LCD_H,BLACK);//清屏
}

void LCD_Fill(u16 xsta, u16 ysta, u16 xend, u16 yend, u16 color) {
    u16 i, j;
    LCD_Address_Set(xsta, ysta, xend - 1, yend - 1);
    for (i = ysta; i < yend; i++)
        for (j = xsta; j < xend; j++)
            LCD_WriteDat(color);
}

void LCD_DrawPoint(u16 x, u16 y, u16 color) {
    LCD_Address_Set(x, y, x, y);
    LCD_WriteDat(color);
}

void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color) {
    u16 t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1;
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;
    if (delta_x > 0) incx = 1;
    else if (delta_x == 0) incx = 0;
    else { incx = -1; delta_x = -delta_x; }
    if (delta_y > 0) incy = 1;
    else if (delta_y == 0) incy = 0;
    else { incy = -1; delta_y = -delta_y; }
    if (delta_x > delta_y) distance = delta_x;
    else distance = delta_y;
    for (t = 0; t < distance + 1; t++) {
        LCD_DrawPoint(uRow, uCol, color);
        xerr += delta_x;
        yerr += delta_y;
        if (xerr > distance) {
            xerr -= distance;
            uRow += incx;
        }
        if (yerr > distance) {
            yerr -= distance;
            uCol += incy;
        }
    }
}

void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 color) {
    LCD_DrawLine(x1, y1, x2, y1, color);
    LCD_DrawLine(x1, y1, x1, y2, color);
    LCD_DrawLine(x1, y2, x2, y2, color);
    LCD_DrawLine(x2, y1, x2, y2, color);
}

void Draw_Circle(u16 x0, u16 y0, u8 r, u16 color) {
    int a, b;
    a = 0; b = r;
    while (a <= b) {
        LCD_DrawPoint(x0 - b, y0 - a, color);
        LCD_DrawPoint(x0 + b, y0 - a, color);
        LCD_DrawPoint(x0 - a, y0 + b, color);
        LCD_DrawPoint(x0 - a, y0 - b, color);
        LCD_DrawPoint(x0 + b, y0 + a, color);
        LCD_DrawPoint(x0 + a, y0 - b, color);
        LCD_DrawPoint(x0 + a, y0 + b, color);
        LCD_DrawPoint(x0 - b, y0 + a, color);
        a++;
        if ((a * a + b * b) > (r * r)) b--;
    }
}

// ============== 文字/图片显示（原逻辑全部保留） ==============

void LCD_ShowChinese(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode) {
    while (*s != 0) {
        if (sizey == 16) LCD_ShowChinese16x16(x, y, s, fc, bc, sizey, mode);
        else if (sizey == 24) LCD_ShowChinese24x24(x, y, s, fc, bc, sizey, mode);
        else if (sizey == 32) LCD_ShowChinese32x32(x, y, s, fc, bc, sizey, mode);
        else return;
        s += 2;
        x += sizey;
    }
}

void LCD_ShowChinese16x16(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode) {
    u8 i, j;
    u16 k;
    u16 HZnum = sizeof(tfont16) / sizeof(typFNT_GB16);
    u16 TypefaceNum = sizey / 8 * sizey;
    u16 x0 = x;
    for (k = 0; k < HZnum; k++) {
        if ((tfont16[k].Index[0] == *(s)) && (tfont16[k].Index[1] == *(s + 1))) {
            LCD_Address_Set(x, y, x + sizey - 1, y + sizey - 1);
            for (i = 0; i < TypefaceNum; i++) {
                for (j = 0; j < 8; j++) {
                    if (!mode) {
                        if (tfont16[k].Msk[i] & (0x01 << j)) LCD_WriteDat(fc);
                        else LCD_WriteDat(bc);
                    } else {
                        if (tfont16[k].Msk[i] & (0x01 << j)) LCD_DrawPoint(x, y, fc);
                        x++;
                        if ((x - x0) == sizey) { x = x0; y++; break; }
                    }
                }
            }
        }
    }
}

void LCD_ShowChinese24x24(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode) {
    u8 i, j;
    u16 k;
    u16 HZnum = sizeof(tfont24) / sizeof(typFNT_GB24);
    u16 TypefaceNum = sizey / 8 * sizey;
    u16 x0 = x;
    for (k = 0; k < HZnum; k++) {
        if ((tfont24[k].Index[0] == *(s)) && (tfont24[k].Index[1] == *(s + 1))) {
            LCD_Address_Set(x, y, x + sizey - 1, y + sizey - 1);
            for (i = 0; i < TypefaceNum; i++) {
                for (j = 0; j < 8; j++) {
                    if (!mode) {
                        if (tfont24[k].Msk[i] & (0x01 << j)) LCD_WriteDat(fc);
                        else LCD_WriteDat(bc);
                    } else {
                        if (tfont24[k].Msk[i] & (0x01 << j)) LCD_DrawPoint(x, y, fc);
                        x++;
                        if ((x - x0) == sizey) { x = x0; y++; break; }
                    }
                }
            }
        }
    }
}

void LCD_ShowChinese32x32(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode) {
    u8 i, j;
    u16 k;
    u16 HZnum = sizeof(tfont32) / sizeof(typFNT_GB32);
    u16 TypefaceNum = sizey / 8 * sizey;
    u16 x0 = x;
    for (k = 0; k < HZnum; k++) {
        if ((tfont32[k].Index[0] == *(s)) && (tfont32[k].Index[1] == *(s + 1))) {
            LCD_Address_Set(x, y, x + sizey - 1, y + sizey - 1);
            for (i = 0; i < TypefaceNum; i++) {
                for (j = 0; j < 8; j++) {
                    if (!mode) {
                        if (tfont32[k].Msk[i] & (0x01 << j)) LCD_WriteDat(fc);
                        else LCD_WriteDat(bc);
                    } else {
                        if (tfont32[k].Msk[i] & (0x01 << j)) LCD_DrawPoint(x, y, fc);
                        x++;
                        if ((x - x0) == sizey) { x = x0; y++; break; }
                    }
                }
            }
        }
    }
}

void LCD_ShowChar(u16 x, u16 y, u8 num, u16 fc, u16 bc, u8 sizey, u8 mode) {
    u8 temp, sizex, t;
    u16 i, TypefaceNum;
    u16 x0 = x;
    sizex = sizey / 2;
    TypefaceNum = sizex / 8 * sizey;
    num = num - ' ';
    LCD_Address_Set(x, y, x + sizex - 1, y + sizey - 1);
    for (i = 0; i < TypefaceNum; i++) {
        if (sizey == 16) temp = ascii_1608[num][i];
        else if (sizey == 32) temp = ascii_3216[num][i];
        else return;
        for (t = 0; t < 8; t++) {
            if (!mode) {
                if (temp & (0x01 << t)) LCD_WriteDat(fc);
                else LCD_WriteDat(bc);
            } else {
                if (temp & (0x01 << t)) LCD_DrawPoint(x, y, fc);
                x++;
                if ((x - x0) == sizex) { x = x0; y++; break; }
            }
        }
    }
}

void LCD_ShowString(u16 x, u16 y, const u8 *p, u16 fc, u16 bc, u8 sizey, u8 mode) {
    while (*p != '\0') {
        LCD_ShowChar(x, y, *p, fc, bc, sizey, mode);
        x += sizey / 2;
        p++;
    }
}

u32 mypow(u8 m, u8 n) {
    u32 result = 1;
    while (n--) result *= m;
    return result;
}

void LCD_ShowIntNum(u16 x, u16 y, u16 num, u8 len, u16 fc, u16 bc, u8 sizey) {
    u8 t, temp, enshow = 0;
    u8 sizex = sizey / 2;
    for (t = 0; t < len; t++) {
        temp = (num / mypow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1)) {
            if (temp == 0) {
                LCD_ShowChar(x + t * sizex, y, ' ', fc, bc, sizey, 0);
                continue;
            } else enshow = 1;
        }
        LCD_ShowChar(x + t * sizex, y, temp + 48, fc, bc, sizey, 0);
    }
}

void LCD_ShowFloatNum1(u16 x, u16 y, float num, u8 len, u16 fc, u16 bc, u8 sizey) {
    u8 t, temp, sizex;
    u16 num1;
    sizex = sizey / 2;
    num1 = num * 100;
    for (t = 0; t < len; t++) {
        temp = (num1 / mypow(10, len - t - 1)) % 10;
        if (t == (len - 2)) {
            LCD_ShowChar(x + (len - 2) * sizex, y, '.', fc, bc, sizey, 0);
            t++;
            len += 1;
        }
        LCD_ShowChar(x + t * sizex, y, temp + 48, fc, bc, sizey, 0);
    }
}

void LCD_ShowPicture(u16 x, u16 y, u16 length, u16 width, const u8 pic[]) {
    u16 i, j, k = 0;
    LCD_Address_Set(x, y, x + length - 1, y + width - 1);
    for (i = 0; i < length; i++) {
        for (j = 0; j < width; j++) {
            LCD_WriteDat8(pic[k * 2]);
            LCD_WriteDat8(pic[k * 2 + 1]);
            k++;
        }
    }
}

