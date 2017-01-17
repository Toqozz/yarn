#ifndef CAIRO_H_INCLUDED
#define CAIRO_H_INCLUDED

#include <cairo.h>
#include "datatypes.h"

struct Wrapper {
    int from;
    int to;
    Message *message;
};

void
rounded_rectangle (cairo_t *context,
                  double x, double y,
                  double width, double height,
                  double aspect, double corner_radius,
                  double r, double g,
                  double b, double a);

void
draw_panel(cairo_t *context, Color bd, Color bg, int x, int y, int w, int h, int bw);

void
draw_panel_shadow(cairo_t *context, Color c, int x, int y, int w, int h);

int
ease (int animation, int index, int curtime, double s, double e, double d);

void
*move_message_thread (void *wrapper);

void
move_message (int from, int to, Message *message);

#endif
