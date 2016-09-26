#ifndef DRAW_H_INCLUDED
#define DRAW_H_INCLUDED

Variables
*var_create(char *font,
            int margin, int number, int upper,
            int gap, int rounding, int timeout, int xpos, int ypos,
            int width, int height);

Message
message_create(char *summary, char *body, int textx, int texty, int x, int y, double fuse);

void
var_destroy(Variables *destroy);

void
draw(Variables *info, Message message);

void
destroy(cairo_surface_t *sfc);

#endif
