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

float calculate_ma(int value)
{
    if (isempty(q) == true)
    {
        initialize(q);
    }
    if (q->count < window_size)
    {
        enqueue(q, value);
        if (q->count == window_size)
        {
            average = 0 / window_size;
            return average;
        }
        return 0;
    }
    else
    {
        int last = dequeue(q);
        enqueue(q, value);
        average += 1 / window_size * (value - last);
        return average;
    }
}
