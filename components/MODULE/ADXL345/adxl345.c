#include "adxl345.h"
#include "keypad.h"


static const char *TAG = "ADXL345";

// 当前量程, 用于换算加速度
static uint8_t current_range = ADXL345_RANGE_2G;
static uint8_t is_initialized = 0;

/**
  * @brief  向ADXL345寄存器写入单字节
  * @param  reg: 寄存器地址
  * @param  value: 写入值
  * @retval None
  */
static void ADXL345_WriteReg(uint8_t reg, uint8_t value)
{
    uint8_t buffer[2] = {reg, value};
    i2c_master_transmit(adxl345_dev_handle, buffer, sizeof(buffer), -1);
}

/**
  * @brief  从ADXL345寄存器读取单字节
  * @param  reg: 寄存器地址
  * @retval 读取到的字节
  */
static uint8_t ADXL345_ReadReg(uint8_t reg)
{
    uint8_t value = 0;
    i2c_master_transmit_receive(adxl345_dev_handle, &reg, 1, &value, 1, -1);
    return value;
}

/**
  * @brief  从ADXL345连续读取多个字节
  * @param  reg: 起始寄存器地址
  * @param  buf: 数据缓冲区
  * @param  len: 读取长度
  * @retval None
  */
static void ADXL345_ReadMulti(uint8_t reg, uint8_t *buf, uint8_t len)
{
    i2c_master_transmit_receive(adxl345_dev_handle, &reg, 1, buf, len, -1);
}

/**
  * @brief  ADXL345初始化
  *         配置为±16g量程, 100Hz数据速率, 10位分辨率, 进入测量模式
  *         (用大量程避免X轴偏移饱和)
  * @retval None
  */
void ADXL345_Init(void)
{
    // 1. 进入待机模式以便配置寄存器
    ADXL345_PowerOff();
    vTaskDelay(pdMS_TO_TICKS(10));

    // 2. 校验设备ID
    uint8_t id = ADXL345_GetDeviceID();


    // 3. 设置数据格式: ±16g量程, 10位分辨率, 右对齐
    current_range = ADXL345_RANGE_16G;
    ADXL345_WriteReg(ADXL345_REG_DATA_FORMAT, current_range);

    // 4. 设置数据速率100Hz
    ADXL345_SetDataRate(ADXL345_RATE_100);

    // 5. 清除偏移
    ADXL345_SetOffset(0, 0, 0);

    // 6. 进入测量模式
    ADXL345_PowerOn();
    vTaskDelay(pdMS_TO_TICKS(20));

    is_initialized = 1;
}

/**
  * @brief  读取设备ID
  * @retval 设备ID (应为0xE5)
  */
uint8_t ADXL345_GetDeviceID(void)
{
    return ADXL345_ReadReg(ADXL345_REG_DEVID);
}

/**
  * @brief  设置量程
  * @param  range: ADXL345_RANGE_2G / _4G / _8G / _16G
  * @retval None
  */
void ADXL345_SetRange(uint8_t range)
{
    range &= 0x03;
    // 保留DATA_FORMAT其它位, 仅更新bit6:bit5
    uint8_t fmt = ADXL345_ReadReg(ADXL345_REG_DATA_FORMAT);
    fmt = (fmt & ~0x60) | (range << 5);
    ADXL345_WriteReg(ADXL345_REG_DATA_FORMAT, fmt);
    current_range = range;
}

/**
  * @brief  设置数据速率
  * @param  rate: ADXL345_RATE_xxx (0.1Hz~3200Hz)
  * @retval None
  */
void ADXL345_SetDataRate(uint8_t rate)
{
    rate &= 0x0F;
    ADXL345_WriteReg(ADXL345_REG_BW_RATE, rate);
}

/**
  * @brief  设置三轴偏移 (用于校准)
  * @param  x/y/z: 偏移值, 范围-128~127 (约15.6mg/LSB)
  * @retval None
  */
void ADXL345_SetOffset(int8_t x, int8_t y, int8_t z)
{
    ADXL345_WriteReg(ADXL345_REG_OFSX, (uint8_t)x);
    ADXL345_WriteReg(ADXL345_REG_OFSY, (uint8_t)y);
    ADXL345_WriteReg(ADXL345_REG_OFSZ, (uint8_t)z);
}

/**
  * @brief  进入测量模式 (唤醒)
  * @retval None
  */
void ADXL345_PowerOn(void)
{
    uint8_t ctl = ADXL345_ReadReg(ADXL345_REG_POWER_CTL);
    ctl &= ~0x38;   // 清除 SLEEP / AUTO_SLEEP / WAKEUP
    ctl |= 0x08;    // 置位 MEASURE
    ADXL345_WriteReg(ADXL345_REG_POWER_CTL, ctl);
}

/**
  * @brief  进入待机模式 (休眠)
  * @retval None
  */
void ADXL345_PowerOff(void)
{
    uint8_t ctl = ADXL345_ReadReg(ADXL345_REG_POWER_CTL);
    ctl &= ~0x08;   // 清除 MEASURE -> standby
    ADXL345_WriteReg(ADXL345_REG_POWER_CTL, ctl);
}

/**
  * @brief  读取三轴原始数据 (10位补码, 右对齐)
  * @param  x/y/z: 三轴原始数据输出指针
  * @retval None
  */
void ADXL345_ReadRaw(int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t buf[6];
    // DATAX0~DATAZ1 连续存放, 支持连续读
    ADXL345_ReadMulti(ADXL345_REG_DATAX0, buf, 6);

    *x = (int16_t)(((uint16_t)buf[1] << 8) | buf[0]);
    *y = (int16_t)(((uint16_t)buf[3] << 8) | buf[2]);
    *z = (int16_t)(((uint16_t)buf[5] << 8) | buf[4]);
}

/**
  * @brief  读取三轴加速度 (单位: g)
  *         10位模式下比例因子 = 4 * 2^range mg/LSB
  * @param  x/y/z: 三轴加速度输出指针
  * @retval None
  */
void ADXL345_ReadAccel(float *x, float *y, float *z)
{
    int16_t rx, ry, rz;
    if (!is_initialized) {
        ADXL345_Init();
    }
    ADXL345_ReadRaw(&rx, &ry, &rz);
    float scale = 0.004f * (1 << current_range);   // g/LSB
    *x = rx * scale;
    *y = ry * scale;
    *z = rz * scale;
}

/**
  * @brief  连续读取多次取平均值 (滤除抖动)
  * @param  x/y/z: 三轴加速度平均值输出指针 (单位: g)
  * @param  times: 采样次数
  * @retval None
  */
void ADXL345_ReadAverage(float *x, float *y, float *z, uint8_t times)
{
    if (times == 0) {
        *x = *y = *z = 0.0f;
        return;
    }
    float sx = 0.0f, sy = 0.0f, sz = 0.0f;
    for (uint8_t i = 0; i < times; i++) {
        float tx, ty, tz;
        ADXL345_ReadAccel(&tx, &ty, &tz);
        sx += tx; sy += ty; sz += tz;
        vTaskDelay(pdMS_TO_TICKS(5));
    }
    *x = sx / times;
    *y = sy / times;
    *z = sz / times;
}

/**
  * @brief  读取X轴加速度 (单位: g)
  * @retval X轴加速度, -1.0表示未初始化失败
  */
float ADXL345_ReadX(void)
{
    float x, y, z;
    ADXL345_ReadAccel(&x, &y, &z);
    return x;
}

/**
  * @brief  读取Y轴加速度 (单位: g)
  * @retval Y轴加速度
  */
float ADXL345_ReadY(void)
{
    float x, y, z;
    ADXL345_ReadAccel(&x, &y, &z);
    return y;
}

/**
  * @brief  读取Z轴加速度 (单位: g)
  * @retval Z轴加速度
  */
float ADXL345_ReadZ(void)
{
    float x, y, z;
    ADXL345_ReadAccel(&x, &y, &z);
    return z;
}


// void TEST(void){
//     float x, y, z;
//     ADXL345_Init();
//     ADXL345_ReadAccel(&x, &y, &z);  
// }