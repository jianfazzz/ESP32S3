#include "mywifiap.h"
#include "keypad.h"
#include "lwip/sockets.h"
#include <string.h>


static const char *TAG = "MAIN";


// 客户端 socket 数组
static int client_sockets[MAX_CLIENTS] = {0};
static int client_count = 0;
// ==========================================================
// 数据上报任务（定时发送传感器数据到所有客户端）
// ==========================================================
void report_task(void *pvParameters)
{
    TickType_t last_wake_time = xTaskGetTickCount();
    char send_buf[256];

    while (1) {
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(REPORT_PERIOD_MS));

        int len = snprintf(send_buf, sizeof(send_buf),
                           "temperature%02d hum%02d light%02d CO2%04u shi%02d fen%02d miao%02d",
                           temperature, hum, light, CO2, shi, fen, miao);
        if (len < 0 || len >= (int)sizeof(send_buf)) {
            continue;
        }

        // 向所有客户端发送
        if (xSemaphoreTake(client_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
            continue;
        }
        for (int i = 0; i < client_count; i++) {
            int sock = client_sockets[i];
            if (sock > 0) {
                int ret = send(sock, send_buf, len, 0);
                if (ret < 0) {
                    close(sock);
                    client_sockets[i] = 0;
                }
            }
        }
        // 清理断开的客户端
        int new_count = 0;
        for (int i = 0; i < client_count; i++) {
            if (client_sockets[i] > 0) {
                client_sockets[new_count++] = client_sockets[i];
            }
        }
        client_count = new_count;
        xSemaphoreGive(client_mutex);
    }
}
// ==========================================================
// 处理接收到的命令
// ==========================================================
static void process_command(const char *cmd)
{
    uint16_t val;

    // val = ReceiveData_(cmd, "mode:");
    // if (val) {
    //     mode = (mode == 0) ? 1 : 0;   // 切换模式
    //     ESP_LOGI(TAG, "Mode switched to %d", mode);
    // }

    val = ReceiveData_(cmd, "tempMax:");
    if (val) { tempMax = (uint8_t)(val - 1); }

    val = ReceiveData_(cmd, "humMax:");
    if (val) { humMax = (uint8_t)(val - 1); }

   val = ReceiveData_(cmd, "angle:");
    if (val>=1 && val<=181) { angle = (uint8_t)(val - 1); }

    // val = ReceiveData_(cmd, "soilMax:");
    // if (val) { soilMax = (uint16_t)(val - 1); }

    // val = ReceiveData_(cmd, "water:");
    // if (val && mode == 1) {
    //     sensor = 1;
    //     water = 1;
    // }

    // val = ReceiveData_(cmd, "WATER:");
    // if (val && mode == 1) {
    //     sensor = 1;
    //     water = 0;
    // }

    // val = ReceiveData_(cmd, "fan:");
    // if (val && mode == 1) {
    //     sensor = 0;
    //     fan = 1;
    // }

    // val = ReceiveData_(cmd, "FAN:");
    // if (val && mode == 1) {
    //     sensor = 0;
    //     fan = 0;
    // }
}

// ==========================================================
// 客户端处理任务（每个连接独立）
// ==========================================================
static void client_handler_task(void *pvParameters)
{
    int sock = (int)pvParameters;
    char rx_buffer[256];
    int len;

    while (1) {
        len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
        if (len <= 0) {
            break;
        }
        rx_buffer[len] = '\0';
        process_command(rx_buffer);
    }

    // 关闭 socket 并从客户端列表中移除
    close(sock);
    if (xSemaphoreTake(client_mutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
        for (int i = 0; i < client_count; i++) {
            if (client_sockets[i] == sock) {
                client_sockets[i] = 0;
                break;
            }
        }
        // 压缩数组
        int new_count = 0;
        for (int i = 0; i < client_count; i++) {
            if (client_sockets[i] > 0)
                client_sockets[new_count++] = client_sockets[i];
        }
        client_count = new_count;
        xSemaphoreGive(client_mutex);
    }
    vTaskDelete(NULL);
}
// ==========================================================
// 函数：ReceiveData_
// 功能：从数据中解析 "name:数值"，并返回数值（+1 适配原逻辑）
// ==========================================================
uint16_t ReceiveData_(const char *dataPtr, const char *name)
{
    char *str1 = strstr((char *)dataPtr, name);
    if (str1 == NULL) return 0;   // 0 表示未找到

    // 找到冒号
    str1 = strchr(str1, ':');
    if (str1 == NULL) return 0;
    str1++;   // 跳过 ':'

    char setvalue[4] = {0};
    int i = 0;
    while (*str1 >= '0' && *str1 <= '9' && i < 3) {
        setvalue[i++] = *str1++;
    }
    setvalue[i] = '\0';
    int val = atoi(setvalue);
    if (val >= 0 && val <= 999) {
        return (uint16_t)(val + 1);   // 原代码返回 +1
    }
    return 0;
}
// ==========================================================
// TCP 服务器任务（接收客户端连接和命令）
// ==========================================================
void tcp_server_task(void *pvParameters)
{
    int listen_fd, new_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (listen_fd < 0) {
        vTaskDelete(NULL);
        return;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(TCP_PORT);

    if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        close(listen_fd);
        vTaskDelete(NULL);
        return;
    }

    if (listen(listen_fd, MAX_CLIENTS) < 0) {
        close(listen_fd);
        vTaskDelete(NULL);
        return;
    }

    while (1) {
        new_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (new_fd < 0) {
            continue;
        }

        // 加入客户端列表
        if (xSemaphoreTake(client_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
            close(new_fd);
            continue;
        }
        if (client_count < MAX_CLIENTS) {
            client_sockets[client_count++] = new_fd;
            xSemaphoreGive(client_mutex);
            // 创建任务处理该客户端
            xTaskCreate(client_handler_task, "client_handler", 4096, (void*)new_fd, 6, NULL);
        } else {
            close(new_fd);
            xSemaphoreGive(client_mutex);
        }
    }
}

// ==========================================================
// Wi-Fi SoftAP 初始化
// ==========================================================
void WIFI_AP_init(void)
{
    // 初始化 NVS
    nvs_flash_init();

    // 创建默认网络接口
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // 配置 AP
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = WIFI_SSID,
            .ssid_len = strlen(WIFI_SSID),
            .password = WIFI_PASSWORD,
            .max_connection = MAX_CLIENTS,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
            .channel = 6,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

