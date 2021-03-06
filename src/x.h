#ifndef X_H_INCLUDED
#define X_H_INCLUDED

#include <X11/Xlib.h>
#include <cairo.h>

#include "datatypes.h"

// Map window and return surface for that window.
cairo_surface_t *
cairo_create_x11_surface(int x, int y, int w, int h);

int
check_x_event(cairo_surface_t *sfc, int *position, int block);

void
x_resize_window(cairo_surface_t *sfc, int x, int y);

int
yarn_destroy(Toolbox t);

#endif
