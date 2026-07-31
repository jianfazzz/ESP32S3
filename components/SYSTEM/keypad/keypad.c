#include "keypad.h"




SemaphoreHandle_t client_mutex;   
char display[32];
uint8_t keyNum[5]= {0}, setn = 1, i=0;

uint8_t temperature,hum,smoke,light,noise,rain;
uint16_t CO2,lux;
float x, y, z;

uint8_t tempMax=35,humMax=70;
uint8_t angle=0;

uint8_t uid[4];
uint8_t finger_Register = 0,finger_Delete=0;//指纹标志
uint16_t finger_count = 0;  // 已录入指纹数，用于分配下一个 page_id
uint16_t page, score;

void app_init(void){

    UART_init();
    IIC_init();
    ADC_init();
    GPTIM_init();   
    SPI_init();

    WIFI_AP_init();


    OLED_Init();
	OLED_CLS();
    LCD_Init();
    LCD_CLS();
    KEY_init();
    // GPIO_init();
    BH1750_Init();
    ADXL345_Init();
    SG90_init();
    RC522_Init();
    AS608_Init();
    JW01_Init();


 
    // 创建互斥量
    client_mutex = xSemaphoreCreateMutex();
    // 创建 TCP 服务器任务
    xTaskCreate(tcp_server_task, "tcp_server", 4096, NULL, 5, NULL);
    // 创建数据上报任务
    xTaskCreate(report_task, "report_task", 4096, NULL, 4, NULL);
}

void KeyScanHandle(void){
    for (int i = 0; i < 5; i++){
        if (keyNum[i] == 1) {
            if (i == 1) {
                setn ++;
                if(setn <= 4){
                    OLED_CLS();
                    LCD_CLS();
                    DISplaysetvalue();
                }else{
                    setn = 1;
                    OLED_CLS(); 
                    LCD_CLS();
                }
            } else if (i == 2) {
                if(setn == 1 && angle < 180)angle+=1;		
                if(setn == 2 && tempMax < 99)tempMax++;		
                if(setn == 3 && humMax < 99)humMax++;
                if(setn == 4 ){
                    finger_Register=1;
                    finger_Delete=0;	
                }		
			    DISplaysetvalue();   //显示设置值
            } else if (i == 3) {
                if(setn == 1 && angle > 0)angle-=1;
                if(setn == 2 && tempMax > 0)tempMax--;
                if(setn == 3 && humMax > 0)humMax--;
                if(setn == 4 ){
                    finger_Register=0;
                    finger_Delete=1;	
                }	
                DISplaysetvalue();
            } else if (i == 4) {

            }            
        }
    } 
}

void Display(void){
    if(setn == 1 ){
        lux = BH1750_GetLux(); 
        ADXL345_ReadAccel(&x, &y, &z); 
        CO2 = JW01_GetCO2(); 

        sprintf(display,"CO2:%04uppm",CO2);
        OLED_ShowStr(0, 0, display , 2,0);

        if (RC522_ReadCard(uid) == MI_OK) {
            snprintf(display, sizeof(display), "%02X%02X%02X%02X",
                    uid[0], uid[1], uid[2], uid[3]);
            OLED_ShowStr(0, 2,display, 2, 0);
        }




        if(AS608_GetFingerPressed()==1){
            AS608_GenImg();
            AS608_Img2Tz(1);   


            uint16_t page_id, score;
            if (AS608_Search(1, 0, 300, &page_id, &score) == AS608_OK) {
                LCD_ShowString(16, 96, (uint8_t *)"FIND!   ", BLACK, YELLOW, 32, 0);
            } else {
                LCD_ShowString(16, 96, (uint8_t *)"NO FIND!", BLACK, YELLOW, 32, 0);
            }
        }    

        sprintf(display,"%02d:%02d:%02d ",shi,fen,miao);
        OLED_ShowStr(0, 4, display , 2,0);
        
        sprintf(display,"%.1f %.1f %.1f ",x,y,z);
        OLED_ShowStr(0, 6, display , 2,0);

        sprintf(display,"L:%d ",lux);
        LCD_ShowString(0, 0, (uint8_t *)display, BLACK, YELLOW, 32, 0);
    
        sprintf(display,"a:%03d ",angle);
        LCD_ShowString(0, 32, (uint8_t *)display, BLACK, YELLOW, 32, 0);
    }
   
}

void DISplaysetvalue(void){		
    if(setn == 2) 
    {
        for(i=0;i<2;i++)OLED_ShowCN(i*16+32,0,i+6,0);//显示中文：设置
        for(i=0;i<2;i++)OLED_ShowCN(i*16+64,0,i+0,0);//显示中文：温度
        sprintf(display,"%02d",tempMax); 
        OLED_ShowStr(56, 3, display , 2,0);	
        OLED_ShowCentigrade(72,3);
    } 
    if(setn == 3) 
    {
        for(i=0;i<2;i++)OLED_ShowCN(i*16+32,0,i+6,0);//显示中文：设置
        for(i=0;i<2;i++)OLED_ShowCN(i*16+64,0,i+2,0);//显示中文：湿度
        sprintf(display,"%02d%%",humMax); 
        OLED_ShowStr(56, 3, display , 2,0);	
    }  
 	if(setn == 4)
	{
		if(finger_Register)
        {
	        LCD_ShowString(16, 32, (uint8_t *)"  ADD ", BLACK, YELLOW, 32, 1);
		}else{
			LCD_ShowString(16, 32, (uint8_t *)"  ADD ", BLACK, YELLOW, 32, 0);	
		}
		if(finger_Delete)
        {
			LCD_ShowString(16, 64, (uint8_t *)"DELETE", BLACK, YELLOW, 32, 1);			
		}else{
			LCD_ShowString(16, 64, (uint8_t *)"DELETE", BLACK, YELLOW, 32, 0);	
        }
    }	
		  
}

void CotorFun(void){
    if(setn == 1 ){
        sg90_set_angle(angle);

        if(temperature>tempMax){
            BEEP_DEVICE(1);
        }else{
            BEEP_DEVICE(0);
        }

    }else{
        if(finger_Register){
            if(AS608_Add_finger(finger_count) == AS608_OK){
                finger_count++;
            }
            finger_Register=0;
        }
        if(finger_Delete){
            AS608_Empty();
            finger_count = 0;
            finger_Delete=0;
        }
    }
}

// 录入指纹：需要按两次手指，返回 AS608_OK 表示成功
uint8_t AS608_Add_finger(uint16_t page_id){
    // 第一次采集：轮询 GenImg 直到检测到手指
    LCD_ShowString(0, 96, (uint8_t *)"PRESS 1  ", BLACK, YELLOW, 32, 0);
    while(AS608_GenImg() != AS608_OK) vTaskDelay(pdMS_TO_TICKS(200));
    if(AS608_Img2Tz(1) != AS608_OK) return AS608_ERR;

    // 等待手指松开：GenImg 返回 NOFINGER 说明已松开
    LCD_ShowString(0, 96, (uint8_t *)"LIFT     ", BLACK, YELLOW, 32, 0);
    while(AS608_GenImg() != AS608_NOFINGER) vTaskDelay(pdMS_TO_TICKS(200));

    // 第二次采集
    LCD_ShowString(0, 96, (uint8_t *)"PRESS 2  ", BLACK, YELLOW, 32, 0);
    while(AS608_GenImg() != AS608_OK) vTaskDelay(pdMS_TO_TICKS(200));
    if(AS608_Img2Tz(2) != AS608_OK) return AS608_ERR;

    // 合成并存储
    if(AS608_RegModel() != AS608_OK) return AS608_ERR;
    if(AS608_Store(1, page_id) != AS608_OK) return AS608_ERR;

    LCD_ShowString(0, 96, (uint8_t *)"OK!      ", BLACK, YELLOW, 32, 0);
    return AS608_OK;
}

