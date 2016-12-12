#ifndef YARN_H_INCLUDED
#define YARN_H_INCLUDED

#include "datatypes.h"

Variables
*var_create(void);

Config
*cfg_create(void);

Message
message_create(char *summary, char *body, int textx, int texty, int x, int y, double fuse);

Variables
*var_initialize(void);

void
*run(void *arg);

void
prepare(Notification *n);

#endif
