#ifndef YARN_H_INCLUDED
#define YARN_H_INCLUDED

#include "datatypes.h"

void
notify_setup(Notification *n);

void
notification_destroy(Notification *n);

Message
message_create(Notification *n, int textx, int texty, int x, int y, double fuse);

void
message_destroy(Message *m);

void
*run(void *arg);

void
prepare(Notification *n);

#endif
