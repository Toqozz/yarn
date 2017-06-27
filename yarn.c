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
//#include "utils.h"

//TODO; look up thread safety things.

// pthread types.
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_t split_notification;
pthread_attr_t tattr;

extern Queue queuespec;
extern Variables opt;

static bool thread_alive = false;

/* Initialize some variables etc */
void
notify_setup(Notification *n)
{
    if (n->expire_timeout <= 0)
        n->expire_timeout = opt.timeout;
}

/* Free stuff from notification struct */
void
notification_destroy(Notification *n)
{
    assert(n != NULL);
    assert(n->app_name != NULL);
    assert(n->app_icon != NULL);
    assert(n->summary != NULL);
    assert(n->body != NULL);

    free(n->app_name);
    free(n->app_icon);
    free(n->summary);
    free(n->body);
    free(n);
}

/* Create a message that can be used for draw stuff */
Message
message_create(Notification *n, int textx, int texty, int x, int y, double fuse)
{
    // Shadows have made this a bit annoying.
    int xoffset = parse_offset_value(opt.shadow_xoffset),
        yoffset = parse_offset_value(opt.shadow_yoffset) + (in_queue(queuespec) * (opt.height + opt.gap));

    Message m;
    char *summary = n->summary,
         *body = n->body;

    // Boop boop.
    summary = parse_quote_markup(parse_strip_markup(summary));
    //summary = parse_quote_markup(summary);

    body = parse_quote_markup(parse_strip_markup(body));
    //body = parse_quote_markup(body);

    m.summary = strdup(summary);
    m.body = strdup(body);
    m.swidth = 0;
    m.bwidth = 0;
    m.textx = textx + xoffset;
    m.texty = texty + yoffset;
    m.x = x + xoffset;
    m.y = y + yoffset;
    m.fuse = fuse;

    m.redraw = 1;

    return m;
}

/* Free strings that are used in Message */
void
message_destroy(Message *m)
{
    assert(m != NULL);
    assert(m->summary != NULL);
    assert(m->body != NULL);

    free(m->summary);
    free(m->body);
}


void *
run(void *arg)
{
    Notification *n = (Notification*) arg;

    // notification struct, text x, text y, x, y, fuse.
    // These positions are based on the cairo surface, so it starts at 0 regardless of
    // the window's position on the screen.
    Message message = message_create(n, 0, 0, 0, 0, n->expire_timeout);
    queue_insert(&queuespec, message);
    notification_destroy(n);

    // Draw...
    draw();

    // When draw finishes we're done.
    thread_alive = false;

    pthread_exit(NULL);
}

/* Prepare and decide before pushing a notification to the queue / starting a new thread */
void
prepare(Notification *n)
{
    int ret;

    notify_setup(n);

    // If there aren't any notifications being shown, we need to create a new thread.
    // If there are notifications being shown, simply add the new notification to the queue.
    if (thread_alive == false) {
        // Detached threads because we never want to join to them.
        pthread_attr_init(&tattr);
        pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);

        ret = pthread_create(&split_notification, &tattr, &run, (void *)n);
        if (ret != 0) {
            perror("pthread_create() error");
            return;
        }

        thread_alive = true;
    } else {
        // Queue full, remove one first.
        if (queuespec.rear == opt.max_notifications) {
            queue_delete(&queuespec, 0);
            queue_align(queuespec);
        }

        Message message = message_create(n, 0, 0, 0, 0, n->expire_timeout);

        queue_insert(&queuespec, message);
        notification_destroy(n);
    }
}
