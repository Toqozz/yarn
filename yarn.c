#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include "yarn.h"
#include "datatypes.h"

//TODO check if thread is running or not first please!!!

//void run(struct notification *n)
void
*run(void *arg)
{
    struct notification *n = (struct notification*) arg;

    printf("sizeof: %lu\n", sizeof(n));

    printf("%s\n", n->summary);

    //free(message);
    //pthread_mutex_destroy(&stack_mutex);
    return NULL;
}
