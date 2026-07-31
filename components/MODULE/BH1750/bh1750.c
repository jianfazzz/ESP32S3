#include "bh1750.h"
#include "keypad.h"


static const char *TAG = "BH1750";

// 全局变量记录当前模式
static uint8_t current_mode = 0;
static uint8_t is_initialized = 0;

/**
  * @brief  向BH1750发送单字节命令
  * @param  cmd: 要发送的命令
  * @retval None
  */
void BH1750_WriteCmd(uint8_t cmd)
{
    esp_err_t ret;
    uint8_t buffer[1] = {cmd};
    
    i2c_master_transmit(bh1750_dev_handle, buffer, sizeof(buffer), -1);
}

/**
  * @brief  从BH1750读取数据
  * @param  data: 读取的数据指针
  * @retval 0: 成功, 1: 失败
  */
uint8_t BH1750_ReadData(uint16_t *data)
{
    uint8_t buffer[2];
    esp_err_t ret;

    ret = i2c_master_receive(bh1750_dev_handle, buffer, sizeof(buffer), -1);
    if (ret != ESP_OK) {
        return 1;  // 读取失败
    }
    // 组合数据 (高字节在前)
    *data = ((uint16_t)buffer[0] << 8) | buffer[1];
    return 0;
}

/**
  * @brief  BH1750初始化
  * @retval None
  */
void BH1750_Init(void)
{
    // 发送上电命令（唤醒设备）
    BH1750_PowerOn();
    vTaskDelay(pdMS_TO_TICKS(10));
    
    // 发送重置命令
    BH1750_Reset();
    vTaskDelay(pdMS_TO_TICKS(10));
    
    // 设置为连续高分辨率模式
    BH1750_SetMode(BH1750_CONT_H_MODE);
    vTaskDelay(pdMS_TO_TICKS(180));  // 等待首次测量完成
    
    is_initialized = 1;
}

/**
  * @brief  获取光照强度(Lux)
  * @retval 光照强度值(Lux)，-1.0表示读取失败
  */
float BH1750_GetLux(void)
{
    uint16_t raw_data;
    float lux;
    
    // 如果设备未初始化，先初始化
    if(!is_initialized) {
        BH1750_Init();
    }
    
    // 检查是否处于连续测量模式
    if(current_mode != BH1750_CONT_H_MODE && 
       current_mode != BH1750_CONT_H_MODE2 && 
       current_mode != BH1750_CONT_L_MODE) {
        // 如果不是连续模式，需要重新启动测量
        BH1750_SetMode(BH1750_CONT_H_MODE);
    }
    
    // 读取数据
    if(BH1750_ReadData(&raw_data) == 0) {
        // 根据BH1750手册转换公式: lux = raw_data / 1.2
        lux = (float)raw_data / 1.2f;
        
        // 验证数据是否有效（BH1750的最大测量值为65535/1.2≈54612.5 Lux）
        if(raw_data == 0x0000 || raw_data == 0xFFFF) {
            BH1750_Init();  // 重新初始化
            return -1.0f;
        }
        
        return lux;
    }
    
    return -1.0f;  // 返回错误值
}

/**
  * @brief  设置BH1750测量模式
  * @param  mode: 测量模式
  * @retval None
  */
void BH1750_SetMode(uint8_t mode)
{
    BH1750_WriteCmd(mode);
    current_mode = mode;  // 记录当前模式
    
    // 根据模式设置延时等待测量完成
    switch(mode) {
        case BH1750_CONT_H_MODE:
        case BH1750_ONE_H_MODE:
            vTaskDelay(pdMS_TO_TICKS(180));  // 高分辨率模式需要180ms
            break;
            
        case BH1750_CONT_H_MODE2:
        case BH1750_ONE_H_MODE2:
            vTaskDelay(pdMS_TO_TICKS(120));  // 高分辨率模式2需要120ms
            break;
            
        case BH1750_CONT_L_MODE:
        case BH1750_ONE_L_MODE:
            vTaskDelay(pdMS_TO_TICKS(16));   // 低分辨率模式需要16ms
            break;
            
        default:
            vTaskDelay(pdMS_TO_TICKS(10));
            break;
    }
}

/**
  * @brief  BH1750重置
  * @retval None
  */
void BH1750_Reset(void)
{
    BH1750_WriteCmd(BH1750_RESET);
}

/**
  * @brief  BH1750进入休眠模式
  * @retval None
  */
void BH1750_PowerDown(void)
{
    BH1750_WriteCmd(BH1750_POWER_DOWN);
    current_mode = 0;  // 清除当前模式
}

/**
  * @brief  BH1750唤醒
  * @retval None
  */
void BH1750_PowerOn(void)
{
    BH1750_WriteCmd(BH1750_POWER_ON);
}

/**
  * @brief  单次测量模式读取光照强度
  * @param  mode: 单次测量模式
  * @retval 光照强度值(Lux)，-1.0表示读取失败
  */
float BH1750_ReadOnce(uint8_t mode)
{
    uint16_t raw_data;
    float lux;
    
    // 确保设备已上电
    if(current_mode == 0) {
        BH1750_PowerOn();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    
    // 发送单次测量命令
    BH1750_WriteCmd(mode);
    
    // 等待测量完成
    switch(mode) {
        case BH1750_ONE_H_MODE:
            vTaskDelay(pdMS_TO_TICKS(180));
            break;
        case BH1750_ONE_H_MODE2:
            vTaskDelay(pdMS_TO_TICKS(120));
            break;
        case BH1750_ONE_L_MODE:
            vTaskDelay(pdMS_TO_TICKS(16));
            break;
        default:
            vTaskDelay(pdMS_TO_TICKS(180));
            break;
    }
    
    // 读取数据
    if(BH1750_ReadData(&raw_data) == 0) {
        lux = (float)raw_data / 1.2f;
        return lux;
    }
    
    return -1.0f;
}

/**
  * @brief  重新启动BH1750（解决数据不更新的问题）
  * @retval None
  */
void BH1750_Restart(void)
{
    // 先断电
    BH1750_PowerDown();
    vTaskDelay(pdMS_TO_TICKS(10));
    
    // 再重新初始化
    BH1750_Init();
}

void TEST(void){
    uint16_t lux;
    BH1750_Init();
    lux = BH1750_GetLux(); 
}
