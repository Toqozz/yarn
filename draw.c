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
#include "cairo.h"
#include "x.h"
#include "parse.h"
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
extern Variables opt;

/* Check on each message's timeouts and delete burnt ones from the queue */
void
check_fuses(void)
{
    for (int i = 0; i < queuespec.rear; i++)
    {
        //printf("Fuse: %Lf, Taking away: %f\n", MessageArray[i].fuse, (double) INTERVAL/1000);
        MessageArray[i].fuse = MessageArray[i].fuse - (double)INTERVAL/1000;
        if (MessageArray[i].fuse <= 0) {
            queue_delete(&queuespec, i);
            queue_align(queuespec);
        }
    }
}

/* Clear surface to a blank/fresh state */
void
draw_clear_surface(cairo_t *context)
{
    cairo_save(context);

    cairo_set_operator(context, CAIRO_OPERATOR_CLEAR);
    cairo_paint(context);

    cairo_restore(context);
}

/* Draw/update everything in a loop */
void
draw(void)
{
    cairo_surface_t *surface;
    cairo_t *context;
    PangoRectangle sextents;
    PangoLayout *layout;
    PangoFontDescription *desc;

    // Figure out the total surface area that will be drawn on.
    int width = opt.width + abs(opt.shadow_xoffset);
    int height = ((opt.height * opt.max_notifications) + (opt.gap * (opt.max_notifications - 1)) + abs(opt.shadow_yoffset));
    // Figure out where exactly the surface needs to be positioned (can change with different shadow geometry).
    int xpos = opt.xpos + (opt.shadow_xoffset < 0 ? opt.shadow_xoffset : 0);
    int ypos = opt.ypos + (opt.shadow_yoffset < 0 ? opt.shadow_yoffset : 0);

    // Surface for drawing on, layout for putting the font on.
    //surface = cairo_create_x11_surface(opt.xpos, opt.ypos, width, height);
    surface = cairo_create_x11_surface(xpos, ypos, width, height);
    context = cairo_create(surface);
    layout = pango_cairo_create_layout(context);

    // Font selection with pango.
    desc = pango_font_description_from_string(opt.font);
    pango_layout_set_font_description(layout, desc);
    pango_font_description_free(desc); // be free my child.
    pango_layout_set_ellipsize(layout, PANGO_ELLIPSIZE_END);

    int running, i;
    for (running = 1; running == 1;)
    {
        // If we need to redraw, clear the surface and redraw notifications.
        if (opt.redraw)
        {
            draw_clear_surface(context);

            for (i = 0; i < queuespec.rear; i++)
            {
                pango_layout_set_markup(layout, MessageArray[i].summary, -1);
                pango_layout_set_width(layout, opt.summary_width*PANGO_SCALE);
                // Pixel extents are much better for this purpose.
                pango_layout_get_pixel_extents(layout, &sextents, NULL);
                MessageArray[i].swidth = sextents.width;

                draw_panel_shadow(context, opt.shadow_color,
                        MessageArray[i].x + opt.shadow_xoffset,
                        MessageArray[i].y + opt.shadow_yoffset,
                        opt.width, opt.height);
                draw_panel(context, opt.bdcolor, opt.bgcolor, MessageArray[i].x, MessageArray[i].y, opt.width, opt.height, opt.bw);

                pango_layout_set_markup(layout, MessageArray[i].summary, -1);
                pango_layout_set_width(layout, opt.summary_width*PANGO_SCALE);
                cairo_set_source_rgba(context, opt.summary_color.red, opt.summary_color.green, opt.summary_color.blue, opt.summary_color.alpha);
                cairo_move_to(context, MessageArray[i].x + opt.margin + opt.bw, MessageArray[i].texty + opt.overline);
                pango_cairo_show_layout(context, layout);
            }

            opt.redraw = 0;
        }

        // New group (everything is pre-rendered and then shown at the same time).
        cairo_push_group(context);

        // Draw body text in each panel.
        for (i = 0; i < queuespec.rear; i++)
        {
            // TODO, opt.margin*3, add option for the *3 or have a separate option probably.
            // Progress the text if it has not reached the end yet.
            MessageArray[i].textx < (((opt.width - opt.bw*2) - MessageArray[i].swidth) - opt.margin*3) ? MessageArray[i].textx++ : false;

            // Make sure that we dont draw out of the box after this point.
            cairo_save(context);

            cairo_set_operator(context, CAIRO_OPERATOR_SOURCE);
            cairo_rectangle(context, MessageArray[i].x + (2*opt.margin) + MessageArray[i].swidth + opt.bw,
                    MessageArray[i].y + opt.bw,
                    ((opt.width - opt.bw*2) - opt.margin*2) - MessageArray[i].swidth,
                    opt.height-opt.bw*2);
            cairo_set_source_rgba(context, opt.bgcolor.red, opt.bgcolor.green, opt.bgcolor.blue, opt.bgcolor.alpha);
            cairo_fill_preserve(context);
            cairo_clip(context);

            // Push the body to the soure.
            pango_layout_set_markup(layout, MessageArray[i].body, -1);
            pango_layout_set_width(layout, opt.body_width*PANGO_SCALE);
            cairo_set_source_rgba(context, opt.body_color.red, opt.body_color.green, opt.body_color.blue, opt.body_color.alpha);
            cairo_move_to(context, (opt.width - MessageArray[i].textx), MessageArray[i].texty + opt.overline); //TODO, overline can be baked into texty
            pango_cairo_show_layout(context, layout);

            // We should be able to draw out of the box next time.
            cairo_restore(context);
        }

        cairo_pop_group_to_source(context);

        // Paint over the group.
        cairo_paint(context);
        cairo_surface_flush(surface);

        // Clue in for x events (allows to check for hotkeys, stuff like that).
        // They're all mouse events... position is useful for deciding which notification was clicked.
        int notification_no = 0, eventpos = 0;
        switch (check_x_event(surface, &eventpos, 0))
        {
            case -3053:         // exposed.
                break;
            case -1:
                // Find out which notification was clicked.
                notification_no = parse_xy_to_notification(eventpos, opt.height+opt.gap, opt.max_notifications);

                // Delete and move below notifications up, if there are any.
                if (in_queue(queuespec) != 1) {
                    queue_delete(&queuespec, notification_no);
                    queue_align(queuespec);
                } else {
                    queue_delete(&queuespec, notification_no);
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
    pango_cairo_font_map_set_default(NULL);
    g_clear_object(&layout);
    cairo_destroy(context);

    destroy(surface);
}
