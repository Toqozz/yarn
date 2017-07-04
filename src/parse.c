#include <cairo.h>
#include <cairo-xlib.h>

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "parse.h"
#include "utils.h"
#include "datatypes.h"

/* Parse a geometry string into some numbers */
void
parse_geometry(const char *wxh, int *xpos, int *ypos, int *width, int *height)
{
    char *x, *y, *w, *h;

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

    //printf("%s, %s, %s, %s\n", w, h, x, y);
    // Change variables 'globally' in memory. (*width and *height have memory addresses from main.).
    *xpos = strtol(x, NULL, 10);   // change value xpos is pointing to to something else.
    *ypos = strtol(y, NULL, 10);
    *width = strtol(w, NULL, 10);
    *height = strtol(h, NULL, 10);

    // Free pointers after everything is done with it.
    free(point);        // Points to dupe's initial pointer.
    free(dupe);
}

/* Find out which notification was pressed given mouse y coordinate and windw height. */
int
parse_xy_to_notification(int ypos, int height, int max_notifications)
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

/* Parse a hex string into an RGBA color struct (0-1 based) */
Color
parse_hex_to_rgba(const char *hex_color)
{
    double a, r, g, b;

    // First turn string into hex.
    int hex;
    sscanf(hex_color, "#%x", &hex);

    // Move bits around and take out specific parts.
    a = ((hex >> 24) & 0xff) / 255.0;
    r = ((hex >> 16) & 0xff) / 255.0;
    g = ((hex >> 8) & 0xff) / 255.0;
    b = (hex & 0xff) / 255.0;

    //printf("a: %x-%f\nr: %x-%f\ng: %x-%f\nb: %x-%f\n", a, a, r, r, g, g, b, b);

    Color rgba = {r, g, b, a};

    return rgba;
}

/* Replace markup with real characters. */
char *
parse_strip_markup(const char *text)
{
    char *temp;

    // TODO, we should later replace &lt; b &rt; with <b> or similar...?
    // regex? or only allow certain things... or lose performance hmm..
    //printf("strip markup from: %s\n", text);

    temp = repl_str(text, "&amp;", "&");
    temp = repl_str(temp, "&apos;", "'");
    temp = repl_str(temp, "&quot;", "\"");
    temp = repl_str(temp, "&lt;", "<");
    temp = repl_str(temp, "&gt;", ">");

    //printf("new string: %s\n", temp);

    return temp;
}

/* Replace real cahracters with markup. */
char *
parse_quote_markup(const char *text)
{
    char *temp;

    //printf("quote markup from: %s\n", text);

    temp = repl_str(text, "&", "&amp;");
    temp = repl_str(temp, "'", "&apos;");
    temp = repl_str(temp, "\"", "&quot;");

    // Just these for now... can add more!
    // We don't need this stuff, its a user convenience thing.
    // Also, maybe hack the repl_str code to occlude these.
    temp = repl_str(temp, "<b>", "&bld;");
    temp = repl_str(temp, "</b>", "&bldc;");
    temp = repl_str(temp, "<i>", "&it;");
    temp = repl_str(temp, "</i>", "&itc;");
    temp = repl_str(temp, "<s>", "&st;");
    temp = repl_str(temp, "</s>", "&stc;");
    temp = repl_str(temp, "<s>", "&st;");
    temp = repl_str(temp, "</s>", "&stc;");
    temp = repl_str(temp, "<u>", "&und;");
    temp = repl_str(temp, "</u>", "&undc;");

    // These are annoying.
    temp = repl_str(temp, "<", "&lt;");
    temp = repl_str(temp, ">", "&gt;");

    temp = repl_str(temp, "&bld;", "<b>");
    temp = repl_str(temp, "&bldc;", "</b>");
    temp = repl_str(temp, "&it;", "<i>");
    temp = repl_str(temp, "&itc;", "</i>");
    temp = repl_str(temp, "&st;", "<s>");
    temp = repl_str(temp, "&stc;", "</s>");
    temp = repl_str(temp, "&st;", "<s>");
    temp = repl_str(temp, "&stc;", "</s>");
    temp = repl_str(temp, "&und;", "<u>");
    temp = repl_str(temp, "&undc;", "</u>");

    return temp;
}

/* Figure whether a message needs to be moved */
int
parse_offset_value(int value)
{
    if (value < 0)
        return -value;
    else
        return 0;
}


