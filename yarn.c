#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <pthread.h>
#include <cairo-xlib.h>

#include "datatypes.h"
#include "parse.h"
#include "draw.h"
#include "queue.h"
#include "yarn.h"

// pthread types.
pthread_t split_notification;
pthread_mutex_t stack_mutex = PTHREAD_MUTEX_INITIALIZER;

extern Queue queuespec;
static bool THREAD_ALIVE = false;

// Option initialization.
double timeout = 10;
int  margin = 5, upper = 0,
     xpos = 0, ypos = 0,
     width = 0, height = 0,
     gap = 7, rounding = 4,
     max = 4;
char *font = "Incosolata 14";
char *dimensions = "300x30+300+300";

//void run(struct notification *n)
void
*run(void *arg)
{
    Notification *n = (Notification*) arg;

    parse(dimensions, &xpos, &ypos, &width, &height);
    Variables *opt = var_create(font, margin, max, upper, gap, rounding, timeout, xpos, ypos, width, height);

    // string, text x, text y, x, y, fuse.
    Message message = message_create(n->summary, n->body, 0, 0, -opt->width-1, 0, opt->timeout);
    queuespec = queue_insert(queuespec, message);

    //printf("sizeof: %lu\n", sizeof(n));
    //printf("%s\n", n->summary);
    //printf("running now!\n");

    draw(opt, message);

    //pthread_mutex_destroy(&stack_mutex);
    free(arg);

    THREAD_ALIVE = false;

    return NULL;
}

void
prepare(Notification *n)
{
    Variables *opt = var_create(font, margin, max, upper, gap, rounding, timeout, xpos, ypos, width, height);

    // If there aren't any notifications being shown, we need to create a new thread.
    // If there are notifications being shown, simply add the new notification to the queue.
    if (THREAD_ALIVE == false) {
        pthread_create(&split_notification, NULL, run, n);
        THREAD_ALIVE = true;
    }
    else {
        queuespec = queue_insert(queuespec, message_create(n->summary, n->body, 0, 0, -opt->width-1, (queuespec.rear)*(opt->height + opt->gap), opt->timeout));
    }

    //free(n);
}
