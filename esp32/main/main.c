#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include <ctype.h>
#include "noise_gate.h"
#include "filter.h"
#include "decision_tree.h"
#include "extractor.h"

static const int RX_BUF_SIZE = 1024;

#define TXD_PIN (UART_PIN_NO_CHANGE)
#define RXD_PIN (UART_PIN_NO_CHANGE)

#define UART UART_NUM_0

uint8_t counter;
char uart_command[30];

void uart_init(void)
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

float duration = 0;
float crosses = 0;
float maximum = 0;
float simetry = 0;
float desviation = 0;

int interpret_command(char command[], int size)
{
    if (size < 3)
    {
        return -1;
    }
    if (command[0] == ':')
    {
        return atoi(&command[1]);
    }
    return -1;
}

float interpret_command_features(char command[], int size)
{

    int result;
    char *data_send;
    switch (command[1])
    {
    case 't':
        duration = (float)atof(&command[2]);
        return duration;
    case 'c':
        crosses = (float)atof(&command[2]);
        return crosses;
    case 'm':
        maximum = (float)atof(&command[2]);
        return maximum;
    case 's':
        simetry = (float)atof(&command[2]);
        return simetry;
    case 'd':
        desviation = (float)atof(&command[2]);
        return desviation;
    case 'f':
        data_send = (char *)malloc(RX_BUF_SIZE + 1);
        result = decision_tree_classify(duration, crosses, maximum, simetry, desviation);
        sprintf(data_send, "%d", result);
        // ESP_LOGI("TX_INT", "Result: %d", result);
        sendData(data_send);
        return 0;
    }
    return -1;
}

void interpret_rx(char command[], int size)
{
    for (int i = 0; i < size; i++)
    {
        switch (command[i])
        {
        case ':':
            counter = 0;
            uart_command[counter++] = command[i];
            break;
        case '\r':
            uart_command[counter] = 0;
            interpret_command_features(uart_command, counter);
            break;
        case '\n':
            uart_command[counter] = 0;
            interpret_command_features(uart_command, counter);
            break;
        default:
            uart_command[counter++] = command[i];
            break;
        }
    }
}

// PIPELINE
void pipeline(float data[], int size, float sampleRate)
{
    initialize_ma();

    float *filtered_data;
    filtered_data = (float *)malloc(size * sizeof(float));

    for (int i = 0; i < size; i++)
    {
        filtered_data[i] = calculate_ma(data[i]);
        // printf("Filter %d: %f\n", i, filtered_data[i]);
    }

    float *noise_gate_data;
    noise_gate_data = (float *)malloc(size * sizeof(float));

    for (int i = 0; i < size; i++)
    {
        noise_gate_data[i] = noise_gate(filtered_data[i]);
        // printf("Gate %d: %f\n", i, noise_gate_data[i]);
    }

    extractFeatures(noise_gate_data, size, sampleRate);

    for (int i = 0; i < 5; i++)
    {
        printf("Feature %d: %f\n", i, features[i]);
    }
    int result = decision_tree_classify(features[0], features[1], features[2], features[3], features[4]);
    printf("Result: %d", result);
}

static void rx_task(void *arg)
{
    char *data = (char *)malloc(RX_BUF_SIZE + 1);
    initialize_ma();
    while (1)
    {
        const int rxBytes = uart_read_bytes(UART_NUM_0, data, RX_BUF_SIZE, 1000 / portTICK_RATE_MS);
        if (rxBytes > 0)
        {
            data[rxBytes] = 0;
            // ESP_LOGI("RX_TASK", "Read %d chars: '%s'", rxBytes, data);
            interpret_rx(data, rxBytes);
            // ESP_LOGI("RX_INT", "Cast value: %f", value);

            // ESP_LOGI("RX_INT", "Cast value: %d", value);
            //  int noise_gate_out = noise_gate(value);
            //  int filter_out = (int)calculate_ma(value);
            //  ESP_LOGI("NOISE_GATE", "Noise gate output %d", noise_gate_out);
            //  char *data_send = (char *)malloc(RX_BUF_SIZE + 1);
            //  sprintf(data_send, "%d", noise_gate_out);
            //  sprintf(data_send, "%d", filter_out);
            //  ESP_LOGI("MOVING_AVERAGE", "Moving average output: %d", (int)filter_out);
            //  sendData(data_send);
        }
    }
    free(data);
}

void app_main(void)
{
    uart_init();
    xTaskCreate(rx_task, "uart_rx_task", 1024 * 2, NULL, configMAX_PRIORITIES, NULL);
    // xTaskCreate(tx_task, "uart_tx_task", 1024 * 2, NULL, configMAX_PRIORITIES - 1, NULL);
}
