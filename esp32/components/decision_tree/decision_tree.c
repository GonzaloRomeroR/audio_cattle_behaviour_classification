#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include <ctype.h>

int decision_tree_classify(float duration, float crosses, float maximum, float simetry, float desviation)
{
    if (desviation <= 0.013)
    {
        if (duration <= 0.019)
        {
            return 1;
        }
        else
        {
            if (desviation <= 0.013)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
    }
    else
    {
        if (crosses <= 124.5)
        {
            if (simetry <= 0.189)
            {
                return 2;
            }
            else
            {
                return 1;
            }
        }
        else
        {
            return 2;
        }
    }
}
