CC=gcc
BEFORE=`pkg-config --cflags dbus-glib-1` \
	`pkg-config --cflags dbus-1` \
	`pkg-config --cflags glib-2.0` \
	`pkg-config --cflags gio-2.0` \
	`pkg-config --cflags cairo` \
	`pkg-config --cflags cairo-xlib` \
	`pkg-config --cflags pango` \
	`pkg-config --cflags pangocairo`
AFTER=`pkg-config --libs dbus-glib-1` \
	`pkg-config --libs dbus-1` \
	`pkg-config --libs glib-2.0` \
	`pkg-config --libs gio-2.0` \
	`pkg-config --libs cairo` \
	`pkg-config --libs cairo-xlib` \
	`pkg-config --libs pango` \
	`pkg-config --libs pangocairo`
CFLAGS=-Wall -g -lglib-2.0 -lpthread
all:	dbus

dbus:
	$(CC) $(BEFORE)	dbus.c draw.c x.c cairo.c parse.c queue.c yarn.c 	$(AFTER) -o dbus
clean:
	rm -f dbus
