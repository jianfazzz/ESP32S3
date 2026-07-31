#include"myadc.h"
#include "keypad.h"

adc_oneshot_unit_handle_t adc_handle;

void ADC_init(void){
    adc_oneshot_unit_init_cfg_t adc_strcture = {
        .clk_src = SOC_ADC_RTC_CTRL_SUPPORTED ,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
        .unit_id = ADC_UNIT_1 ,
    };
    adc_oneshot_new_unit(&adc_strcture, &adc_handle);


    adc_oneshot_chan_cfg_t adc_channle_CONFIG = {
        .atten = ADC_ATTEN_DB_12 ,
        .bitwidth = ADC_BITWIDTH_12 ,
    };

    
    #if ADC1_CH0
    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_0, &adc_channle_CONFIG);
    #endif
    #if ADC1_CH1
    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_1, &adc_channle_CONFIG);
    #endif
    #if ADC1_CH2
    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_2, &adc_channle_CONFIG);
    #endif
    #if ADC1_CH3
    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_3, &adc_channle_CONFIG);
    #endif
    #if ADC1_CH4
    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_4, &adc_channle_CONFIG);
    #endif
    #if ADC1_CH5
    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_5, &adc_channle_CONFIG);
    #endif       
    #if ADC1_CH6
    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_6, &adc_channle_CONFIG);
    #endif
    #if ADC1_CH7
    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_7, &adc_channle_CONFIG);
    #endif
    #if ADC1_CH8
    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_8, &adc_channle_CONFIG);
    #endif       
    #if ADC1_CH9
    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_9, &adc_channle_CONFIG);
    #endif
}

int ADC_read_39(void){
    int value;
    adc_oneshot_read(adc_handle, ADC_CHANNEL_0, &value);
    return value;
}
int ADC_read_38(void){
    int value;
    adc_oneshot_read(adc_handle, ADC_CHANNEL_1, &value);
    return value;
}
int ADC_read_15(void){
    int value;
    adc_oneshot_read(adc_handle, ADC_CHANNEL_2, &value);
    return value;
}
int ADC_read_4(void){
    int value;
    adc_oneshot_read(adc_handle, ADC_CHANNEL_3, &value);
    return value;
}
int ADC_read_5(void){
    int value;
    adc_oneshot_read(adc_handle, ADC_CHANNEL_4, &value);
    return value;
}
int ADC_read_6(void){
    int value;
    adc_oneshot_read(adc_handle, ADC_CHANNEL_5, &value);
    return value;
}
int ADC_read_7(void){
    int value;
    adc_oneshot_read(adc_handle, ADC_CHANNEL_6, &value);
    return value;
}
int ADC_read_8(void){
    int value;
    adc_oneshot_read(adc_handle, ADC_CHANNEL_7, &value);
    return value;
}
int ADC_read_9(void){
    int value;
    adc_oneshot_read(adc_handle, ADC_CHANNEL_8, &value);
    return value;
}
int ADC_read_10(void){
    int value;
    adc_oneshot_read(adc_handle, ADC_CHANNEL_9, &value);
    return value;
}


