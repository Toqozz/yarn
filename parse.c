#include <cairo.h>
#include <cairo-xlib.h>

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "parse.h"
#include "datatypes.h"

void
parse(const char *wxh, int *xpos, int *ypos, int *width, int *height)
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

// Find out which notification was pressed given mouse y coordinate and windw height.
int
get_notification (int ypos, int height, int max_notifications)
{
    int notification = 0;

    for (int i = 1; i <= max_notifications; i++) {
        if (ypos < i*height) {
            //printf("%d is less than %d, it is in %d\n", ypos, i*height, i);
            notification = i;
            break;
        }
    }

    return notification-1;
}

Color
hex_to_rgb(const char *hex_color)
{
}

