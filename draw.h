#ifndef DRAW_H_INCLUDED
#define DRAW_H_INCLUDED

#include <cairo.h>
#include "datatypes.h"

void
var_destroy(Variables *destroy);

void
check_fuses(void);

void
draw(void);

void
destroy(cairo_surface_t *sfc);

#endif
