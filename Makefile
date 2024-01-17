# $Id$
include config.mk

CC := cc
CFLAGS := -g -std=c99 -fPIC
LDFLAGS :=
LIBS :=

ifdef SSL
LIBS += -lssl -lcrypto
endif

.PHONY: all

all: ./Library/libw3.so

./Library/libw3.so:
	$(MAKE) -C ./Library
