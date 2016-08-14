#ifndef DRAW_H_INCLUDED
#define DRAW_H_INCLUDED

void queue_insert(struct MessageInfo message);

void queue_delete(int position);

_Bool queue_empty();

void
parse(char *wxh, int *xpos, int *ypos, int *width, int *height);

struct Variables
*var_create(char *font,
            int margin, int number, int upper,
            int gap, int rounding, int timeout, int xpos, int ypos,
            int width, int height);

struct MessageInfo
message_create(char *string, int textx, int texty, int x, int y, double fuse);

void
var_destroy(struct Variables *destroy);

void
draw(struct Variables *info, char *string);

void destroy(cairo_surface_t *sfc);

#endif
