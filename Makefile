# $Id$
include config.mk

CC := cc
CFLAGS := -g -std=c99 -fPIC
LDFLAGS :=
LIBS :=
PREFIX := /usr/local

ifdef SSL
CFLAGS += -DSSL_SUPPORT
LIBS += -lssl -lcrypto
endif

ifdef DEBUG
CFLAGS += -D__DEBUG__
endif

.PHONY: all clean ./Library/libw3.so ./Example/fetch

all: ./Library/libw3.so ./Example/fetch

./Library/libw3.so:
	$(MAKE) -C ./Library CC=$(CC) CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="$(LIBS)"

./Example/fetch:
	$(MAKE) -C ./Example CC=$(CC) fetch

clean:
	$(MAKE) -C ./Library clean
	$(MAKE) -C ./Example clean

install:
	$(MAKE) -C ./Library install PREFIX=$(PREFIX)
	$(MAKE) -C ./Example install PREFIX=$(PREFIX)
