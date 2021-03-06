#ifndef PARSE_H_INCLUDED
#define PARSE_H_INCLUDED

#include "datatypes.h"

void
parse_geometry(const char *wxh, int *xpos, int *ypos, int *width, int *height);

int
parse_xy_to_notification(int ypos, int height, int max_notifications);

Color
parse_hex_to_rgba(const char *hex_color);

char *
parse_strip_markup(const char *text);

char *
parse_quote_markup(const char *text);

int
parse_offset_value(int value);

#endif
