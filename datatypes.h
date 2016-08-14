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

// TODO, transfer variables and notification into one?  i kind of like them separate.

struct notification {
    char *app_name;
    char *summary;
    char *body;
    char *app_icon;
    int   replaces_id;
    int   expire_timeout;
};
