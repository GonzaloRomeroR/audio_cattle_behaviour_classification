#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include <ctype.h>

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