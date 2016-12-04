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
rounded_rectangle (double x, double y,
                  double width, double height,
                  double aspect, double corner_radius,
                  cairo_t *context,
                  double r, double g,
                  double b, double a);

int
ease (int animation, int index, int curtime, double s, double e, double d);

void
*move_message_thread (void *wrapper);

void
move_message (int from, int to, Message *message);

void
queue_align();

#endif
