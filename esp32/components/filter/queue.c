#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include <ctype.h>
#include "queue.h"

void initialize(queue *q)
{
    q = malloc(sizeof(queue));
    q->count = 0;
    q->front = NULL;
    q->rear = NULL;
}

int isempty(queue *q)
{
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
    node *temp = q->rear;
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