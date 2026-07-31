#include "as608.h"
#include "myuart.h"
#include "keypad.h"
#include <string.h>

static const char *TAG = "AS608";

#define AS608_UART_PORT  UART_NUM_2
#define AS608_RX_BUF     512

// 模块地址（默认 0xFFFFFFFF）
static uint32_t as608_addr = 0xFFFFFFFF;

/* ========== 底层收发 ========== */

// 发送指令包
static void AS608_SendPacket(uint8_t *cmd_data, uint16_t cmd_len) {
    uint8_t buf[128];
    uint16_t idx = 0;

    // 发送前清空接收缓冲，避免收到上次的脏数据
    uart_flush_input(AS608_UART_PORT);

    // 包头
    buf[idx++] = 0xEF;
    buf[idx++] = 0x01;
    // 地址
    buf[idx++] = (as608_addr >> 24) & 0xFF;
    buf[idx++] = (as608_addr >> 16) & 0xFF;
    buf[idx++] = (as608_addr >> 8)  & 0xFF;
    buf[idx++] =  as608_addr        & 0xFF;
    // 包标识
    buf[idx++] = AS608_PID_CMD;
    // 包长度 = 内容(cmd_len) + 校验(2)，不含标识字节
    uint16_t pkg_len = cmd_len + 2;
    buf[idx++] = (pkg_len >> 8) & 0xFF;
    buf[idx++] =  pkg_len       & 0xFF;
    // 内容
    for (uint16_t i = 0; i < cmd_len; i++) {
        buf[idx++] = cmd_data[i];
    }
    // 校验和 = 标识 + 长度(2B) + 内容
    uint16_t checksum = AS608_PID_CMD;
    checksum += (pkg_len >> 8) & 0xFF;
    checksum +=  pkg_len       & 0xFF;
    for (uint16_t i = 0; i < cmd_len; i++) {
        checksum += cmd_data[i];
    }
    buf[idx++] = (checksum >> 8) & 0xFF;
    buf[idx++] =  checksum       & 0xFF;

    uart_write_bytes(AS608_UART_PORT, buf, idx);
    // 等待 TX 发送完成，避免立刻切 RX 导致丢失
    uart_wait_tx_done(AS608_UART_PORT, pdMS_TO_TICKS(100));
}

// 接收应答包，返回应答码（content[0]），content 其余写入 out
static uint8_t AS608_RecvPacket(uint8_t *out, uint16_t *out_len, uint32_t timeout_ms) {
    uint8_t header[9];

    // 先读 9 字节：包头(2) + 地址(4) + 标识(1) + 长度(2)
    int n = uart_read_bytes(AS608_UART_PORT, header, 9, pdMS_TO_TICKS(timeout_ms));
    if (n < 9) {
        ESP_LOGW(TAG, "recv timeout, got %d bytes", n);
        return AS608_ERR;
    }

    // 校验包头
    if (header[0] != 0xEF || header[1] != 0x01) {
        ESP_LOGW(TAG, "bad header: %02X %02X", header[0], header[1]);
        return AS608_ERR;
    }

    // 应答标识应为 0x07
    if (header[6] != AS608_PID_ACK) {
        ESP_LOGW(TAG, "bad pid: %02X", header[6]);
        return AS608_ERR;
    }

    // 包长度
    uint16_t pkg_len = ((uint16_t)header[7] << 8) | header[8];
    // content 长度 = pkg_len - 校验(2)，不含标识字节
    uint16_t content_len = pkg_len - 2;
    if (content_len == 0 || content_len > AS608_RX_BUF) {
        return AS608_ERR;
    }

    // 读取 content + 校验
    uint8_t body[AS608_RX_BUF];
    n = uart_read_bytes(AS608_UART_PORT, body, content_len + 2, pdMS_TO_TICKS(timeout_ms));
    if (n < content_len + 2) {
        ESP_LOGW(TAG, "body short: %d/%d", n, content_len + 2);
        return AS608_ERR;
    }

    // content[0] 为应答码，其余为返回数据
    if (out && out_len) {
        uint16_t data_len = (content_len > 1) ? (content_len - 1) : 0;
        *out_len = data_len;
        if (data_len > 0 && out) {
            memcpy(out, body + 1, data_len);
        }
    }

    return body[0]; // 应答码
}

/* ========== 初始化 ========== */

void AS608_Init(void) {
    // 配置 TCH 触摸引脚为输入
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << AS608_TCH),
        .pull_down_en = 0,
        .pull_up_en = 0,
    };
    gpio_config(&io_conf);

    // 清空 UART 接收缓冲
    uart_flush_input(AS608_UART_PORT);

    ESP_LOGI(TAG, "AS608 init done (UART2, TCH=%d)", AS608_TCH);
}

/* ========== TCH 引脚 ========== */

uint8_t AS608_GetFingerPressed(void) {
    // TCH 引脚高电平表示有手指触摸
    return gpio_get_level(AS608_TCH) == 1;
}

/* ========== 高层指令 ========== */

uint8_t AS608_GenImg(void) {
    uint8_t cmd[1] = { AS608_CMD_GENIMG };
    AS608_SendPacket(cmd, 1);
    uint16_t len;
    return AS608_RecvPacket(NULL, &len, 2000);
}

uint8_t AS608_Img2Tz(uint8_t buffer_id) {
    uint8_t cmd[2] = { AS608_CMD_IMG2TZ, buffer_id };
    AS608_SendPacket(cmd, 2);
    uint16_t len;
    return AS608_RecvPacket(NULL, &len, 2000);
}

uint8_t AS608_RegModel(void) {
    uint8_t cmd[1] = { AS608_CMD_REGMODEL };
    AS608_SendPacket(cmd, 1);
    uint16_t len;
    return AS608_RecvPacket(NULL, &len, 3000);
}

uint8_t AS608_Store(uint8_t buffer_id, uint16_t page_id) {
    uint8_t cmd[4] = {
        AS608_CMD_STORE,
        buffer_id,
        (uint8_t)((page_id >> 8) & 0xFF),
        (uint8_t)( page_id       & 0xFF),
    };
    AS608_SendPacket(cmd, 4);
    uint16_t len;
    return AS608_RecvPacket(NULL, &len, 2000);
}

uint8_t AS608_Search(uint8_t buffer_id, uint16_t start, uint16_t count,
                     uint16_t *page_id, uint16_t *score) {
    uint8_t cmd[6] = {
        AS608_CMD_SEARCH,
        buffer_id,
        (uint8_t)((start >> 8) & 0xFF),
        (uint8_t)( start       & 0xFF),
        (uint8_t)((count >> 8) & 0xFF),
        (uint8_t)( count       & 0xFF),
    };
    AS608_SendPacket(cmd, 6);

    uint8_t data[8];
    uint16_t data_len;
    uint8_t ack = AS608_RecvPacket(data, &data_len, 3000);

    if (ack == AS608_OK) {
        // 返回数据: 页号(2B) + 得分(2B)
        if (data_len >= 4) {
            if (page_id) *page_id = ((uint16_t)data[0] << 8) | data[1];
            if (score)   *score   = ((uint16_t)data[2] << 8) | data[3];
        }
    }
    return ack;
}

uint8_t AS608_Empty(void) {
    uint8_t cmd[1] = { AS608_CMD_EMPTY };
    AS608_SendPacket(cmd, 1);
    uint16_t len;
    return AS608_RecvPacket(NULL, &len, 2000);
}

uint8_t AS608_DeleteChar(uint16_t page_id, uint16_t count) {
    uint8_t cmd[5] = {
        AS608_CMD_DELETCHAR,
        (uint8_t)((page_id >> 8) & 0xFF),
        (uint8_t)( page_id       & 0xFF),
        (uint8_t)((count   >> 8) & 0xFF),
        (uint8_t)( count         & 0xFF),
    };
    AS608_SendPacket(cmd, 5);
    uint16_t len;
    return AS608_RecvPacket(NULL, &len, 2000);
}

uint8_t AS608_GetFingerCount(uint16_t *count) {
    uint8_t cmd[1] = { AS608_CMD_READSYSPARA };
    AS608_SendPacket(cmd, 1);

    uint8_t data[20];
    uint16_t data_len;
    uint8_t ack = AS608_RecvPacket(data, &data_len, 2000);

    if (ack == AS608_OK && data_len >= 6) {
        // 系统参数: 状态(2B) + 标识(2B) + 库容量(2B) + ...
        // 库容量在 data[4..5]
        if (count) *count = ((uint16_t)data[4] << 8) | data[5];
    }
    return ack;
}

/* ===== 波特率切换 + 自检 ===== */

static void AS608_ChangeBaud(uint32_t baud) {
    // 直接切换波特率，不 delete/install，更可靠
    uart_set_baudrate(AS608_UART_PORT, (int)baud);
    vTaskDelay(pdMS_TO_TICKS(50));
    uart_flush_input(AS608_UART_PORT);
}

uint8_t AS608_SelfTest(void) {
    // 尝试更多波特率，含 28800（57600/2）
    static const uint32_t bauds[] = {
        57600, 28800, 115200, 9600, 19200, 38400, 4800, 14400
    };
    uint8_t cmd[1] = { AS608_CMD_READSYSPARA };
    uint16_t data_len;
    uint8_t data[20];

    for (uint8_t i = 0; i < sizeof(bauds) / sizeof(bauds[0]); i++) {
        ESP_LOGI(TAG, "try baudrate = %lu ...", (unsigned long)bauds[i]);
        AS608_ChangeBaud(bauds[i]);

        AS608_SendPacket(cmd, 1);
        uint8_t ack = AS608_RecvPacket(data, &data_len, 1500);

        if (ack == AS608_OK) {
            ESP_LOGI(TAG, "AS608 responding at baudrate = %lu", (unsigned long)bauds[i]);
            return AS608_OK;
        }

        // 打印残留原始字节
        uint8_t raw[32] = {0};
        int rn = uart_read_bytes(AS608_UART_PORT, raw, 32, pdMS_TO_TICKS(50));
        if (rn > 0) {
            printf("[AS608 raw %lu] ", (unsigned long)bauds[i]);
            for (int k = 0; k < rn; k++) printf("%02X ", raw[k]);
            printf("\n");
        }
    }

    ESP_LOGE(TAG, "no response on any baudrate. Check TX/RX and VCC.");
    return AS608_ERR;
}

