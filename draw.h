#ifndef DRAW_H_INCLUDED
#define DRAW_H_INCLUDED

#include <cairo.h>
#include "datatypes.h"

void
check_fuses(void);

void
draw_setup_toolbox(Toolbox *box);

void
draw_clear_surface(cairo_t *context);

void
draw(void);

void
destroy(cairo_surface_t *sfc);

#endif
