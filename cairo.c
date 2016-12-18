#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include <stdio.h>

#include <cairo.h>
#include <cairo-xlib.h>

#include "cairo.h"
#include "datatypes.h"

// Modified from http://cairographics.org/samples/rounded_rectangle/
void rounded_rectangle(cairo_t *context,
                       double x, double y,
                       double width, double height,
                       double aspect, double corner_radius,
                       double r, double g,
                       double b, double a)
{
    double radius = corner_radius / aspect;
    double degrees = M_PI / 180.0;

    cairo_new_sub_path(context);
    cairo_arc(context, x + width - radius, y + radius, radius, -90 * degrees, 0 * degrees);
    cairo_arc(context, x + width - radius, y + height - radius, radius, 0 * degrees, 90 * degrees);
    cairo_arc(context, x + radius, y + height - radius, radius, 90 * degrees, 180 * degrees);
    cairo_arc(context, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
    cairo_close_path(context);

    cairo_set_source_rgba(context, r, g, b, a);
    cairo_fill(context);
}

void draw_panel(cairo_t *context, Color bd, Color bg, int x, int y, int w, int h, int bw)
{
    cairo_set_source_rgba(context, bd.red, bd.green, bd.blue ,bd.alpha);
    cairo_rectangle(context, x, y, w, h);
    cairo_fill(context);

    cairo_set_source_rgba(context, bg.red, bg.blue, bg.green, bg.alpha);
    cairo_rectangle(context, x + bw, y + bw, w - bw*2, h - bw*2);
    cairo_fill_preserve(context);
    //cairo_clip(context);
}

void draw_panel_shadow(cairo_t *context, Color c, int x, int y, int w, int h)
{
    //cairo_set_operator(context, CAIRO_OPERATOR_OVER);
    cairo_set_source_rgba(context, c.red, c.green, c.blue, c.alpha);
    //for (int i = 4; i > 0; i--) {
    cairo_rectangle(context, x, y, w, h);
    cairo_fill(context);
    //}
}

// Ease the transition between two numbers -> ""animation""
// p = period to execute the animation over (ms), s = start value,  e = end value, d = duration.
// TODO how the FUCK do i make this readable.
int
ease (int animation, int index, int curtime, double s, double e, double d)
{
    // Yes, offset could be calculated in here,
    // However, it would require an if statement to be checked every time a loop ran.

    // How far along we are in the equation (0.01 - 1). (percentage).
    double p = (double)curtime/d;
    //printf("p: %f\t", p);
    // Get the 'gap' between the start and the end.
    double g = fabs(s - e);
    //printf("g: %f\t", g);
    //printf("s: %f\n", s);

    double temp = 0.0;

    // 0: circular ease in.
    // 1: circular ease out.
    // 2: circular ease in out.
    // 3: bounce ease out.
    // 4: sine ease in.
    // 5: sine ease out
    switch(animation) {
    case 0:
        //return 1 - sqrt(1 - (p*p))*-s;
        temp = (1 - sqrt(1 - (p*p))) * g;
        break;

    case 1:
        temp = (sqrt((2 - p)*p)) * g;
        break;

    case 2:
        if (p < 0.5)
            temp = (0.5 * (1 - sqrt(1 - 4 * (p * p)))) * g;
        else
            temp = (0.5 * (sqrt(-((2 * p) -3) * ((2 * p) -1)) + 1)) * g;
        break;

    case 3:
        if (p < 4/11.0)
            temp = ((121 * p * p)/16.0) * g;
        else if (p < 8/11.0)
            temp = ((363/40.0 * p * p) - (99/10.0 * p) + 17/5.0) * g;
        else if (p < 9/10.0)
            temp = ((4356/361.0 * p * p) - (35442/1805.0 * p) + 16061/1805.0) * g;
        else
            temp = ((54/5.0 * p * p) - (513/25.0 * p) + 268/25.0) * g;
        break;

    case 4:
        temp = (sin((p - 1) * M_PI_2) + 1) * g;

    case 5:
        temp = (sin(p * M_PI_2)) * g;

    case 6:
        temp = -150.38411;

    default:
        temp = 0;
    }

    if (e > s)
        return (int)round(temp + s);
    else
        return (int)round(s - temp);
}

// TODO, change this to work off positions -> similar to 'align'.
void
*move_message_thread (void *arg_wrapper)
{
    int ms = 33;
    struct timespec t = {0, ms*1000000};

    struct Wrapper *wrapper = (struct Wrapper*)arg_wrapper;

    int running;
    int timepassed = 0;
    for (running = 1; running == 1; timepassed++)
    {
        if (wrapper->message->y != wrapper->to) {
            //printf("y = %d\n", wrapper->message->y);
            wrapper->message->y = ease(1, 0, timepassed, wrapper->from, wrapper->to, 33);
            wrapper->message->texty = ease(1, 0, timepassed, wrapper->from, wrapper->to, 33);
        } else {
            printf("done.\n");
            wrapper->message->y = wrapper->to;
            wrapper->message->texty = wrapper->to;
            running = 0;
        }

        nanosleep(&t, &t);
    }

    return NULL;
}

// Move a notification to somewhere else along the stack cleanly (vertically)..
void
move_message (int from, int to, Message *message)
{
    pthread_t move_msg_thread;

    //TODO free this.
    struct Wrapper *arg_wrapper = malloc(sizeof(struct Wrapper));
    arg_wrapper->from = from;
    arg_wrapper->to = to;
    arg_wrapper->message = message;

    pthread_create(&move_msg_thread, NULL, move_message_thread, arg_wrapper);
    pthread_join(move_msg_thread, NULL);
}

