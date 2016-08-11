#ifndef CAIRO_H_INCLUDED
#define CAIRO_H_INCLUDED

void
rounded_rectangle(double x, double y,
                  double width, double height,
                  double aspect, double corner_radius,
                  cairo_t *context,
                  double r, double g,
                  double b, double a);

#endif
