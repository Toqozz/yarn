#include <stdio.h>
#include <stdbool.h>

#include "datatypes.h"
#include "queue.h"

extern Message MessageArray[QUEUESIZE];

// TODO, use address instead of returning queue..

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

void
queue_align (Queue queuespec) {
    // TODO use actual measurements.
    // TODO, put notification size somewhere globally accessible.
    for (int i = 0; i < queuespec.rear; i++) {
        MessageArray[i].y = i * 30;
        MessageArray[i].texty = i * 30;
    }
}

int
in_queue(Queue queuespec)
{
    return queuespec.rear;
}

