#include <cairo.h>
#include <cairo-xlib.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <pango/pangocairo.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#include "draw.h"
#include "datatypes.h"
#include "x.h"
#include "parse.h"
#include "queue.h"
#include "cairo.h"

// Interval = 33 = 30fps.
#define INTERVAL 33

// Nanosleep helper.
struct timespec req = {0, INTERVAL*1000000};

/*
 * The queuespec and MessageArray are global so that they can be
 * accessed easily by different threads.
 * The queue is an essential part of yarn.
 */
Queue queuespec = { 0, -1 };
Message MessageArray[QUEUESIZE];

// Free heap memory.
void
var_destroy(Variables *destroy)
{
    assert(destroy != NULL);
    free(destroy);
}

// Check on each message's fuse and delete burnt ones from the queue.
void
check_fuses (void)
{
    for (int i = 0; i < queuespec.rear; i++)
    {
        //printf("Fuse: %Lf, Taking away: %f\n", MessageArray[i].fuse, (double) INTERVAL/1000);
        MessageArray[i].fuse = MessageArray[i].fuse - (double) INTERVAL/1000;
        if (MessageArray[i].fuse <= 0)
            queuespec = queue_delete(queuespec, i);
    }
}

void
draw(Variables *opt)
{
    cairo_surface_t *surface;
    cairo_t *context;
    PangoRectangle sextents;
    PangoRectangle bextents;
    PangoLayout *layout;
    PangoFontDescription *desc;

    // Surface for drawing on, layout for putting the font on.
    surface = cairo_create_x11_surface(opt->xpos, opt->ypos, opt->width, (opt->height + opt->gap) * opt->max);
    context = cairo_create(surface);
    layout = pango_cairo_create_layout (context);

    // Font selection with pango.
    desc = pango_font_description_from_string(opt->font);
    pango_layout_set_font_description(layout, desc);
    pango_font_description_free(desc); // be free my child.

    int running;
    int timepassed = 0, eventpos = 0;
    for (running = 1; running == 1; timepassed++)
    {
        //printf("timepassed: %d\n", timepassed);
        // New group (everything is pre-rendered and then shown at the same time).
        cairo_push_group(context);

        for (int i = 0; i < queuespec.rear; i++)
        {
            //++MessageArray[i].x < 0 ? (MessageArray[i].x = MessageArray[i].x/1.05) : (MessageArray[i].x = 0);
            // If the bar has reached the end, stop it.  Otherwise keep going.
            // Ease aka animate.
            if (MessageArray[i].x < 0)
                MessageArray[i].x = ease(1, 0, timepassed, -300, 0, 50);
            else
                MessageArray[i].x = 0;

            //MessageArray[i].x = ease(MessageArray[i].x, 0, 0);
            MessageArray[i].textx++;

            // Draw each "panel".
            rounded_rectangle(MessageArray[i].x, MessageArray[i].y, opt->width, opt->height, 1, opt->rounding, context, 1,0.5,0,1);

            // Allow markup on the string.
            // Pixel extents are much better for this purpose.
            pango_layout_set_markup(layout, MessageArray[i].summary, -1);
            pango_layout_get_pixel_extents(layout, &sextents, NULL);
            pango_layout_set_markup(layout, MessageArray[i].body, -1);
            pango_layout_get_pixel_extents(layout, &bextents, NULL);
            // Push the text to the soure.
            cairo_set_source_rgba(context, 0,0,0,1);
            cairo_move_to(context, (opt->width - MessageArray[i].textx), MessageArray[i].texty + opt->upper);
            pango_cairo_show_layout(context, layout);

            // Draw over the body with a margin (bit more room for summary).
            cairo_set_source_rgba(context, 1,0.5,0,1);
            cairo_rectangle(context, 0, MessageArray[i].y, opt->margin*2 + sextents.width, opt->height);
            cairo_fill(context);
            // Set and push text to the source.
            pango_layout_set_markup(layout, MessageArray[i].summary, -1);
            cairo_set_source_rgba(context, 0,0,0,1);
            cairo_move_to(context, MessageArray[i].x + opt->margin, MessageArray[i].texty + opt->upper);
            pango_cairo_show_layout(context, layout);

            // Finally clean up empty space to prevent things like text appearing outside the rectangle.
            cairo_set_source_rgba(context, 0.5,0.5,0.5,1);
            cairo_rectangle(context, MessageArray[i].x + opt->width, MessageArray[i].y, opt->width - MessageArray[i].x, opt->height);
            cairo_fill(context);
        }

        cairo_pop_group_to_source(context);

        // Paint over the group.
        cairo_set_operator(context, CAIRO_OPERATOR_SOURCE);
        cairo_paint(context);
        cairo_surface_flush(surface);

        // Clue in for x events (allows to check for hotkeys, stuff like that).
        // They're all mouse events... position is useful for deciding which notification was clicked.
                    //printf("the notification clicked was: %d, at %d\n", notification_no, eventpos);
                    //printf("from: %d\t to: %d\n", MessageArray[notification_no+1].y, MessageArray[notification_no].y);
        int notification_no = 0;
        switch (check_x_event(surface, &eventpos, 0))
        {
            case -3053:
                // Expose event.
                break;
            case -1:
                // Find out which notification was clicked and delete it.
                notification_no = get_notification(eventpos, opt->height+opt->gap, opt->max);

                // Move below notifications up, if there are any.
                if (in_queue(queuespec) != 1) {
                    for (int i = notification_no; i < queuespec.rear; i++) {
                        move_message(MessageArray[i+1].y, MessageArray[i].y, &MessageArray[i+1]);
                    }

                    MessageArray[notification_no].visible = 0;
                    //queuespec = queue_delete(queuespec, notification_no);
                } else {
                    queuespec = queue_delete(queuespec, notification_no);
                }

                break;
        }

        // Check message fuses/remove from queue accordingly.
        check_fuses();

        // If the queue is empty, kill this thread basically.
        if (in_queue(queuespec) == 0) {
            running = 0;
        }

        // Finally sleep ("animation").
        nanosleep(&req, &req);
    }

    // Destroy once done.
    g_object_unref(layout);
    pango_cairo_font_map_set_default(NULL);
    cairo_destroy(context);

    var_destroy(opt);

    destroy(surface);
}
