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

float features[5] = {0, 0, 0, 0, 0};
int size = 0;

float getDuration(int size, float sampleRate)
{
    return size / sampleRate;
}

float getSum(float num[], int size)
{
    float sum = 0;
    for (int i = 0; i < size; i++)
    {
        sum += num[i];
    }
    return sum;
}

float getStd(float num[], int size)
{
    float sum = getSum(num, size);
    float mean = sum / size;
    float std = 0;
    for (int i = 0; i < size; i++)
    {
        std += pow(num[i] - mean, 2);
    }
    std = std / size;
    std = pow(std, 0.5);
    return std;
}

float getMaximum(float num[], int size)
{
    float max = -(FLT_MAX - 1);
    for (int i = 0; i < size; i++)
    {
        if (num[i] > max)
        {
            max = num[i];
        }
    }
    return max;
}

float getCrosses(float num[], int size)
{
    int crosses = 0;
    bool dt;
    bool dt_prev = ((num[1] - num[0]) > 0);

    for (int i = 1; i < size - 1; i++)
    {
        dt = ((num[i + 1] - num[i]) > 0);
        if (dt != dt_prev)
        {
            crosses++;
        }
        dt_prev = dt;
    }
    return crosses;
}

float getSimetry(float num[], int size)
{
    float max = getMaximum(num, size);
    int argMax = 0;

    for (int i = 0; i < size; i++)
    {
        if (num[i] == max)
        {
            argMax = i;
        }
    }

    float areaBefore = 0;

    for (int i = 0; i < argMax; i++)
    {
        areaBefore += num[i] + (num[i + 1] - num[i]) / 2;
    }

    if (areaBefore == 0)
    {
        return 0;
    }

    float areaAfter = 0;

    for (int i = argMax; i < size - 1; i++)
    {
        areaAfter += num[i] + (num[i + 1] - num[i]) / 2;
    }

    return areaAfter / areaBefore;
}

void extractFeatures(float num[], int size, float sampleRate)
{
    features[0] = getDuration(size, sampleRate);
    features[1] = getMaximum(num, size);
    features[2] = getCrosses(num, size);
    features[3] = getSimetry(num, size);
    features[4] = getStd(num, size);
}
