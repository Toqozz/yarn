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

#include "x.h"
#include "parse.h"
#include "cairo.h"
#include "datatypes.h"
#include "queue.h"
#include "draw.h"

// Interval = 33 = 30fps.
#define INTERVAL 16.5

// Nanosleep helper.
struct timespec req = {0, INTERVAL*1000000};

/*
 * The queuespec and MessageArray are global so that they can be
 * accessed easily by different threads.
 * The queue is an essential part of yarn.
 */
Queue queuespec = { 0, -1 };
Message MessageArray[QUEUESIZE];

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
    message.fuse = fuse;

    return message;
}

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

// Main animation of the bar function.
// TODO insert multiple possible animations.
// TODO do the real ease -- [current] and [to] arguments.
int
ease (int xpos, int min, int animation)
{
    int temp = 0;

    switch(animation) {
    case 0:
        if (++xpos < min) {
            xpos = xpos/1.05;
            temp = xpos;
        }
        break;
    case 1:
        break;
    default:
        break;
    }

    return temp;
}

// p = period to execute the animation over (ms), s = start value,  e = end value, d = duration.
int
new_ease (int animation, int index, int curtime, double s, double e, double d)
{
    // How far along we are in the equation (0.01 - 1). (percentage).
    double p = 0.0;
    //printf("temp: %f\n", temp);
    //printf("equation: %f\n", 1.0 - sqrt(1.0 - (temp*temp))*-s);

    switch(animation) {
    // circular ease in.
    case 0:
        p = (double)curtime/d;
        return 1 - sqrt(1 - (p*p))*-s;
        break;
    // circular ease out.
    case 1:
        p = (double)curtime/d;
        return sqrt((2 - p)*p)*-s + s;
        break;
    // circular ease in out.
    case 2:
        p = (double)curtime/d;
        if (p < 0.5)
            return 0.5 * (1 - sqrt(1 - 4 * (p * p)))*-s + s;
        else
            return 0.5 * (sqrt(-((2 * p) -3) * ((2 * p) -1)) + 1)*-s + s;
        break;
    // bounce ease out.
    case 3:
        p = (double) curtime/d;
        if (p < 4/11.0)
            return ((121 * p * p)/16.0)*-s + s;
        else if (p < 8/11.0)
            return ((363/40.0 * p * p) - (99/10.0 * p) + 17/5.0)*-s + s;
        else if (p < 9/10.0)
            return ((4356/361.0 * p * p) - (35442/1805.0 * p) + 16061/1805.0)*-s + s;
        else
            return ((54/5.0 * p * p) - (513/25.0 * p) + 268/25.0)*-s + s;
        break;
    default:
        break;
    }

    return p;
}

void
draw(Variables *opt, Message message)
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
    int timepassed;
    int eventpos;
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
                MessageArray[i].x = new_ease(2, 0, timepassed, -300, 0, 100);
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
            //cairo_move_to(context, (MessageArray[i].textx - bextents.width) + sextents.width, MessageArray[i].y + opt->upper);
            cairo_set_source_rgba(context, 0,0,0,1);
            cairo_move_to(context, (opt->width - MessageArray[i].textx), MessageArray[i].texty + opt->upper);
            pango_cairo_show_layout(context, layout);

            // Draw over the text with a margin.
            // This is actually enough room for the summary + the margin.
            cairo_set_source_rgba(context, 1,0.5,0,1);
            cairo_rectangle(context, 0, MessageArray[i].y, opt->margin*2 + sextents.width, opt->height);
            cairo_fill(context);

            // Set and push text to the soure.
            //printf("opt->height: %d, extents.height: %d, opt->height-extents.height = %d / 2 = %f\n", opt->height, sextents.height, (opt->height - sextents.height),MessageArray[i].y + (double)(opt->height - sextents.height)/2);
            pango_layout_set_markup(layout, MessageArray[i].summary, -1);
            cairo_set_source_rgba(context, 0,0,0,1);
            cairo_move_to(context, MessageArray[i].x + opt->margin, MessageArray[i].texty + opt->upper);
            pango_cairo_show_layout(context, layout);

            // Fill empty space to prevent things like text appearing outside the rectangle.
            cairo_set_source_rgba(context, 0.5,0.5,0.5,1);
            cairo_rectangle(context, MessageArray[i].x + opt->width, MessageArray[i].y, opt->width - MessageArray[i].x, opt->height);
            cairo_fill(context);
        }

        // Pop the group.
        cairo_pop_group_to_source(context);

        // Paint it.
        cairo_set_operator(context, CAIRO_OPERATOR_SOURCE);
        cairo_paint(context);
        cairo_surface_flush(surface);

        // Clue in for x events (allows to check for hotkeys, stuff like that).
        // They're all mouse events... position is useful for deciding which notification was clicked.
        switch (check_x_event(surface, &eventpos, 0))
        {
            case -3053:
                //fprintf(stderr, "exposed\n");
                break;
            case 0xff1b:    // esc
            case -1:        // left mouse button
                // Find out which notification was clicked and delete it.
                printf("the notification clicked was: %d, at %d\n", get_notification(eventpos, opt->height+opt->gap, opt->max), eventpos);
                queuespec = queue_delete(queuespec, get_notification(eventpos, opt->height+opt->gap, opt->max)-1);
                break;
        }

        // Check message fuses/remove from queue accordingly.
        check_fuses();

        // If the queue is empty, kill this thread basically.
        if (queue_empty(queuespec))
            running = 0;

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
