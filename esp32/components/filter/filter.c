#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include <ctype.h>
#include "queue.h"

int window_size = 3;
queue *q;
float average = 0;

void initialize_ma()
{
    q = malloc(sizeof(queue));
    initialize(q);
    // ESP_LOGI("INIT", "Initializing filter");
}

float calculate_ma(int value)
{
    // ESP_LOGI("CALC", "Calculations started");
    if (q->count < window_size)
    {
        enqueue(q, value);
        if (q->count == window_size)
        {
            float sum = queue_sum(q);
            average = sum / window_size;
            return average;
        }
        return 0;
    }
    else
    {
        int last = dequeue(q);
        enqueue(q, value);
        average += 1 / (float)window_size * (value - last);
        return average;
    }
}
