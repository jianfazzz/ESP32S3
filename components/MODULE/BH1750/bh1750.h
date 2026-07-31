#ifndef __BH1750_H_
#define __BH1750_H_

#include <stdint.h>


// BH1750指令集
#define BH1750_POWER_DOWN     0x00  // 断电
#define BH1750_POWER_ON       0x01  // 上电
#define BH1750_RESET          0x07  // 重置
#define BH1750_CONT_H_MODE    0x10  // 连续高分辨率模式(0.5lx精度)
#define BH1750_CONT_H_MODE2   0x11  // 连续高分辨率模式2(1lx精度)
#define BH1750_CONT_L_MODE    0x13  // 连续低分辨率模式(4lx精度)
#define BH1750_ONE_H_MODE     0x20  // 一次高分辨率模式(0.5lx精度)
#define BH1750_ONE_H_MODE2    0x21  // 一次高分辨率模式2(1lx精度)
#define BH1750_ONE_L_MODE     0x23  // 一次低分辨率模式(4lx精度)

// BH1750 I2C地址 (7位地址左移1位后写地址为0x46)
#define BH1750_ADDRESS       0x23  // 7位I2C地址 (0x46 >> 1)

// 函数声明
void BH1750_Init(void);
float BH1750_GetLux(void);
void BH1750_WriteCmd(uint8_t cmd);
uint8_t BH1750_ReadData(uint16_t *data);
void BH1750_SetMode(uint8_t mode);
void BH1750_Reset(void);
void BH1750_PowerDown(void);
void BH1750_PowerOn(void);
float BH1750_ReadOnce(uint8_t mode);
void BH1750_Restart(void);

#endif
