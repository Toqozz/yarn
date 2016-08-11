struct Variables {
    char *font;
    int   margin;
    int   number;
    int   upper;
    int   gap;
    int   rounding;
    int   timeout;
    int   xpos;
    int   ypos;
    int   width;
    int   height;
};

struct MessageInfo {
    char *string;
    int   textx;
    int   texty;
    int   x;
    int   y;
    long double fuse;
};

struct notification {
    char *app_name;
    int  replaces_id;
    char *app_icon;
    char *summary;
    char *body;
    int  expire_timeout;
};
