#ifndef QUEUE_H
#define QUEUE_H

#include "type.h"
#define QUEUE_MAX_BUFFER 128

typedef uint8 buffer_size;

typedef struct __queue
{
    buffer_size buffer[QUEUE_MAX_BUFFER];
    int32 head, tail;
} Queue;

bool QueueEn(Queue *q, buffer_size val);
bool QueueDe(Queue *q, buffer_size *val);
bool QueueDeSome(Queue *q, buffer_size some);
bool QueuePeekSome(Queue *q, int32 some, buffer_size *buffer);
bool QueuePeek(Queue *q, buffer_size *val);
bool QueueIsFull(Queue *q);
bool QueueIsEmpty(Queue *q);
int32 QueueCount(Queue *q);

#endif