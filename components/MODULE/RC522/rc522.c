#include "rc522.h"
#include "myspi.h"
#include "keypad.h"

static const char *TAG = "RC522";

/* ---------- 底层 SPI 寄存器读写 ---------- */

uint8_t RC522_ReadReg(uint8_t addr) {
    uint8_t tx[2] = { (uint8_t)((addr << 1) | 0x80), 0x00 };
    uint8_t rx[2] = { 0 };

    spi_transaction_t t = {
        .length = 16,
        .tx_buffer = tx,
        .rx_buffer = rx,
    };
    spi_device_polling_transmit(rc522_dev_handle, &t);
    return rx[1];
}

void RC522_WriteReg(uint8_t addr, uint8_t val) {
    uint8_t tx[2] = { (uint8_t)(addr << 1), val };

    spi_transaction_t t = {
        .length = 16,
        .tx_buffer = tx,
    };
    spi_device_polling_transmit(rc522_dev_handle, &t);
}

static void RC522_SetBitMask(uint8_t addr, uint8_t mask) {
    RC522_WriteReg(addr, RC522_ReadReg(addr) | mask);
}

static void RC522_ClearBitMask(uint8_t addr, uint8_t mask) {
    RC522_WriteReg(addr, RC522_ReadReg(addr) & ~mask);
}

/* ---------- 复位与天线 ---------- */

static void RC522_Reset(void) {
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << RC522_RST),
        .pull_down_en = 0,
        .pull_up_en = 0,
    };
    gpio_config(&io_conf);

    gpio_set_level(RC522_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(5));
    gpio_set_level(RC522_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(RC522_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(5));
}

static void RC522_AntennaOn(void) {
    uint8_t val = RC522_ReadReg(MFRC522_REG_TX_CONTROL);
    if ((val & 0x03) != 0x03) {
        RC522_SetBitMask(MFRC522_REG_TX_CONTROL, 0x03);
    }
}

/* ---------- 初始化 ---------- */

void RC522_Init(void) {
    RC522_Reset();

    RC522_WriteReg(MFRC522_REG_COMMAND, MFRC522_CMD_SOFTRESET);
    vTaskDelay(pdMS_TO_TICKS(50));

    RC522_WriteReg(MFRC522_REG_T_MODE, 0x8D);
    RC522_WriteReg(MFRC522_REG_T_PRESCALER, 0x3E);
    RC522_WriteReg(MFRC522_REG_T_RELOAD_L, 30);
    RC522_WriteReg(MFRC522_REG_T_RELOAD_H, 0);

    RC522_WriteReg(MFRC522_REG_TX_AUTO, 0x40);
    RC522_WriteReg(MFRC522_REG_MODE, 0x3D);

    RC522_WriteReg(MFRC522_REG_TX_MODE, 0x00);
    RC522_WriteReg(MFRC522_REG_RX_MODE, 0x00);

    RC522_WriteReg(MFRC522_REG_RF_CFG, 0x70);

    RC522_AntennaOn();
    ESP_LOGI(TAG, "RC522 init done");
}

/* ---------- 与卡片通信 ---------- */

static uint8_t RC522_ToCard(uint8_t cmd, uint8_t *send, uint8_t send_len,
                            uint8_t *recv, uint16_t *recv_len) {
    uint8_t status = MI_ERR;
    uint8_t irq_en = 0x00;
    uint8_t wait_irq = 0x00;

    if (cmd == MFRC522_CMD_TRANSCEIVE) {
        irq_en = 0x77;
        wait_irq = 0x30;
    } else if (cmd == MFRC522_CMD_AUTHENT) {
        irq_en = 0x12;
        wait_irq = 0x10;
    }

    RC522_WriteReg(MFRC522_REG_COM_IEN, irq_en | 0x80);
    RC522_ClearBitMask(MFRC522_REG_COM_IRQ, 0x80);
    RC522_SetBitMask(MFRC522_REG_FIFO_LEVEL, 0x80);
    RC522_WriteReg(MFRC522_REG_COMMAND, MFRC522_CMD_IDLE);

    for (uint8_t i = 0; i < send_len; i++) {
        RC522_WriteReg(MFRC522_REG_FIFO_DATA, send[i]);
    }

    RC522_WriteReg(MFRC522_REG_COMMAND, cmd);
    if (cmd == MFRC522_CMD_TRANSCEIVE) {
        RC522_SetBitMask(MFRC522_REG_BIT_FRAMING, 0x80);
    }

    uint8_t n = 200;
    do {
        vTaskDelay(pdMS_TO_TICKS(1));
        n--;
    } while (n && !(RC522_ReadReg(MFRC522_REG_COM_IRQ) & wait_irq));

    RC522_ClearBitMask(MFRC522_REG_BIT_FRAMING, 0x80);

    if (n == 0) {
        return MI_ERR;
    }

    if (!(RC522_ReadReg(MFRC522_REG_ERROR) & 0x1B)) {
        status = MI_OK;
        if (cmd == MFRC522_CMD_TRANSCEIVE) {
            uint8_t len = RC522_ReadReg(MFRC522_REG_FIFO_LEVEL);
            uint8_t last_bits = RC522_ReadReg(MFRC522_REG_CONTROL) & 0x07;
            *recv_len = (last_bits) ? (len - 1) * 8 + last_bits : len * 8;

            if (len == 0) len = 1;
            if (len > 16) len = 16;

            for (uint8_t i = 0; i < len; i++) {
                recv[i] = RC522_ReadReg(MFRC522_REG_FIFO_DATA);
            }
        }
    }

    return status;
}

/* ---------- 高层读卡 ---------- */

static uint8_t RC522_Request(uint8_t req_mode) {
    uint8_t status;
    uint8_t buf[16];
    uint16_t len;

    RC522_WriteReg(MFRC522_REG_BIT_FRAMING, 0x07);
    buf[0] = req_mode;
    status = RC522_ToCard(MFRC522_CMD_TRANSCEIVE, buf, 1, buf, &len);

    if ((status == MI_OK) && (len == 0x10)) {
        return MI_OK;
    }
    return MI_ERR;
}

static uint8_t RC522_Anticoll(uint8_t *uid) {
    uint8_t status;
    uint8_t buf[16];
    uint16_t len;

    RC522_WriteReg(MFRC522_REG_BIT_FRAMING, 0x00);
    buf[0] = MF1_ANTICOLL;
    buf[1] = 0x20;
    status = RC522_ToCard(MFRC522_CMD_TRANSCEIVE, buf, 2, buf, &len);

    if (status == MI_OK) {
        for (uint8_t i = 0; i < 4; i++) {
            uid[i] = buf[i];
        }
        uint8_t check = uid[0] ^ uid[1] ^ uid[2] ^ uid[3];
        if (check != buf[4]) {
            status = MI_ERR;
        }
    }
    return status;
}

static void RC522_Halt(void) {
    uint8_t buf[16];
    uint16_t len;

    buf[0] = MF1_DESELECT;
    buf[1] = 0;
    RC522_ToCard(MFRC522_CMD_TRANSCEIVE, buf, 2, buf, &len);
}

uint8_t RC522_ReadCard(uint8_t *uid) {
    if (uid == NULL) return MI_ERR;

    uint8_t status = RC522_Request(MF1_REQIDL);
    if (status != MI_OK) {
        return status;
    }

    status = RC522_Anticoll(uid);
    if (status == MI_OK) {
        RC522_Halt();
    }

    return status;
}
// void TEST(void){
//     uint8_t uid[4];
//     if (RC522_ReadCard(uid) == MI_OK) {
//         snprintf(display, sizeof(display), "%02X%02X%02X%02X",
//                 uid[0], uid[1], uid[2], uid[3]);
//         OLED_ShowStr(0, 4,display, 2, 0);
//     }
// }

