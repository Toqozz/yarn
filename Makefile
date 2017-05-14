CC=clang
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
	`pkg-config --libs pangocairo` \
	`pkg-config --libs libconfig` \
	-lm
CFLAGS=-Wall -g -pthread
all:	yarn

yarn:
	$(CC) $(BEFORE)	main.c dbus.c utils.c parse.c cfg.c x.c cairo.c draw.c queue.c yarn.c 	$(AFTER) -o yarn
clean:
	rm -f yarn
