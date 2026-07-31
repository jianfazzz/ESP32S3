#include"mygptim.h"
#include "keypad.h"

uint8_t shi,fen,miao;
volatile uint16_t pwm_tick = 0;   // 软件 PWM 计数值

gptimer_handle_t GPTIMER1_HANDLE,GPTIMER2_HANDLE,GPTIMER3_HANDLE,GPTIMER4_HANDLE;


bool IRAM_ATTR TimerCallback1(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx){
    miao++;
    if(miao==60)
    {
        miao=0;
        fen++;
    }
    if(fen==60)
    {
        shi++;
        fen=0;
    }
    return  0 ;

}
bool IRAM_ATTR TimerCallback2(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx){
    
    return 0;               
}
bool IRAM_ATTR TimerCallback3(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx){

    return  0 ;
}
bool IRAM_ATTR TimerCallback4(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx){

    return  0 ;
} 

void GPTIM_init(void){
    #if TIM1
        GPTIM1_init();
    #endif
    #if TIM2
        GPTIM2_init();
    #endif
    #if TIM3
        GPTIM3_init();
    #endif
    #if TIM4
        GPTIM4_init();
    #endif
}

void GPTIM1_init(void){

    gptimer_config_t GPTIMER_CFG={
        .clk_src = GPTIMER_CLK_SRC_DEFAULT ,
        .direction = GPTIMER_COUNT_UP ,
        .flags.intr_shared = 0 ,
        .intr_priority = 0 ,
        .resolution_hz = TIM1_frequency  ,       
    };
    gptimer_new_timer(&GPTIMER_CFG, &GPTIMER1_HANDLE);


    gptimer_alarm_config_t GPTIMER_ALARM_CFG = {
        .alarm_count = TIM1_target ,
        .flags.auto_reload_on_alarm = 1 ,
        .reload_count = 0 ,        
    };
    gptimer_set_alarm_action(GPTIMER1_HANDLE, &GPTIMER_ALARM_CFG);

    gptimer_event_callbacks_t GPTIMER_EVENT_CALLBACKS = {
        .on_alarm = TimerCallback1 ,
    };
    gptimer_register_event_callbacks(GPTIMER1_HANDLE, &GPTIMER_EVENT_CALLBACKS, NULL);

    gptimer_enable(GPTIMER1_HANDLE);

    gptimer_start(GPTIMER1_HANDLE);

}

void GPTIM2_init(void){

    gptimer_config_t GPTIMER_CFG={
        .clk_src = GPTIMER_CLK_SRC_DEFAULT ,
        .direction = GPTIMER_COUNT_UP ,
        .flags.intr_shared = 0 ,
        .intr_priority = 3 ,
        .resolution_hz = TIM2_frequency  ,       
    };
    gptimer_new_timer(&GPTIMER_CFG, &GPTIMER2_HANDLE);


    gptimer_alarm_config_t GPTIMER_ALARM_CFG = {
        .alarm_count = TIM2_target ,
        .flags.auto_reload_on_alarm = 1 ,
        .reload_count = 0 ,        
    };
    gptimer_set_alarm_action(GPTIMER2_HANDLE, &GPTIMER_ALARM_CFG);

    gptimer_event_callbacks_t GPTIMER_EVENT_CALLBACKS = {
        .on_alarm = TimerCallback2 ,
    };
    gptimer_register_event_callbacks(GPTIMER2_HANDLE, &GPTIMER_EVENT_CALLBACKS, NULL);

    gptimer_enable(GPTIMER2_HANDLE);

    gptimer_start(GPTIMER2_HANDLE);

}

void GPTIM3_init(void){

    gptimer_config_t GPTIMER_CFG={
        .clk_src = GPTIMER_CLK_SRC_DEFAULT ,
        .direction = GPTIMER_COUNT_UP ,
        .flags.intr_shared = 0 ,
        .intr_priority = 0 ,
        .resolution_hz = TIM3_frequency  ,       
    };
    gptimer_new_timer(&GPTIMER_CFG, &GPTIMER3_HANDLE);


    gptimer_alarm_config_t GPTIMER_ALARM_CFG = {
        .alarm_count = TIM3_target ,
        .flags.auto_reload_on_alarm = 1 ,
        .reload_count = 0 ,        
    };
    gptimer_set_alarm_action(GPTIMER3_HANDLE, &GPTIMER_ALARM_CFG);

    gptimer_event_callbacks_t GPTIMER_EVENT_CALLBACKS = {
        .on_alarm = TimerCallback3 ,
    };
    gptimer_register_event_callbacks(GPTIMER3_HANDLE, &GPTIMER_EVENT_CALLBACKS, NULL);

    gptimer_enable(GPTIMER3_HANDLE);

    gptimer_start(GPTIMER3_HANDLE);

}

void GPTIM4_init(void){

    gptimer_config_t GPTIMER_CFG={
        .clk_src = GPTIMER_CLK_SRC_DEFAULT ,
        .direction = GPTIMER_COUNT_UP ,
        .flags.intr_shared = 0 ,
        .intr_priority = 0 ,
        .resolution_hz = TIM4_frequency  ,       
    };
    gptimer_new_timer(&GPTIMER_CFG, &GPTIMER4_HANDLE);


    gptimer_alarm_config_t GPTIMER_ALARM_CFG = {
        .alarm_count = TIM4_target ,
        .flags.auto_reload_on_alarm = 1 ,
        .reload_count = 0 ,        
    };
    gptimer_set_alarm_action(GPTIMER4_HANDLE, &GPTIMER_ALARM_CFG);

    gptimer_event_callbacks_t GPTIMER_EVENT_CALLBACKS = {
        .on_alarm = TimerCallback4 ,
    };
    gptimer_register_event_callbacks(GPTIMER4_HANDLE, &GPTIMER_EVENT_CALLBACKS, NULL);

    gptimer_enable(GPTIMER4_HANDLE);

    gptimer_start(GPTIMER4_HANDLE);

}


