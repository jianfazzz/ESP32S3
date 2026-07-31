#include "oled.h"
#include "oled_Font.h"
#include "keypad.h"

#include <stdarg.h>       // 用于可变参数
#include "sdkconfig.h"

//------------------ 底层 I2C 写操作 ------------------
static esp_err_t i2c_write_byte(uint8_t addr, uint8_t control_byte, uint8_t data)
{
    uint8_t buffer[2] = {control_byte, data};
    return i2c_master_transmit(oled_dev_handle, buffer, sizeof(buffer), -1);
}

//------------------ 基本命令/数据写 ------------------
void WriteCmd(uint8_t cmd)
{
    i2c_write_byte(OLED_ADDRESS, OLED_cmd, cmd);
}

void WriteDat(uint8_t dat)
{
    i2c_write_byte(OLED_ADDRESS, OLED_dat, dat);
}

//------------------ OLED 初始化 ------------------
void OLED_Init(void)
{

    WriteCmd(0xAE); // display off
    WriteCmd(0x20); // Set Memory Addressing Mode
    WriteCmd(0x10); // Page Addressing Mode
    WriteCmd(0xB0); // Set Page Start Address (0)
    WriteCmd(0xC8); // COM Output Scan Direction
    WriteCmd(0x00); // low column start address
    WriteCmd(0x10); // high column start address
    WriteCmd(0x40); // set start line address
    WriteCmd(0x81); // set contrast control
    WriteCmd(0xFF); // contrast 0~255
    WriteCmd(0xA1); // segment re-map (127->0)
    WriteCmd(0xA6); // normal display
    WriteCmd(0xA8); // set multiplex ratio
    WriteCmd(0x3F); // 1/64 duty
    WriteCmd(0xA4); // output follows RAM content
    WriteCmd(0xD3); // set display offset
    WriteCmd(0x00); // no offset
    WriteCmd(0xD5); // set display clock divide
    WriteCmd(0xF0); // divide ratio
    WriteCmd(0xD9); // set pre-charge period
    WriteCmd(0x22); 
    WriteCmd(0xDA); // set COM pins hardware config
    WriteCmd(0x12);
    WriteCmd(0xDB); // set VCOMH deselect level
    WriteCmd(0x20); // 0.77*VCC
    WriteCmd(0x8D); // charge pump enable
    WriteCmd(0x14); 
    WriteCmd(0xAF); // display on
}

//------------------ 设置显示起始点 ------------------
void OLED_SetPos(uint8_t x, uint8_t y)
{
    WriteCmd(0xB0 + y);
    WriteCmd(((x & 0xF0) >> 4) | 0x10);
    WriteCmd((x & 0x0F) | 0x00);   // 注意：STM32原始代码是 |0x01，此处按标准改为0x00
}

//------------------ 全屏填充 ------------------
void OLED_Fill(uint8_t fill_Data)
{
    uint8_t m, n;
    for(m = 0; m < 8; m++)
    {
        WriteCmd(0xB0 + m);
        WriteCmd(0x00);
        WriteCmd(0x10);
        for(n = 0; n < 128; n++)
        {
            WriteDat(fill_Data);
        }
    }
}

//------------------ 清屏 ------------------
void OLED_CLS(void)
{
    OLED_Fill(0x00);
}

//------------------ OLED 唤醒 ------------------
void OLED_ON(void)
{
    WriteCmd(0x8D);  // charge pump setting
    WriteCmd(0x14);  // enable charge pump
    WriteCmd(0xAF);  // display on
}

//------------------ OLED 休眠 ------------------
void OLED_OFF(void)
{
    WriteCmd(0x8D);  // charge pump setting
    WriteCmd(0x10);  // disable charge pump
    WriteCmd(0xAE);  // display off
}

//------------------ 显示一个字符 ------------------
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t TextSize, uint8_t mode)
{
    uint8_t c = chr - ' ';
    if(TextSize == FONT_8X16)
    {
        if(x > 120) { x = 0; y++; }
        OLED_SetPos(x, y);
        for(uint8_t i = 0; i < 8; i++)
            WriteDat(mode ? ~(F8X16[c*16 + i]) : F8X16[c*16 + i]);
        OLED_SetPos(x, y+1);
        for(uint8_t i = 0; i < 8; i++)
            WriteDat(mode ? ~(F8X16[c*16 + i + 8]) : F8X16[c*16 + i + 8]);
    }
    else  // FONT_6X8
    {
        if(x > 126) { x = 0; y++; }
        OLED_SetPos(x, y);
        for(uint8_t i = 0; i < 6; i++)
            WriteDat(mode ? ~(F6x8[c][i]) : F6x8[c][i]);
    }
}

//------------------ 显示字符串 ------------------
void OLED_ShowStr(uint8_t x, uint8_t y, const char ch[], uint8_t TextSize, uint8_t mode)
{
    uint8_t j = 0;
    while(ch[j] != '\0')
    {
        uint8_t c = (uint8_t)ch[j] - ' ';   // 显式转换为无符号
        if(TextSize == FONT_8X16)
        {
            if(x > 120) { x = 0; y++; }
            OLED_SetPos(x, y);
            for(uint8_t i = 0; i < 8; i++)
                WriteDat(mode ? ~(F8X16[c*16 + i]) : F8X16[c*16 + i]);
            OLED_SetPos(x, y+1);
            for(uint8_t i = 0; i < 8; i++)
                WriteDat(mode ? ~(F8X16[c*16 + i + 8]) : F8X16[c*16 + i + 8]);
            x += 8;
        }
        else  // FONT_6X8
        {
            if(x > 126) { x = 0; y++; }
            OLED_SetPos(x, y);
            for(uint8_t i = 0; i < 6; i++)
                WriteDat(mode ? ~(F6x8[c][i]) : F6x8[c][i]);
            x += 6;
        }
        j++;
    }
}

//------------------ 显示一个中文汉字（16x16） ------------------
void OLED_ShowCN(uint8_t x, uint8_t y, uint8_t N, uint8_t mode)
{
    uint16_t adder = 32 * N;   // 每个汉字32字节
    OLED_SetPos(x, y);
    for(uint8_t wm = 0; wm < 16; wm++)
    {
        // 修复按位取反报错：0xFF - val 替代 ~val
        WriteDat(mode ? (0xFF - F16x16[adder]) : F16x16[adder]);
        adder++;
    }
    OLED_SetPos(x, y+1);
    for(uint8_t wm = 0; wm < 16; wm++)
    {
        WriteDat(mode ? (0xFF - F16x16[adder]) : F16x16[adder]);
        adder++;
    }
}

//------------------ 显示位图 ------------------
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char N)
{
	unsigned int j=0;
	unsigned char x,y;

  if(y1%8==0)
		y = y1/8;
  else
		y = y1/8 + 1;
	for(y=y0;y<y1;y++)
	{
		OLED_SetPos(x0,y);
    for(x=x0;x<x1;x++)
		{
			WriteDat(BMP2[N][j++]);
		}
	}
}
//------------------ 显示摄氏温度符号 ------------------
void OLED_ShowCentigrade(uint8_t x, uint8_t y)
{
    uint8_t buf[] = {
        0x10,0x28,0x10,0xC0,0x20,0x10,0x10,0x10,0x20,0x70,0x00,0x00,
        0x00,0x00,0x00,0x07,0x08,0x10,0x10,0x10,0x10,0x08,0x04,0x00
    };
    OLED_SetPos(x, y);
    for(uint8_t i = 0; i < 12; i++)
        WriteDat(buf[i]);
    OLED_SetPos(x, y+1);
    for(uint8_t i = 0; i < 12; i++)
        WriteDat(buf[i+12]);
}

//------------------ 显示两位数（温度/湿度） ------------------
void OLED_ShowTH(uint8_t x, uint8_t y, uint8_t DATA)
{
    OLED_SetPos(x, y);
    for(uint8_t i = 0; i < 8; i++)
        WriteDat(F8X16[(DATA/10 + 16)*16 + i]);
    OLED_SetPos(x, y+1);
    for(uint8_t i = 0; i < 8; i++)
        WriteDat(F8X16[(DATA/10 + 16)*16 + i + 8]);

    OLED_SetPos(x+8, y);
    for(uint8_t i = 0; i < 8; i++)
        WriteDat(F8X16[(DATA%10 + 16)*16 + i]);
    OLED_SetPos(x+8, y+1);
    for(uint8_t i = 0; i < 8; i++)
        WriteDat(F8X16[(DATA%10 + 16)*16 + i + 8]);
}

//------------------ 格式化字符串显示 ------------------
void OLED_ShowFormatStr(uint8_t x, uint8_t y, const char* format, ...)
{
    char buf[32];
    va_list args;
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);
    OLED_ShowStr(x, y, buf, FONT_8X16, 0);  // 现在 buf 是 char[]，匹配
}


