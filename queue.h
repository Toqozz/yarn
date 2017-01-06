#ifndef QUEUE_H_INCLUDED
#define QUEUE_H_INCLUDED

#define QUEUESIZE 100

#include <stdbool.h>
#include "datatypes.h"

void
queue_insert(Queue *queuespec, Message message);

void
queue_delete(Queue *queuespec, int position);

void
queue_align(Queue queuespec);

int
in_queue(Queue queuespec);

#endif
