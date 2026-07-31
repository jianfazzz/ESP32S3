#ifndef __MYUART_H_
#define __MYUART_H_

#include <stdint.h>


/* ---------- UART 引脚定义---------- */
#define UART1_TX 21
#define UART1_RX 47
#define UART2_TX 10
#define UART2_RX 9




/* ---------- 初始化函数 ---------- */
void UART_init(void);
void UART1_init(void);
void UART2_init(void);


#endif