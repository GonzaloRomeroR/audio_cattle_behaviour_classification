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
    if (desviation <= 690)
    {
        return 1;
    }
    else
    {
        if (maximum <= 113.5)
        {
            return 0;
        }
        else
        {
            return 2;
        }
    }
}
