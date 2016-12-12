#ifndef DATATYPES_H_INCLUDED
#define DATATYPES_H_INCLUDED

typedef struct Color {
    double red;
    double green;
    double blue;
    double alpha;
} Color;

typedef struct Queue {
    int rear;
    int front;
} Queue;

typedef struct Notification {
    char *app_name;
    char *summary;
    char *body;
    char *app_icon;
    int   replaces_id;
    int   expire_timeout;
} Notification;

typedef struct Config {
    int max_notifications;
    int gap;

    const char *geometry;

    const char *summary_color;
    const char *body_color;
    const char *bgcolor;
    const char *bdcolor;

    const char *font;

    double interval;
    double timeout;
    double rounding;

    int scroll_speed;
    int margin;
    int overline;
    int bw;
} Config;

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

typedef struct Variables {
    int   max_notifications;
    int   gap;

    int   xpos;
    int   ypos;
    int   width;
    int   height;

    Color summary_color;
    Color body_color;
    Color bgcolor;
    Color bdcolor;

    const char *font;

    double interval;
    double timeout;
    double rounding;

    int   scroll_speed;
    int   margin;
    int   overline;
    int   bw;
} Variables;

#endif
