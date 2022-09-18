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

void initialize(queue *q);
bool isempty(queue *q);
void enqueue(queue *q, int value);
int dequeue(queue *q);
void display(node *head);
int queue_sum(queue *q);