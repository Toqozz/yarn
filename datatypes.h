#ifndef DATATYPES_H_INCLUDED
#define DATATYPES_H_INCLUDED

#include <cairo-xlib.h>
#include <pango/pangocairo.h>
#include <time.h>

typedef struct Toolbox {
    cairo_t              *ctx;
    cairo_surface_t      *sfc;
    PangoLayout          *lyt;
    PangoFontDescription *dsc;
    PangoRectangle        sextents;
    PangoRectangle        bextents;
} Toolbox;

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
    char *app_icon;
    char *summary;
    char *body;
    int   replaces_id;
    int   expire_timeout;
} Notification;

typedef struct Config {
    int max_notifications;
    int gap;

    const char *geometry;

    int shadow;
    int shadow_xoffset;
    int shadow_yoffset;

    int summary_width;
    int body_width;

    const char *summary_color;
    const char *body_color;
    const char *shadow_color;
    const char *bgcolor;
    const char *bdcolor;

    const char *font;
    const char *summary_markup;
    const char *body_markup;

    double interval;
    double timeout;
    double rounding;

    int scroll_speed;
    int lmargin;
    int mmargin;
    int rmargin;
    int overline;
    int bw;
} Config;

typedef struct Message {
    char *summary;
    char *body;
    int   swidth;
    int   bwidth;
    int   textx;
    int   texty;
    int   x;
    int   y;
    int   redraw;

    int   total_bw;
    int   total_bheight;
    int   total_swidth;
    int   total_bwidth;
    int   bwidth_startx;
    int   bwidth_starty;

    long double fuse;
} Message;

typedef struct Variables {
    int   max_notifications;
    int   gap;

    int   xpos;
    int   ypos;
    int   width;
    int   height;

    int   shadow;
    int   shadow_xoffset;
    int   shadow_yoffset;

    int summary_width;
    int body_width;

    Color summary_color;
    Color body_color;
    Color shadow_color;
    Color bgcolor;
    Color bdcolor;

    const char *font;

    struct timespec tspec;

    double interval;
    double timeout;
    double rounding;

    int   scroll_speed;
    int   lmargin;
    int   mmargin;
    int   rmargin;
    int   overline;
    int   bw;
} Variables;


#endif
