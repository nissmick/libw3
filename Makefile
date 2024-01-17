# $Id$
include config.mk

CC := cc
CFLAGS := -g -std=c99 -fPIC -D_BSD_SOURCE
LDFLAGS :=
LIBS :=
PREFIX := /usr/local

ifdef SSL
CFLAGS += -DSSL_SUPPORT
LIBS += -lssl -lcrypto
endif

ifdef WINDOWS
LIBS += -lws2_32
endif

ifdef DEBUG
CFLAGS += -g -D__DEBUG__
endif

ifdef WINDOWS
.PHONY: all clean ./Library/w3.dll ./Example/fetch

all: ./w3.pc ./Library/w3.dll ./Example/fetch

./Library/w3.dll:
	$(MAKE) -C ./Library CC=$(CC) CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="$(LIBS)" WINDOWS=YES
else
.PHONY: all clean ./Library/libw3.so ./Example/fetch

all: ./w3.pc ./Library/libw3.so ./Example/fetch

./Library/libw3.so:
	$(MAKE) -C ./Library CC=$(CC) CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="$(LIBS)"
endif

./Example/fetch:
	$(MAKE) -C ./Example CC=$(CC) fetch

./w3.pc:
	echo "prefix=$(PREFIX)" > $@
	echo "exec_prefix=\$${prefix}" >> $@
	echo "includedir=\$${prefix}/include" >> $@
	echo "libdir=\$${exec_prefix}/lib" >> $@
	echo >> $@
	echo "Name: w3" >> $@
	echo "Description: The WWW Library" >> $@
	echo "Version: $(shell cat Library/W3Core.h | grep -m 1 LIBW3_VERSION | sed -E "s/.+\"([^\"]+)\"/\1/g")" >> $@
	echo "Cflags: -I\$${includedir}/W3" >> $@
	echo "Libs: -I\$${libdir} -lw3" >> $@

clean:
	-rm ./w3.pc
	$(MAKE) -C ./Library clean
	$(MAKE) -C ./Example clean

install: ./w3.pc
	$(MAKE) -C ./Library install PREFIX=$(PREFIX)
	$(MAKE) -C ./Example install PREFIX=$(PREFIX)
	mkdir -p $(PREFIX)/lib/pkgconfig
	cp ./w3.pc $(PREFIX)/lib/pkgconfig/
