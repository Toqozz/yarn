#include <cairo.h>
#include <cairo-xlib.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "x.h"

// Apply atoms to window.
static void
x_set_wm(Window win, Display *dsp)
{
    Atom property[3];  // Change 2 things at once, (parent + 2 children).

    // Set window's WM_NAME property.
    // char *title = "yarn"; -- only used twice.
    XStoreName(dsp, win, "yarn");
    // No children.
    property[2] = XInternAtom(dsp, "_NET_WM_NAME", false); // Get WM_NAME atom and store it in _net_wm_title.
    XChangeProperty(dsp, win, property[2], XInternAtom(dsp, "UTF8_STRING", false), 8, PropModeReplace, (unsigned char *) "yarn", 4);

    // Set window's class.
    // char *class = "yarn"; -- only used once.
    XClassHint classhint = { "yarn", "yarn" };
    XSetClassHint(dsp, win, &classhint);

    // Parent.
    property[2] = XInternAtom(dsp, "_NET_WM_WINDOW_TYPE", false);   // Let WM know type.
    // Children.
    property[0] = XInternAtom(dsp, "_NET_WM_WINDOW_TYPE_NOTIFICATION", false);
    property[1] = XInternAtom(dsp, "_NET_WM_WINDOW_TYPE_UTILITY", false);
    // Reach for 2 longs, (2L).
    XChangeProperty(dsp, win, property[2], XA_ATOM, 32, PropModeReplace, (unsigned char *) property, 2L);

    // Parent.
    property[2] = XInternAtom(dsp, "_NET_WM_STATE", false);   // Let WM know state.
    // Child.
    property[0] = XInternAtom(dsp, "_NET_WM_STATE_ABOVE", false);
    // Reach for 1 long, (1L).
    XChangeProperty(dsp, win, property[2], XA_ATOM, 32, PropModeReplace, (unsigned char *) property, 1L);
}

// Map window and return surface for that window.
cairo_surface_t *
cairo_create_x11_surface(int x, int y, int w, int h)
{
    Display *display;
    Drawable drawable;
    int screen;   // Screen #.
    cairo_surface_t *surface;

    // Error if no open..
    if ((display = XOpenDisplay(NULL)) == NULL)   // Set display though.
        exit(1);

    screen = DefaultScreen(display);   // Use primary display.
    XVisualInfo vinfo;
    // Match the display settings.
    // TODO, default depth instead of guess?
    XMatchVisualInfo(display, screen, 32, TrueColor, &vinfo);

    XSetWindowAttributes attr;
    // We need all 3 of these attributes, or BadMatch: http://stackoverflow.com/questions/3645632/how-to-create-a-window-with-a-bit-depth-of-32
    attr.colormap = XCreateColormap(display, DefaultRootWindow(display), vinfo.visual, AllocNone);
    attr.border_pixel = 0;
    attr.background_pixel = 0;

    drawable = XCreateWindow(display, DefaultRootWindow(display),  // Returns a window (a drawable place).
            x,y,     // Position on screen.
            w,h,     // Width, Height.
            0,       // Border width.
            vinfo.depth, InputOutput, vinfo.visual,   // Depth, Class, Visual type.
            CWColormap | CWBorderPixel | CWBackPixel, // Overwritten attributes.
            &attr);

    // Apply the Atoms to the new window.
    // Request that the X server report these events.
    x_set_wm(drawable, display);
    XSelectInput(display, drawable, ExposureMask | ButtonPressMask | KeyPressMask);

    // Show window on screen.
    XMapWindow(display, drawable);

    // Finally create a surface from the window.
    surface = cairo_xlib_surface_create(display, drawable,
            vinfo.visual, w, h);
    //cairo_xlib_surface_set_size(surface, w, h);

    return surface;
}

// Respond properly to events.
int
check_x_event(cairo_surface_t *sfc, int *position, int block)
{
    XEvent event;

    for (;;)
    {
        // If there is nothing in the event cue, XNextEvent will block the program.
        // If we want to block, then we can.
        if (block || XPending(cairo_xlib_surface_get_display(sfc)))
            XNextEvent(cairo_xlib_surface_get_display(sfc), &event);
        else
            return 0;

        // Check it out.
        switch (event.type)
        {
            // This event shouldn't really happen that much (we should always be exposed).
            case Expose:
                return -3053; // 3xp053
            // -event.xbutton.button -- less likely to be taken?
            case ButtonPress:
                //fprintf(stderr, "the button is: %d\n", -event.xbutton.button);
                *position = event.xbutton.y;
                return -event.xbutton.button;
            case KeyPress:
                return event.xkey.keycode;
            // This event is a bit weird: https://lists.cairographics.org/archives/cairo/2014-August/025534.html
            case 65:
                break;
            // We don't know the event, drop.
            default:
                //fprintf(stderr, "Dropping unhandled XEvent.type = %d.\n", event.type);
                break;
        }
    }
}

void
x_resize_window(Display *dsp, Window win, int x, int y)
{
    XWindowChanges values;

    values.width = x;
    values.height = y;

    XConfigureWindow(dsp, win, CWWidth | CWHeight, &values);
}

void
destroy(cairo_surface_t *sfc)
{
    Display *dsp = cairo_xlib_surface_get_display(sfc);
    Drawable draw = cairo_xlib_surface_get_drawable(sfc);

    cairo_surface_destroy(sfc);
    cairo_debug_reset_static_data();
    XDestroyWindow(dsp, draw);
    XCloseDisplay(dsp);
}
