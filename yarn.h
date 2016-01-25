extern void run();

struct notification
{
    char *app_name;
    int  replaces_id;
    char *app_icon;
    char *summary;
    char *body;
    int  expire_timeout;
};
