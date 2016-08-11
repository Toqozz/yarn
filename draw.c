#include <cairo.h>
#include <cairo-xlib.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <pango/pangocairo.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>
//#include <pthread.h>

#include "x.h"
#include "cairo.h"
#include "datatypes.h"

// TODO replace with config options?
// Interval = 33 = 30fps.
#define QUEUESIZE 100
#define INTERVAL 33

// nanosleep and queue for messages.
struct timespec req = {0, INTERVAL*1000000};
struct MessageInfo MessageQueue[QUEUESIZE];

// Fifo queue.
int   rear = 0;
int   front = -1;
int   i;
void queue_insert(struct MessageInfo message)
{
    if (rear == QUEUESIZE-1)
        printf("Queue is full, skipped.\n");
    else
    {
        // If queue is initially empty.
        if (front == -1)
            front = 0;

        // Add item to array.
        // There is a new item, the end is pushed back.
        MessageQueue[rear++] = message;
    }
}
void queue_delete(int position)
{
    // Nothing in queue.
    if (front == - 1)
        printf("Queue is empty -- nothing to delete.\n");

    // Move each item down one.
    else
    {
        for (i = position; i < rear-1; i++)
            MessageQueue[i] = MessageQueue[i+1];
        rear--;
    }
}
bool queue_empty()
{
    if (front == rear)
        return true;
    else
        return false;
}

// Please save me, this time I cannot run.
void
help()
{
    printf("basic window making and text printing.\n"
           "usage: cairo [ -h | -f | -m | -u | -d | -n | -g ]\n"
           "        -h Show this help\n"
           "        -f Font to use\n"
           "        -m Distance from the left side of the window\n"
           "        -u Distance text is placed from the top of the window\n"
           "        -d Dimensions (WxH+X+Y)\n"
           "        -n Number of messages\n"
           "        -g Gap between messages\n"
           "        -t Timeout for the message(s)\n"
           );
    exit(0);
}

void
parse(char *wxh, int *xpos, int *ypos, int *width, int *height)
{
    char *x;
    char *y;
    char *w;
    char *h;

    // We need something that is mutable.
    // Remember pointer position (it's being mutated).
    char *dupe = strdup(wxh);
    char *point = dupe;

    // If memeory not got.
    assert(dupe != 0);

    // ex. "500x10+20+30"
    w = strsep(&dupe, "x");         // w = "500", dupe = "10+20+30"
    h = strsep(&dupe, "+");         // h = "10", dupe = "20+30"
    x = strsep(&dupe, "+");         // x = "20", dupe = "30"
    y = strsep(&dupe, "+");         // y = "30", dupe = ""

    // Change variables 'globally' in memory. (*width and *height have memory addresses from main.).
    *xpos = strtol(x, NULL, 10);   // change value xpos is pointing to to something else.
    *ypos = strtol(y, NULL, 10);
    *width = strtol(w, NULL, 10);
    *height = strtol(h, NULL, 10);

    // Free pointers after everything is done with it.
    free(point);        // Points to dupe's old memory footprint?
    free(dupe);
}

// Create a struct on the heap.
struct Variables
*var_create(char *font,
            int margin, int number, int upper,
            int gap, int rounding, int timeout, int xpos, int ypos,
            int width, int height)
{
    struct Variables *info = malloc(sizeof(struct Variables));
    assert(info != NULL);

    info->font = font;
    info->margin = margin;
    info->number = number;
    info->upper = upper;
    info->gap = gap;
    info->rounding = rounding;
    info->timeout = timeout;
    info->xpos = xpos;
    info->ypos = ypos;
    info->width = width;
    info->height = height;

    return info;
}

// Create messages on the stack.
struct MessageInfo
message_create(char *string, int textx, int texty, int x, int y, double fuse)
{
    struct MessageInfo message;

    message.string = string;
    message.textx = textx;
    message.texty =texty;
    message.x = x;
    message.y = y;
    message.fuse = fuse;

    return message;
}

// Free heap memory.
void
var_destroy(struct Variables *destroy)
{
    assert(destroy != NULL);
    free(destroy);
}

void
runner(struct Variables *info, char *strings[])
{
    cairo_surface_t *surface;
    cairo_t *context;
    PangoRectangle extents;
    PangoLayout *layout;
    PangoFontDescription *desc;

    // TODO: REPLACE with detection:
    // if new message, message create on the end of the array.

    // Surface for drawing on, layout for putting the font on.
    surface = cairo_create_x11_surface(info->xpos, info->ypos, info->width, (info->height + info->gap) * info->number);
    context = cairo_create(surface);
    layout = pango_cairo_create_layout (context);

    // Font selection with pango.
    desc = pango_font_description_from_string(info->font);
    pango_layout_set_font_description(layout, desc);
    pango_font_description_free(desc); // be free my child.

    //struct MessageInfo messages[info->number];
    int i;
    for (i = 0; i < info->number; i++){
        // string, text x, text y, x, y, fuse.
        queue_insert(message_create(strings[i], 0, 0, -info->width-1, i*(info->height + info->gap), info->timeout));
        printf("String: %s, Fuse: %Lf\n", MessageQueue[i].string, MessageQueue[i].fuse);
    }


    int running;
    int timepassed;
    for (running = 1; running == 1; timepassed++)
    {
        // Clear the surface.
        cairo_set_operator(context, CAIRO_OPERATOR_CLEAR);
        cairo_paint(context);
        cairo_set_operator(context, CAIRO_OPERATOR_OVER);

        // New group (everything is pre-rendered and then shown at the same time).
        cairo_push_group(context);

        for (i = 0; i < rear; i++)
        {
            // If the bar has reached the end, stop it.  Otherwise keep going.
            ++MessageQueue[i].x < 0 ? ((MessageQueue[i].x = MessageQueue[i].x/1.05)) : ((MessageQueue[i].x = 0));
            MessageQueue[i].textx++;

            // Draw each "panel".
            rounded_rectangle(MessageQueue[i].x, MessageQueue[i].y, info->width, info->height, 1, info->rounding, context, 1,0.5,0,1);

            // Allow markup on the string.
            // Pixel extents are much better for this purpose.
            pango_layout_set_markup(layout, MessageQueue[i].string, -1);
            pango_layout_get_pixel_extents(layout, &extents, NULL);

            // Push the text to the soure.
            cairo_set_source_rgba(context, 0,0,0,1);
            cairo_move_to(context, MessageQueue[i].textx - extents.width, MessageQueue[i].y + info->upper);
            pango_cairo_show_layout(context, layout);

            // Draw over the text with a margin.
            cairo_set_source_rgba(context, 1,0.5,0,1);
            cairo_rectangle(context, 0, MessageQueue[i].y, info->margin, info->height);
            cairo_fill(context);

            // Kind of cool.
            //cairo_translate(context, info->width/2.0, info->height/2.0);
            //cairo_rotate(context, 1);
            //
            //cairo_scale(context, -1, 1);
        }

        // Pop the group.
        cairo_pop_group_to_source(context);

        // Paint it.
        cairo_paint(context);
        cairo_surface_flush(surface);

        // Clue in for x events (allows to check for hotkeys, stuff like that).
        switch (check_x_event(surface, 0))
        {
            case -3053:
                //fprintf(stderr, "exposed\n");
                break;
            case 0xff1b:    // esc
            case -1:        // left mouse button
                fprintf(stderr, "left mouse button\n");
                running = 0;
                break;
        }

        for (i = 0; i < rear; i++)
        {
            printf("Fuse: %Lf, Taking away: %f\n", MessageQueue[i].fuse, (double) INTERVAL/1000);
            MessageQueue[i].fuse = MessageQueue[i].fuse - (double) INTERVAL/1000;
            if (MessageQueue[i].fuse <= 0)
                queue_delete(i);
        }

        if (queue_empty())
            running = 0;

        // Finally sleep ("animation").
        nanosleep(&req, &req);
    }

    // Destroy once done.
    g_object_unref(layout);
    pango_cairo_font_map_set_default(NULL);
    cairo_destroy(context);

    var_destroy(info);

    destroy(surface);
}

int
main (int argc, char *argv[])
{
    // Option initialization.
    double timeout = 10;
    int  margin = 5, number = 1,
         upper = 0, xpos = 0,
         ypos = 0, width = 0,
         height = 0, gap = 0,
         rounding = 0;
    char *font;
    char *dimensions = "300x300";


    int  opt;
    while ((opt = getopt(argc, argv, "hf:m:n:u:g:r:t:d:")) != -1) {
        switch(opt)
        {
            case 'h': help(); break;
            case 'f': font = optarg;  break;
            case 'm': margin = strtol(optarg, NULL, 10); break;
            case 'n': number = strtol(optarg, NULL, 10); break;
            case 'u': upper = strtol(optarg, NULL, 10);  break;
            case 'g': gap = strtol(optarg, NULL, 10); break;
            case 'r': rounding = strtol(optarg, NULL, 10); break;
            case 't': timeout = strtod(optarg, NULL); break;
            case 'd': dimensions = optarg; break;
            default: help();
        }
    }

    char *strings[number];

    // Initialise to NULL and read stdin.  If the char is NULL, getline will do memory allocation automatically.
    int i;
    int status;
    unsigned long len;
    for (i = 0; i < number; i++) {
        strings[i] = NULL;
        status = getline(&strings[i], &len, stdin);

        assert(status != -1);
        strings[i][strlen(strings[i])-1] = '\0';
    }

    // Option checking.
    if (!font) printf("Font is required\n");
    //if (!dimensions) dimensions = "300x300";
    //if (margin < 0) margin = 5;
    if (rounding < 0) rounding = 0;

    // Parse the dimensions string into relative variables.
    parse(dimensions, &xpos, &ypos, &width, &height);

    // Create info on the heap.
    struct Variables *info = var_create(font, margin, number, upper, gap, rounding, timeout, xpos, ypos, width, height);

    // Run until done.
    runner(info, strings);

    // Done with strings -- program ending.
    for (i = 0; i < number; i++)
        free(strings[i]);

    return(0);
}
