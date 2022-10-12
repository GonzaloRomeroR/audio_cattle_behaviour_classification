#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include <ctype.h>

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <stdbool.h>

bool opened = false;
float open_threshold = 0.05;
float close_threshold = 0.05;
float hold = 0;
float hold_time = 0;
float sample_rate = 22050;

float noise_gate(float value)
{
    if (!opened)
    {

        if (fabs(value) >= open_threshold)
        {
            opened = true;
            hold_time = 0;
            return value;
        }
        return 0;
    }

    else
    {
        if (fabs(value) >= open_threshold)
        {
            opened = true;
            hold_time = 0;
            return value;
        }
        else if (hold_time >= hold)
        {
            opened = false;
            hold_time = 0;
            return 0;
        }
        else if (fabs(value) >= close_threshold)
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