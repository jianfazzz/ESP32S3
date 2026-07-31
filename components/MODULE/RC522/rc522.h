#ifndef __RC522_H_
#define __RC522_H_

#include <stdint.h>

#define RC522_RST     6
#define RC522_SDA     5

// MFRC522 寄存器定义
#define MFRC522_REG_COMMAND         0x01
#define MFRC522_REG_COM_IEN         0x02
#define MFRC522_REG_DIV_IEN         0x03
#define MFRC522_REG_COM_IRQ         0x04
#define MFRC522_REG_DIV_IRQ         0x05
#define MFRC522_REG_ERROR           0x06
#define MFRC522_REG_STATUS1         0x07
#define MFRC522_REG_STATUS2         0x08
#define MFRC522_REG_FIFO_DATA       0x09
#define MFRC522_REG_FIFO_LEVEL      0x0A
#define MFRC522_REG_CONTROL         0x0C
#define MFRC522_REG_BIT_FRAMING     0x0D
#define MFRC522_REG_MODE            0x11
#define MFRC522_REG_TX_MODE         0x12
#define MFRC522_REG_RX_MODE         0x13
#define MFRC522_REG_TX_CONTROL      0x14
#define MFRC522_REG_TX_AUTO         0x15
#define MFRC522_REG_CRC_RESULT_H    0x21
#define MFRC522_REG_CRC_RESULT_L    0x22
#define MFRC522_REG_T_MODE          0x2A
#define MFRC522_REG_T_PRESCALER     0x2B
#define MFRC522_REG_T_RELOAD_H      0x2C
#define MFRC522_REG_T_RELOAD_L      0x2D
#define MFRC522_REG_RF_CFG          0x26

// 命令
#define MFRC522_CMD_IDLE            0x00
#define MFRC522_CMD_MEM             0x01
#define MFRC522_CMD_GENID           0x02
#define MFRC522_CMD_CALCCRC         0x03
#define MFRC522_CMD_TRANSMIT        0x04
#define MFRC522_CMD_NOCMDCH         0x07
#define MFRC522_CMD_RECEIVE         0x08
#define MFRC522_CMD_TRANSCEIVE      0x0C
#define MFRC522_CMD_AUTHENT         0x0E
#define MFRC522_CMD_SOFTRESET       0x0F

// Mifare 卡片命令
#define MF1_REQIDL                  0x26
#define MF1_REQALL                  0x52
#define MF1_ANTICOLL                0x93
#define MF1_SELECTTAG               0x93
#define MF1_AUTHENT1A               0x60
#define MF1_AUTHENT1B               0x61
#define MF1_READ                    0x30
#define MF1_WRITE                   0xA0
#define MF1_DESELECT                0xC2

// 状态码
#define MI_OK                       0
#define MI_NOTAGERR                 1
#define MI_ERR                      2

// API 声明
void RC522_Init(void);
uint8_t RC522_ReadCard(uint8_t *uid);

#endif
