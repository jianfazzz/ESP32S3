#include"keypad.h"


void app_main(void)
{
    app_init();

    while(1){
        KEY_scan(keyNum);
        KeyScanHandle();
        Display();
        CotorFun();					
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

