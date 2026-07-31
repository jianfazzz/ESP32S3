#include"keypad.h"


void app_main(void)
{
    app_init();

    while(1){
        KEY_scan(keyNum);
        KeyScanHandle();
        if(setn == 1){
            Display();
            CotorFun();					
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

