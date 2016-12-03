#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <pthread.h>
#include <cairo-xlib.h>

#include "yarn.h"
#include "datatypes.h"
#include "parse.h"
#include "draw.h"
#include "queue.h"

// pthread types.
pthread_t split_notification;
pthread_mutex_t stack_mutex = PTHREAD_MUTEX_INITIALIZER;

extern Queue queuespec;
static bool THREAD_ALIVE = false;

// Create a struct on the heap.
struct Variables
*var_create(char *font,
            int margin, int max, int upper,
            int gap, int rounding, int timeout, int xpos, int ypos,
            int width, int height)
{
    struct Variables *info = malloc(sizeof(struct Variables));
    assert(info != NULL);

    info->font = font;
    info->margin = margin;
    info->max = max;
    info->upper = upper;
    info->gap = gap;
    info->rounding = rounding;
    info->timeout = timeout;
    info->xpos = xpos;
    info->ypos = ypos;
    info->width = width;
    info->height = height;

    return info;
}

// Create messages on the stack.
Message
message_create(char *summary, char *body, int textx, int texty, int x, int y, double fuse)
{
    Message message;

    message.summary = summary;
    message.body = body;
    message.textx = textx;
    message.texty =texty;
    message.x = x;
    message.y = y;
    message.visible = 1;
    message.fuse = fuse;

    return message;
}

Variables
*var_initialize(void)
{
    // Option initialization.
    double timeout = 10;
    int  margin = 5, upper = 0,
         xpos = 0, ypos = 0,
         width = 0, height = 0,
         gap = 7, rounding = 4,
         max = 4;
    char *font = "Incosolata 11";
    char *dimensions = "300x20+300+300";

    parse(dimensions, &xpos, &ypos, &width, &height);
    Variables *temp = var_create(font, margin, max, upper, gap, rounding, timeout, xpos, ypos, width, height);
}

void
*run(void *arg)
{
    Notification *n = (Notification*) arg;

    //parse(dimensions, &xpos, &ypos, &width, &height);
    //Variables *opt = var_create(font, margin, max, upper, gap, rounding, timeout, xpos, ypos, width, height);
    Variables *opt = var_initialize();

    // string, text x, text y, x, y, fuse.
    Message message = message_create(n->summary,    // notify summary
                                     n->body,       // notify body
                                     0,             // text x
                                     0,             // text y
                                     -opt->width,   // x
                                     0,             // y
                                     opt->timeout); // fuse
    queuespec = queue_insert(queuespec, message);

    draw(opt);

    //pthread_mutex_destroy(&stack_mutex);
    free(arg);

    THREAD_ALIVE = false;

    return NULL;
}

void
prepare(Notification *n)
{
    // If there aren't any notifications being shown, we need to create a new thread.
    // If there are notifications being shown, simply add the new notification to the queue.
    if (THREAD_ALIVE == false) {
        pthread_create(&split_notification, NULL, run, n);
        THREAD_ALIVE = true;
    } else {
        Variables *opt = var_initialize();
        queuespec = queue_insert(queuespec, message_create(n->summary,
                                                           n->body,
                                                           0,
                                                           queuespec.rear * (opt->height + opt->gap),
                                                           -opt->width,
                                                           queuespec.rear * (opt->height + opt->gap),
                                                           opt->timeout));
    }

    //free(n);
}
