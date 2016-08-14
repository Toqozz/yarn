CC=gcc
BEFORE=-Wall -g `pkg-config --cflags dbus-glib-1` \
	`pkg-config --cflags dbus-1` \
	`pkg-config --cflags glib-2.0` \
	`pkg-config --cflags gio-2.0`
AFTER=`pkg-config --libs dbus-glib-1` \
    `pkg-config --libs dbus-1` \
    `pkg-config --libs glib-2.0` \
	`pkg-config --libs gio-2.0`
CFLAGS=-Wall -g -I/usr/include/dbus-1.0 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -lglib-2.0 -lpthread
all:	dbus

dbus:
	$(CC) $(BEFORE)	yarn.c dbus.c draw.c x.c cairo.c 	$(AFTER) -o dbus
clean:
	rm -f dbus
