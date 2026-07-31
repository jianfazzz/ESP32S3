#ifndef __MYUART_H_
#define __MYUART_H_

#include <stdint.h>


/* ---------- UART 引脚定义 ---------- */
#define UART1_TX 21
#define UART1_RX 47
#define UART2_TX 10
#define UART2_RX 9

/* ---------- 模块波特率 ---------- */
#define AS608_BAUDRATE   57600


/* ---------- 初始化函数 ---------- */
void UART_init(void);


#endif
