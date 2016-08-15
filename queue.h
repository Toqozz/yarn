#ifndef QUEUE_H_INCLUDED
#define QUEUE_H_INCLUDED

#define QUEUESIZE 100

Queue
queue_insert(Queue queuespec, Message message);

Queue
queue_delete(Queue queuespec, int position);

bool
queue_empty(Queue queuespec);

#endif
