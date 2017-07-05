# https://github.com/dunst-project/dunst/blob/master/config.mk
PREFIX ?= /usr/local

PKG_CONFIG:=$(shell which pkg-config)
ifeq (${PKG_CONFIG}, ${EMPTY})
	$(error "Failed to find pkg-config, please make sure it is installed")
endif

CFLAGS += -g -Wall

pkg_config_packs := dbus-1 glib-2.0 gio-2.0 cairo cairo-xlib pango pangocairo libconfig

INCS := $(shell ${PKG_CONFIG} --cflags ${pkg_config_packs})
CFLAGS += ${INCS}
LDFLAGS += -lm $(shell ${PKG_CONFIG} --libs ${pkg_config_packs})

ifeq (${INCS}, ${EMPTY})
	$(error "pkg-config failed, see errors above")
endif
