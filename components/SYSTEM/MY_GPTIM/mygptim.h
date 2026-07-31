#ifndef __MYGPTIM_H_
#define __MYGPTIM_H_

#include <stdio.h>

extern uint8_t shi,fen,miao;
extern int PWM_flag;
extern volatile uint16_t pwm_tick;     // 用于软件 PWM 的周期计数器



#define TIM1_frequency  1000000  // 1MHz，每 1us 计数加 1
#define TIM1_target     1000000  // 1 秒中断一次

#define TIM2_frequency  1000000  
#define TIM2_target     1000000       

#define TIM3_frequency  1000000 
#define TIM3_target     1000000  

#define TIM4_frequency  1000000  
#define TIM4_target     1000000  


void GPTIM_init(void);
void GPTIM1_init(void);
void GPTIM2_init(void);
void GPTIM3_init(void);
void GPTIM4_init(void);

#endif

