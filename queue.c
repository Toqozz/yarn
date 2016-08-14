#include <stdio.h>
#include <stdbool.h>

#include "datatypes.h"
#include "queue.h"

extern Message MessageArray[QUEUESIZE];

Queue
queue_insert(Queue queuespec, Message message)
{
    if (queuespec.rear == QUEUESIZE-1)
        printf("Queue is full, skipped.\n");
    else
    {
        // If queue is initially empty.
        if (queuespec.front == -1)
            queuespec.front = 0;

        // Add item to array.
        // There is a new item, the end is pushed back.
        MessageArray[queuespec.rear++] = message;
    }

    return queuespec;
}
Queue
queue_delete(Queue queuespec, int position)
{
    int i = 0;

    // Nothing in queue.
    if (queuespec.front == - 1)
        printf("Queue is empty -- nothing to delete.\n");

    // Move each item down one.
    else
    {
        for (i = position; i < queuespec.rear-1; i++)
            MessageArray[i] = MessageArray[i+1];
        queuespec.rear--;
    }
    return queuespec;
}
bool
queue_empty(Queue queuespec)
{
    if (queuespec.front == queuespec.rear)
        return true;
    else
        return false;
}
