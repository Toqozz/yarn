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
//#include <pthread.h>

#include "datatypes.h"
#include "x.h"
#include "cairo.h"
#include "draw.h"
#include "queue.h"

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
message_create(char *string, int textx, int texty, int x, int y, double fuse)
{
    Message message;

    message.string = string;
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

void
draw(Variables *opt, Message message)
{
    cairo_surface_t *surface;
    cairo_t *context;
    PangoRectangle extents;
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
    for (running = 1; running == 1; timepassed++)
    {

        // New group (everything is pre-rendered and then shown at the same time).
        cairo_push_group(context);

        // Clear the surface.
        //cairo_set_operator(context, CAIRO_OPERATOR_CLEAR);
        //cairo_paint(context);
        //cairo_set_operator(context, CAIRO_OPERATOR_OVER);

        for (int i = 0; i < queuespec.rear; i++)
        {
            // If the bar has reached the end, stop it.  Otherwise keep going.
            ++MessageArray[i].x < 0 ? ((MessageArray[i].x = MessageArray[i].x/1.05)) : ((MessageArray[i].x = 0));
            MessageArray[i].textx++;

            // Draw each "panel".
            rounded_rectangle(MessageArray[i].x, MessageArray[i].y, opt->width, opt->height, 1, opt->rounding, context, 1,0.5,0,1);

            // Allow markup on the string.
            // Pixel extents are much better for this purpose.
            pango_layout_set_markup(layout, MessageArray[i].string, -1);
            pango_layout_get_pixel_extents(layout, &extents, NULL);

            // Push the text to the soure.
            cairo_set_source_rgba(context, 0,0,0,1);
            cairo_move_to(context, MessageArray[i].textx - extents.width, MessageArray[i].y + opt->upper);
            pango_cairo_show_layout(context, layout);

            // Draw over the text with a margin.
            cairo_set_source_rgba(context, 1,0.5,0,1);
            cairo_rectangle(context, 0, MessageArray[i].y, opt->margin, opt->height);
            cairo_fill(context);
        }

        // Pop the group.
        cairo_pop_group_to_source(context);

        // Paint it.
        cairo_set_operator(context, CAIRO_OPERATOR_SOURCE);
        cairo_paint(context);
        cairo_surface_flush(surface);

        // Clue in for x events (allows to check for hotkeys, stuff like that).
        switch (check_x_event(surface, 0))
        {
            case -3053:
                //fprintf(stderr, "exposed\n");
                break;
            case 0xff1b:    // esc
            case -1:        // left mouse button
                fprintf(stderr, "left mouse button\n");
                running = 0;
                break;
        }

        for (int i = 0; i < queuespec.rear; i++)
        {
            printf("Fuse: %Lf, Taking away: %f\n", MessageArray[i].fuse, (double) INTERVAL/1000);
            MessageArray[i].fuse = MessageArray[i].fuse - (double) INTERVAL/1000;
            if (MessageArray[i].fuse <= 0)
                queuespec = queue_delete(queuespec, i);
        }

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



/*
    int  opt;
    while ((opt = getopt(argc, argv, "hf:m:n:u:g:r:t:d:")) != -1) {
        switch(opt)
        {
            case 'h': help(); break;
            case 'f': font = optarg;  break;
            case 'm': margin = strtol(optarg, NULL, 10); break;
            case 'n': number = strtol(optarg, NULL, 10); break;
            case 'u': upper = strtol(optarg, NULL, 10);  break;
            case 'g': gap = strtol(optarg, NULL, 10); break;
            case 'r': rounding = strtol(optarg, NULL, 10); break;
            case 't': timeout = strtod(optarg, NULL); break;
            case 'd': dimensions = optarg; break;
            default: help();
        }
    }

    // Done with strings -- program ending.
    for (i = 0; i < number; i++)
        free(strings[i]);

    return(0);
}
*/
