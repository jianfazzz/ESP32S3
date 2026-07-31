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
                if(setn <= 3){
                    OLED_CLS();
                    DISplaysetvalue();
                }else{
                    setn = 1;
                    OLED_CLS(); 
                }
            } else if (i == 2) {
                if(setn == 1 && angle < 180)angle+=1;		
                if(setn == 2 && tempMax < 99)tempMax++;		
                if(setn == 3 && humMax < 99)humMax++;			
			    DISplaysetvalue();   //显示设置值
            } else if (i == 3) {
                if(setn == 1 && angle > 0)angle-=1;
                if(setn == 2 && tempMax > 0)tempMax--;
                if(setn == 3 && humMax > 0)humMax--;
                DISplaysetvalue();
            } else if (i == 4) {

            }            
        }
    } 
}

void Display(void){

    lux = BH1750_GetLux(); 
    ADXL345_ReadAccel(&x, &y, &z); 

    if (RC522_ReadCard(uid) == MI_OK) {
        snprintf(display, sizeof(display), "%02X%02X%02X%02X",
                uid[0], uid[1], uid[2], uid[3]);
        OLED_ShowStr(0, 4,display, 2, 0);
    }
    sprintf(display,"%02d:%02d:%02d ",shi,fen,miao);
    OLED_ShowStr(0, 0, display , 2,0);
	
    sprintf(display,"%.1f %.1f %.1f ",x,y,z);
    OLED_ShowStr(0, 2, display , 2,0);

    sprintf(display,"L:%d ",lux);
    LCD_ShowString(0, 0, (uint8_t *)display, BLACK, YELLOW, 32, 0);
  
    sprintf(display,"a:%03d ",angle);
    LCD_ShowString(0, 32, (uint8_t *)display, BLACK, YELLOW, 32, 0);   
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
}

void CotorFun(void){	
    sg90_set_angle(angle);
    if(temperature>tempMax){
        BEEP_DEVICE(1);
    }else{
        BEEP_DEVICE(0);
    }
}

