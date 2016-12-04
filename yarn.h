#ifndef YARN_H_INCLUDED
#define YARN_H_INCLUDED

#include "datatypes.h"

Variables
*var_create(char *font,
            int margin, int max, int gap, int upper,
            int bw, char* bc, int rounding, int timeout, int xpos, int ypos,
            int width, int height);

Message
message_create(char *summary, char *body, int textx, int texty, int x, int y, double fuse);

Variables
*var_initialize(void);

void
*run(void *arg);

void
prepare(Notification *n);

#endif
