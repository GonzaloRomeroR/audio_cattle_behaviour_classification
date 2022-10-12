#include <stdio.h>
#include <math.h>
#include <float.h>
#include <stdbool.h>

// QUEUE

#define TRUE 1
#define FALSE 0
#define FULL 10

struct node
{
    int data;
    struct node *next;
};
typedef struct node node;

struct queue
{
    int count;
    node *front;
    node *rear;
};
typedef struct queue queue;

void initialize(queue *q)
{
    q->count = 0;
    q->front = NULL;
    q->rear = NULL;
}

bool isempty(queue *q)
{
    // ESP_LOGI("ENTERING", "Entering is empty");
    return (q->rear == NULL);
}

void enqueue(queue *q, int value)
{
    if (q->count < FULL)
    {
        node *tmp;
        tmp = malloc(sizeof(node));
        tmp->data = value;
        tmp->next = NULL;
        if (!isempty(q))
        {
            q->rear->next = tmp;
            q->rear = tmp;
        }
        else
        {
            q->front = q->rear = tmp;
        }
        q->count++;
    }
    else
    {
        printf("List is full\n");
    }
}

int queue_sum(queue *q)
{
    int sum = 0;
    node *temp = q->front;
    while (temp)
    {
        sum += temp->data;
        temp = temp->next;
    }
    return sum;
}

int dequeue(queue *q)
{
    node *tmp;
    int n = q->front->data;
    tmp = q->front;
    q->front = q->front->next;
    q->count--;
    free(tmp);
    return (n);
}

void display(node *head)
{
    if (head == NULL)
    {
        printf("NULL\n");
    }
    else
    {
        printf("%d\n", head->data);
        display(head->next);
    }
}



// MOVING AVERAGE
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

//NOISE GATE
bool opened = false;
float open_threshold = 20;
float close_threshold = 20;
float hold = 0;
float hold_time = 0;
float sample_rate = 1;

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
        printf("VALUE: %f\n", abs(value));
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
        else if (abs(value) >= close_threshold)
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


// EXTRACTOR
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


// DECISION TREE
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


// PIPELINE
void pipeline(float data[], int size, float sampleRate)
{
    initialize_ma();

    float *filtered_data;
    filtered_data = (float*)malloc(size * sizeof(float));

    for (int i = 0; i < size; i++){
        filtered_data[i] = calculate_ma(data[i]);
        //printf("Filter %d: %f\n", i, filtered_data[i]);
    }

    float *noise_gate_data;
    noise_gate_data = (float*)malloc(size * sizeof(float));

    for (int i = 0; i < size; i++){
        noise_gate_data[i] = noise_gate(filtered_data[i]);
        //printf("Gate %d: %f\n", i, noise_gate_data[i]);
    }

    extractFeatures(noise_gate_data, size, sampleRate);

    for (int i = 0; i < 5; i++){
        printf("Feature %d: %f\n", i, features[i]);
    }
    int result = decision_tree_classify(features[0], features[1], features[2], features[3], features[4]);
    printf("Result: %d", result);
}


void main()
{
    float sampleRate = 20;
    float data[6] = {23.4, 55, 22.6, 3, 40.5, 18};
    int size = 6;
    pipeline(data, size, sampleRate);
}
