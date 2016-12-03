#ifndef DATATYPES_H_INCLUDED
#define DATATYPES_H_INCLUDED

typedef struct Variables {
    char *font;
    int   margin;
    int   max;
    int   upper;
    int   gap;
    int   rounding;
    int   timeout;
    int   xpos;
    int   ypos;
    int   width;
    int   height;
} Variables;

typedef struct Queue {
    int rear;
    int front;
} Queue;

typedef struct Message {
    char *summary;
    char *body;
    int   textx;
    int   texty;
    int   x;
    int   y;
    int   visible;
    long double fuse;
} Message;

typedef struct Notification {
    char *app_name;
    char *summary;
    char *body;
    char *app_icon;
    int   replaces_id;
    int   expire_timeout;
} Notification;

#endif
