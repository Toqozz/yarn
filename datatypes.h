#ifndef DATATYPES_H_INCLUDED
#define DATATYPES_H_INCLUDED

typedef struct Config {
    int max_notifications;
    int gap;

    const char *geometry;

    const char *summary_color;
    const char *body_color;
    const char *fgcolor;
    const char *bgcolor;
    const char *bdcolor;

    double interval;

    int timeout;
    int scroll_speed;

    const char *font;

    int margin;
    int overline;
    int bw;
    int rounding;
} Config;

typedef struct Variables {
    const char *font;
    const char *bc;

    double timeout;

    int   margin;
    int   max;
    int   gap;
    int   overline;
    int   bw;
    int   rounding;
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

typedef struct Color {
    double red;
    double green;
    double blue;
    double alpha;
} Color;

#endif
