# $Id$
include config.mk

CC := cc
CFLAGS := -g -std=c99 -fPIC
LDFLAGS :=
LIBS :=

ifdef SSL
LIBS += -lssl -lcrypto
SSL=SSL=YES
endif

.PHONY: all clean

all: ./Library/libw3.so ./Example/fetch

./Library/libw3.so:
	$(MAKE) -C ./Library $(SSL) CC=$(CC) CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="$(LIBS)"

./Example/fetch:
	$(MAKE) -C ./Example CC=$(CC) fetch

clean:
	$(MAKE) -C ./Library clean
	$(MAKE) -C ./Example clean
