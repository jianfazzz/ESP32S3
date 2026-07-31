#include "myuart.h"
#include "keypad.h"

void UART_init(void) {
    #if JW01
    {
        // JW01 使用 UART1
        uart_config_t cfg = {
            .baud_rate = JW01_BAUDRATE,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
            .source_clk = UART_SCLK_APB,
        };
        uart_param_config(UART_NUM_1, &cfg);
        uart_set_pin(UART_NUM_1, UART1_TX, UART1_RX, -1, -1);
        uart_driver_install(UART_NUM_1, 2048, 2048, 0, NULL, 0);
    }
    #endif

    #if AS608
    {
        // AS608 使用 UART2
        uart_config_t cfg = {
            .baud_rate = AS608_BAUDRATE,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
            .source_clk = UART_SCLK_APB,
        };
        uart_param_config(UART_NUM_2, &cfg);
        uart_set_pin(UART_NUM_2, UART2_TX, UART2_RX, -1, -1);
        uart_driver_install(UART_NUM_2, 4096, 4096, 0, NULL, 0);
    }
    #endif
}
