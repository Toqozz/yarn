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
#include "cairo.h"
#include "config.h"

// pthread types.
pthread_t split_notification;
pthread_mutex_t stack_mutex = PTHREAD_MUTEX_INITIALIZER;

extern Queue queuespec;
extern Message MessageArray[QUEUESIZE];
static bool THREAD_ALIVE = false;

// Create a struct on the heap.
Variables
*var_create(char *font,
            int margin, int max, int gap, int overline, int bw, char *bc,
            int rounding, int timeout, int xpos, int ypos,
            int width, int height)
{
    // TODO make sure that this gets freed.
    Variables *v = malloc(sizeof(Variables));
    assert(v != NULL);

    v->font = font;
    v->margin = margin;
    v->max = max;
    v->gap = gap;
    v->overline = overline;
    v->bw = bw;
    v->bc = bc;
    v->rounding = rounding;
    v->timeout = timeout;
    v->xpos = xpos;
    v->ypos = ypos;
    v->width = width;
    v->height = height;

    return v;
}

Config
*cfg_create(void)
{
    Config *c = malloc(sizeof(Config));
    assert(c != NULL);

    return c;
}

// Create messages on the stack.
Message
message_create(char *summary, char *body, int textx, int texty, int x, int y, double fuse)
{
    Message m;

    m.summary = summary;
    m.body = body;
    m.textx = textx;
    m.texty =texty;
    m.x = x;
    m.y = y;
    m.visible = 1;
    m.fuse = fuse;

    return m;
}

Variables
*var_initialize(void)
{
    // Option initialization.
    double timeout = 10;
    int  margin = 5, upper = 0,
         bw = 3, xpos = 0,
         ypos = 0, width = 0,
         height = 0, gap = 7,
         rounding = 4, max = 4;
    char *font = "Inconsolata 11";
    char *dimensions = "300x23+1930+10";
    char *bc = "ebdbb2";

    parse(dimensions, &xpos, &ypos, &width, &height);
    Variables *temp = var_create(font, margin, max, gap, upper, bw, bc, rounding, timeout, xpos, ypos, width, height);
}

void
*run(void *arg)
{
    Notification *n = (Notification*) arg;

    Variables *opt = var_initialize();

    // string, text x, text y, x, y, fuse.
    Message message = message_create(n->summary,    // notify summary
                                     n->body,       // notify body
                                     0,             // text x
                                     0,             // text y
                                     0,             // x
                                     0,             // y
                                     opt->timeout); // fuse
    queuespec = queue_insert(queuespec, message);

    // Draw...
    draw(opt);

    // Notification is freed when its done.
    free(arg);

    THREAD_ALIVE = false;

    return NULL;
}

void
prepare(Notification *n)
{
    if (THREAD_ALIVE == false) {
        pthread_create(&split_notification, NULL, run, n);
        THREAD_ALIVE = true;
    } else {
        Variables *opt = var_initialize();
        if (queuespec.rear >= opt->max) {
            queuespec = queue_delete(queuespec, 0);
            queue_align(queuespec);

            // If there aren't any notifications being shown, we need to create a new thread.
            queuespec = queue_insert(queuespec, message_create(n->summary,
                                                               n->body,
                                                               0,
                                                               queuespec.rear * (opt->height + opt->gap),
                                                               0,
                                                               queuespec.rear * (opt->height + opt->gap),
                                                               opt->timeout));
        } else {
            // If there are notifications being shown, simply add the new notification to the queue.
            queuespec = queue_insert(queuespec, message_create(n->summary,
                                                               n->body,
                                                               0,
                                                               queuespec.rear * (opt->height + opt->gap),
                                                               0,
                                                               queuespec.rear * (opt->height + opt->gap),
                                                               opt->timeout));
        }
    }
}
