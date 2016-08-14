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

typedef struct Queues {
    int rear;
    int front;
} Queue;
typedef struct Messages {
    char *string;
    int   textx;
    int   texty;
    int   x;
    int   y;
    long double fuse;
} Message;

// TODO, transfer variables and notification into one?  i kind of like them separate.

typedef struct Notifications {
    char *app_name;
    char *summary;
    char *body;
    char *app_icon;
    int   replaces_id;
    int   expire_timeout;
} Notification;
