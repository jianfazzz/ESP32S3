#ifndef __MYADC_H_
#define __MYADC_H_

#define ADC1_CH0     0    // 39
#define ADC1_CH1     0    // 38
#define ADC1_CH2     0    // 15
#define ADC1_CH3     1    // 4
#define ADC1_CH4     0    // 5
#define ADC1_CH5     0    // 6
#define ADC1_CH6     0    // 7
#define ADC1_CH7     0    // 8
#define ADC1_CH8     0    // 9
#define ADC1_CH9     0    // 10





void ADC_init(void);

int ADC_read_39(void);
int ADC_read_38(void);
int ADC_read_15(void);
int ADC_read_4(void);
int ADC_read_5(void);
int ADC_read_6(void);
int ADC_read_7(void);
int ADC_read_8(void);
int ADC_read_9(void);
int ADC_read_10(void);



#endif

