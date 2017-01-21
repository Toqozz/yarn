#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <pthread.h>
#include <cairo-xlib.h>
#include <string.h>

#include "yarn.h"
#include "datatypes.h"
#include "parse.h"
#include "draw.h"
#include "queue.h"
#include "cairo.h"
#include "cfg.h"

//TODO; look up thread safety things.

// pthread types.
pthread_t split_notification;
pthread_attr_t tattr;
pthread_mutex_t stack_mutex = PTHREAD_MUTEX_INITIALIZER;

extern Queue queuespec;
//extern Message MessageArray[QUEUESIZE];
extern Variables opt;

// Maybe this can be moved so its not a global.
static bool THREAD_ALIVE = false;

void
notification_destroy(Notification *n)
{
    assert(n != NULL);

    free(n->app_name);
    free(n->app_icon);
    free(n->summary);
    free(n->body);
    free(n);
}

// Create messages on the stack.
Message
message_create(Notification *n, int textx, int texty, int x, int y, double fuse)
{
    Message m;
    char *summary = n->summary,
         *body = n->body;

    // Boop boop.
    summary = parse_strip_markup(summary);
    summary = parse_quote_markup(summary);

    body = parse_strip_markup(body);
    body = parse_quote_markup(body);

    m.summary = strdup(summary);
    m.body = strdup(body);
    m.swidth = 0;
    m.bwidth = 0;
    m.textx = textx;
    m.texty = texty;
    m.x = x;
    m.y = y;
    m.fuse = fuse;

    return m;
}

void *
run(void *arg)
{
    Notification *n = (Notification*) arg;
    // Shadows have made this a bit annoying.
    int xoffset = parse_offset_value(opt.shadow_xoffset),
        yoffset = parse_offset_value(opt.shadow_yoffset);

    // string, text x, text y, x, y, fuse.
    // These positions are based on the cairo surface, so it must be
    Message message = message_create(n,             // Notification struct.
                                     0 + xoffset,   // textx
                                     in_queue(queuespec) * (opt.height + opt.gap) + yoffset,   // texty
                                     0 + xoffset,   // x
                                     in_queue(queuespec) * (opt.height + opt.gap) + yoffset,   // y
                                     opt.timeout);  // fuse
    queue_insert(&queuespec, message);
    notification_destroy(n);

    // Draw...
    draw();

    // TODO, lowercase, its not a constant...
    THREAD_ALIVE = false;

    pthread_exit(NULL);
}

void
prepare(Notification *n)
{
    int ret;

    // TODO, change opt.timeout to n->expire_timeout.
    // If there aren't any notifications being shown, we need to create a new thread.
    if (THREAD_ALIVE == false) {
        // 1 == true == error.
        pthread_attr_init(&tattr);
        pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);

        ret = pthread_create(&split_notification, &tattr, &run, (void *)n);
        if (ret != 0) {
            perror("pthread_create() error");
            return;
        }

        THREAD_ALIVE = true;

    // If there are notifications being shown, simply add the new notification to the queue.
    } else {
        int xoffset = parse_offset_value(opt.shadow_xoffset),
            yoffset = parse_offset_value(opt.shadow_yoffset);

        // Queue full, remove one first.
        if (queuespec.rear == opt.max_notifications) {
            queue_delete(&queuespec, 0);
            queue_align(queuespec);
        }

        Message message = message_create(n,
                                         0 + xoffset,
                                         in_queue(queuespec) * (opt.height + opt.gap) + yoffset,
                                         0 + xoffset,
                                         in_queue(queuespec) * (opt.height + opt.gap) + yoffset,
                                         opt.timeout);

        queue_insert(&queuespec, message);
        notification_destroy(n);
    }
}
