#include "type.h"
#include "queue.h"

bool QueueEn(Queue *q, buffer_size val)
{
    int next_tail = (q->tail + 1) % QUEUE_MAX_BUFFER;
    if (next_tail == q->head) //Queue full
        return false;
    q->buffer[q->tail] = val;
    q->tail = next_tail;
    return true;
}
bool QueueDe(Queue *q, buffer_size *val)
{
    if (q->head == q->tail)
        return false;
    *val = q->buffer[q->head];
    q->head = (q->head + 1) % QUEUE_MAX_BUFFER;
    return true;
}
bool QueueDeSome(Queue *q, buffer_size some)
{
    int count = QueueCount(q);
    if (some > count)
        return false;
    int tmp = QUEUE_MAX_BUFFER + q->tail;
    tmp -= some;
    tmp %= QUEUE_MAX_BUFFER;
    q->tail = tmp;
    return true;
}
bool QueuePeekSome(Queue *q, int32 some, buffer_size *buffer)
{
    int count = QueueCount(q);
    if (some > count)
        return false;
    for (int i = 0; i < some; i++)
    {
        buffer[i] = q->buffer[(q->head + i) % QUEUE_MAX_BUFFER];
    }
    return true;
}

bool QueuePeek(Queue *q, buffer_size *val)
{
    if (q->head == q->tail)
        return false;
    *val = q->buffer[q->head];
    return true;
}
bool QueueIsFull(Queue *q)
{
    return (q->tail + 1) % QUEUE_MAX_BUFFER == q->head;
}

bool QueueIsEmpty(Queue *q)
{
    return q->head == q->tail;
}
int32 QueueCount(Queue *q)
{
    int32 result = q->tail - q->head;
    if (result >= 0)
        return result;
    return QUEUE_MAX_BUFFER + result;
}