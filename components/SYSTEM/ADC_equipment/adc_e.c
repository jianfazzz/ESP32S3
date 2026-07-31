#include "adc_e.h"
#include "keypad.h"

 //------------------ MQ系列 ------------------
#if MQ

float GET_MQ(void){
	float adc1;
	float result;
    adc1 = ADC_read_5()*99/4096;;  
    result = adc1 >= 99? 99: adc1;

    return result;	
}
#endif

 //------------------ 分贝 ------------------
#if NOISE

float noi_mapADCtoPercent(uint16_t adcValue) {
    // 根据实际实验数据修改这些数组
		const uint16_t adcPoints[] = { 0, 1500, 1800, 2000, 2300, 2500, 2800, 3100, 3500, 3800, 4095 };
		const float percentPoints[] = { 0, 8, 20, 30, 40, 50, 60, 70, 80, 90, 100 };
    const int numPoints = sizeof(adcPoints)/sizeof(adcPoints[0]);

    // 边界处理
    if (adcValue <= adcPoints[0]) return percentPoints[0];
    if (adcValue >= adcPoints[numPoints-1]) return percentPoints[numPoints-1];

    // 查找所在的区间
    for (int i = 0; i < numPoints-1; i++) {
        if (adcValue <= adcPoints[i+1]) {
            // 线性插值
            float ratio = (float)(adcValue - adcPoints[i]) / (adcPoints[i+1] - adcPoints[i]);
            return percentPoints[i] + ratio * (percentPoints[i+1] - percentPoints[i]);
        }
    }
    return 100.0f; 
}
float GET_NOISE(void) {
    uint16_t rawAdc = ADC_read_4();         // 读取原始 ADC
    return rawAdc;
    // return noi_mapADCtoPercent(rawAdc);     // 转为百分比
}

#endif

 //------------------ 光线 ------------------
#if LIGHT
float Get_LIGHT(void){
	float adc1;
	uint8_t result;
    adc1 = ADC_read_4()*99/4096; 
    result = adc1 >= 99? 99: adc1;

    return result;
}	
#endif

 //------------------ PM2.5 ------------------
#if PM
float adc,adc1;
float PM_count=0;
float PM25_value;

void PM25_LED_init(void){
    gpio_config_t PM25_LED_CONFIG={
        .intr_type = GPIO_INTR_DISABLE ,
        .mode =GPIO_MODE_INPUT_OUTPUT ,
        .pin_bit_mask = 1ull << 16 ,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    gpio_config(&PM25_LED_CONFIG);
}

float Get_PM(void){
	gpio_set_level(16, 0);                
	esp_rom_delay_us(280);               
	adc = ADC_read_7()*3.3/4095.0;	
	PM_count++;
	esp_rom_delay_us(40);              //延时40us
	gpio_set_level(16, 1);             //PM2.5 LED灯拉高关闭
	adc1=adc1+adc;

	if(PM_count >= 5){
		PM_count = 0;                 //计数次数清零	 		 
		adc1 = adc1 / 5+0.8;          //取5次平均值	 	  
		PM25_value = (adc1*0.17-0.1)*1000;     //计算PM2.5值
		adc1 = 0;               
	}
	return PM25_value;              //返回PM2.5值	
}
#endif 

 //------------------ 雨量 ------------------
#if RAIN

#endif

 //------------------ 风速 ------------------
#if WIND_SPEED

#endif

 //------------------ 风向 ------------------
#if WIND_DIRECTION

#endif       

 //------------------ 水位 ------------------
#if WATER_LEVEL

#endif

 //------------------ 土壤湿度 ------------------
#if SOIL_HUM

#endif

 //------------------ 酸碱度 ------------------
#if PH

#endif       

 

