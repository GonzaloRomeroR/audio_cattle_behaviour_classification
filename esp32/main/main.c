#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include <ctype.h>

static const int RX_BUF_SIZE = 1024;

#define TXD_PIN (UART_PIN_NO_CHANGE)
#define RXD_PIN (UART_PIN_NO_CHANGE)

#define UART UART_NUM_0

//////// Noise Gate /////////
bool opened = false;
int open_threshold = 30000;
int close_threshold = 20000;
float hold = 1;
float hold_time = 0;
float sample_rate = 22050;

int noise_gate(int value)
{

    if (!opened)
    {
        if (abs(value) > open_threshold)
        {
            opened = true;
            hold_time = 0;
            return value;
        }
        return 0;
    }

    else
    {
        if (hold_time > hold)
        {
            opened = false;
            hold_time = 0;
            return 0;
        }
        else if (abs(value) > close_threshold)
        {
            hold_time = 0;
        }
        else
        {
            hold_time += 1 / sample_rate;
        }
        return value;
    }

    return value;
}

/////////////////////////////

void init(void)
{
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    uart_driver_install(UART, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART, &uart_config);
    uart_set_pin(UART, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_flush(UART);
}

int sendData(const char *data)
{
    const int len = strlen(data);
    const int txBytes = uart_write_bytes(UART, data, len);
    return txBytes;
}

/*
static void tx_task(void *arg)
{
    while (1)
    {
        sendData("");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
*/

int interpret_command(char command[], int size)
{
    if (size < 3)
    {
        return -1;
    }
    if (command[0] == ':' && command[1] == ':')
    {
        return atoi(&command[2]);
    }
    return -1;
}

static void rx_task(void *arg)
{
    char *data = (char *)malloc(RX_BUF_SIZE + 1);
    while (1)
    {
        const int rxBytes = uart_read_bytes(UART_NUM_0, data, RX_BUF_SIZE, 1000 / portTICK_RATE_MS);
        if (rxBytes > 0)
        {
            data[rxBytes] = 0;
            // ESP_LOGI("RX_TASK", "Read %d chars: '%s'", rxBytes, data);
            int value = interpret_command(data, rxBytes);
            // ESP_LOGI("RX_INT", "Cast value: %d", value);
            int noise_gate_out = noise_gate(value);
            // ESP_LOGI("NOISE_GATE", "Noise gate output %d", noise_gate_out);
            char *data_send = (char *)malloc(RX_BUF_SIZE + 1);
            sprintf(data_send, "%d", noise_gate_out);
            sendData(data_send);
        }
    }
    free(data);
}

void app_main(void)
{
    init();
    xTaskCreate(rx_task, "uart_rx_task", 1024 * 2, NULL, configMAX_PRIORITIES, NULL);
    // xTaskCreate(tx_task, "uart_tx_task", 1024 * 2, NULL, configMAX_PRIORITIES - 1, NULL);
}
