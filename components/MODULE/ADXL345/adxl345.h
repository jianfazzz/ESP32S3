#ifndef __ADXL345_H_
#define __ADXL345_H_

#include <stdint.h>

// ADXL345 I2C 7位地址 (SDO接GND时为0x53, SDO接VCC时为0x1D)
#define ADXL345_ADDRESS       0x53

// ADXL345 寄存器地址
#define ADXL345_REG_DEVID           0x00  // 设备ID (只读, 固定为0xE5)
#define ADXL345_REG_THRESH_TAP      0x1D  // 敲击阈值
#define ADXL345_REG_OFSX            0x1E  // X轴偏移
#define ADXL345_REG_OFSY            0x1F  // Y轴偏移
#define ADXL345_REG_OFSZ            0x20  // Z轴偏移
#define ADXL345_REG_DUR             0x21  // 敲击持续时间
#define ADXL345_REG_LATENT          0x22  // 敲击延迟
#define ADXL345_REG_WINDOW          0x23  // 敲击窗口
#define ADXL345_REG_THRESH_ACT      0x24  // 活动阈值
#define ADXL345_REG_THRESH_INACT    0x25  // 静止阈值
#define ADXL345_REG_TIME_INACT      0x26  // 静止时间
#define ADXL345_REG_ACT_INACT_CTL   0x27  // 活动/静止控制
#define ADXL345_REG_THRESH_FF       0x28  // 自由落体阈值
#define ADXL345_REG_TIME_FF         0x29  // 自由落体时间
#define ADXL345_REG_TAP_AXES        0x2A  // 敲击轴使能
#define ADXL345_REG_ACT_TAP_STATUS  0x2B  // 活动/敲击状态
#define ADXL345_REG_BW_RATE         0x2C  // 数据速率及低功耗模式
#define ADXL345_REG_POWER_CTL       0x2D  // 电源控制
#define ADXL345_REG_INT_MAP         0x2E  // 中断映射
#define ADXL345_REG_INT_ENABLE      0x2F  // 中断使能
#define ADXL345_REG_INT_SOURCE      0x30  // 中断源
#define ADXL345_REG_DATA_FORMAT     0x31  // 数据格式
#define ADXL345_REG_DATAX0          0x32  // X轴数据低字节
#define ADXL345_REG_DATAX1          0x33  // X轴数据高字节
#define ADXL345_REG_DATAY0          0x34  // Y轴数据低字节
#define ADXL345_REG_DATAY1          0x35  // Y轴数据高字节
#define ADXL345_REG_DATAZ0          0x36  // Z轴数据低字节
#define ADXL345_REG_DATAZ1          0x37  // Z轴数据高字节
#define ADXL345_REG_FIFO_CTL        0x38  // FIFO控制
#define ADXL345_REG_FIFO_STATUS     0x39  // FIFO状态

// 设备ID
#define ADXL345_DEVICE_ID           0xE5

// 量程定义 (DATA_FORMAT寄存器 bit6:bit5)
#define ADXL345_RANGE_2G            0x00  // ±2g  (10位, 4mg/LSB)
#define ADXL345_RANGE_4G            0x01  // ±4g  (10位, 8mg/LSB)
#define ADXL345_RANGE_8G            0x02  // ±8g  (10位, 16mg/LSB)
#define ADXL345_RANGE_16G           0x03  // ±16g (10位, 32mg/LSB)

// 数据速率定义 (BW_RATE寄存器 bit3:bit0)
#define ADXL345_RATE_0_10           0x00  // 0.10 Hz
#define ADXL345_RATE_0_20           0x01  // 0.20 Hz
#define ADXL345_RATE_0_39           0x02  // 0.39 Hz
#define ADXL345_RATE_0_78           0x03  // 0.78 Hz
#define ADXL345_RATE_1_56           0x04  // 1.56 Hz
#define ADXL345_RATE_3_13           0x05  // 3.13 Hz
#define ADXL345_RATE_6_25           0x06  // 6.25 Hz
#define ADXL345_RATE_12_5           0x07  // 12.5 Hz
#define ADXL345_RATE_25             0x08  // 25 Hz
#define ADXL345_RATE_50             0x09  // 50 Hz
#define ADXL345_RATE_100            0x0A  // 100 Hz (默认)
#define ADXL345_RATE_200            0x0B  // 200 Hz
#define ADXL345_RATE_400            0x0C  // 400 Hz
#define ADXL345_RATE_800            0x0D  // 800 Hz
#define ADXL345_RATE_1600           0x0E  // 1600 Hz
#define ADXL345_RATE_3200           0x0F  // 3200 Hz

// 函数声明
void    ADXL345_Init(void);
uint8_t ADXL345_GetDeviceID(void);
void    ADXL345_SetRange(uint8_t range);
void    ADXL345_SetDataRate(uint8_t rate);
void    ADXL345_SetOffset(int8_t x, int8_t y, int8_t z);
void    ADXL345_PowerOn(void);
void    ADXL345_PowerOff(void);
void    ADXL345_ReadRaw(int16_t *x, int16_t *y, int16_t *z);
void    ADXL345_ReadAccel(float *x, float *y, float *z);
void    ADXL345_ReadAverage(float *x, float *y, float *z, uint8_t times);
float   ADXL345_ReadX(void);
float   ADXL345_ReadY(void);
float   ADXL345_ReadZ(void);

#endif
