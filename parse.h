#ifndef PARSE_H_INCLUDED
#define PARSE_H_INCLUDED

void
parse(const char *wxh, int *xpos, int *ypos, int *width, int *height);

int
get_notification(int ypos, int height, int max_notifications);

#endif
