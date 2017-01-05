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
#include "cfg.h"

// pthread types.
pthread_t split_notification;
pthread_mutex_t stack_mutex = PTHREAD_MUTEX_INITIALIZER;

extern Queue queuespec;
extern Message MessageArray[QUEUESIZE];
extern Variables opt;

static bool THREAD_ALIVE = false;

Variables
*var_create(void)
{
    // TODO make sure that this gets freed.
    Variables *v = malloc(sizeof(Variables));
    assert(v != NULL);

    return v;
}

// Create messages on the stack.
Message
message_create(char *summary, char *body, int textx, int texty, int x, int y, double fuse)
{
    Message m;

    summary = parse_strip_markup(summary);
    summary = parse_quote_markup(summary);

    body = parse_strip_markup(body);
    body = parse_quote_markup(body);

    m.summary = summary;
    m.body = body;
    m.swidth = 0;
    m.bwidth = 0;
    m.textx = textx;
    m.texty = texty;
    m.x = x;
    m.y = y;
    m.visible = 1;
    m.fuse = fuse;

    return m;
}

void *
run(void *arg)
{
    Notification *n = (Notification*) arg;

    // string, text x, text y, x, y, fuse.
    Message message = message_create(n->summary,    // notify summary
                                     n->body,       // notify body
                                     0,             // text x
                                     0,             // text y
                                     0,             // x
                                     0,             // y
                                     opt.timeout); // fuse
    queuespec = queue_insert(queuespec, message);

    // Draw...
    draw();

    // Notification is freed when its done.
    free(n);

    // TODO, lowercase, its not a constant...
    THREAD_ALIVE = false;

    return NULL;
}

void
prepare(Notification *n)
{
    if (THREAD_ALIVE == false) {
        pthread_create(&split_notification, NULL, run, n);
        pthread_detach(split_notification);
        THREAD_ALIVE = true;
    } else {
        //Variables *opt.= var_initialize();
        if (queuespec.rear == opt.max_notifications) {
            queuespec = queue_delete(queuespec, 0);
            queue_align(queuespec);

            // If there aren't any notifications being shown, we need to create a new thread.
            queuespec = queue_insert(queuespec, message_create(n->summary,
                                                               n->body,
                                                               0,
                                                               queuespec.rear * (opt.height + opt.gap),
                                                               0,
                                                               queuespec.rear * (opt.height + opt.gap),
                                                               opt.timeout));
        } else {
            // If there are notifications being shown, simply add the new notification to the queue.
            queuespec = queue_insert(queuespec, message_create(n->summary,
                                                               n->body,
                                                               0,
                                                               queuespec.rear * (opt.height + opt.gap),
                                                               0,
                                                               queuespec.rear * (opt.height + opt.gap),
                                                               opt.timeout));
        }
    }
}
