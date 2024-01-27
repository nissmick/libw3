# $Id$
include config.mk

ifeq ($(shell uname -s),SunOS)
GREP = ggrep
else
GREP = grep
endif

CC := cc
CFLAGS := -g -std=c99 -fPIC -D_BSD_SOURCE
LDFLAGS :=
LIBS :=
PREFIX := /usr/local
VERSION = $(shell cat Library/W3Version.h | $(GREP) -m 1 LIBW3_VERSION | sed -E "s/.+\"([^\"]+)\".+/\1/g")$(shell cat Library/W3Version.h | grep -A 1 -Eo "LIBW3_VERSION" | tail -n1 | grep -Eo "W")


ifeq ($(SSL),YES)
CFLAGS += -DSSL_SUPPORT
LIBS += -lssl -lcrypto
endif

ifeq ($(WIN32),YES)
CC := i686-w64-mingw32-gcc
WINDRES := i686-w64-mingw32-windres
WINDOWS := YES
endif

ifeq ($(WIN64),YES)
CC := x86_64-w64-mingw32-gcc
WINDRES := x86_64-w64-mingw32-windres
WINDOWS := YES
endif

ifeq ($(WINDOWS),YES)
LIBS += -lws2_32
endif

ifneq ($(WINDOWS),YES)
ifeq ($(shell uname -s),SunOS)
LIBS += -lsocket
endif
endif

ifeq ($(DEBUG),YES)
CFLAGS += -g -D__DEBUG__
endif

ifeq ($(WINDOWS),YES)
.PHONY: all clean ./Library/w3.dll ./Example format

ALL := ./Library/w3.dll ./Example

all: ./w3.pc ./Library/W3Version.h $(ALL)

./Library/w3.dll:
	$(MAKE) -C ./Library CC=$(CC) CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="$(LIBS)" WINDOWS=YES

./Example: ./Library/w3.dll
	$(MAKE) -C ./Example/fetch CC=$(CC) RESFILE=../libw3.res WINDRES=$(WINDRES) WINDOWS=YES
	$(MAKE) -C ./Example/interactive CC=$(CC) RESFILE=../libw3.res WINDRES=$(WINDRES) WINDOWS=YES

./Library/W3Version.h:
	m4 -DSUFFIX=\"W\" ./W3Version.h.p > $@

else

.PHONY: all clean ./Library/libw3.so ./Library/libw3.a ./Example format

ALL := ./Library/libw3.so ./Library/libw3.a ./Example

all: ./w3.pc ./Library/W3Version.h $(ALL)

./Library/libw3.so:
	$(MAKE) -C ./Library CC=$(CC) CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="$(LIBS)" ./libw3.so

./Library/libw3.a:
	$(MAKE) -C ./Library CC=$(CC) CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="$(LIBS)" ./libw3.a

./Example: ./Library/libw3.so
	$(MAKE) -C ./Example CC=$(CC) examples

./Library/W3Version.h:
	m4 -DSUFFIX=\"\" ./W3Version.h.p > $@

endif

./w3.pc:
	echo "prefix=$(PREFIX)" > $@
	echo "exec_prefix=\$${prefix}" >> $@
	echo "includedir=\$${prefix}/include" >> $@
	echo "libdir=\$${exec_prefix}/lib" >> $@
	echo >> $@
	echo "Name: w3" >> $@
	echo "Description: The WWW Library" >> $@
	echo "Version: $(VERSION)" >> $@
	echo "Cflags: -I\$${includedir}/W3" >> $@
	echo "Libs: -I\$${libdir} -lw3" >> $@

clean:
	-rm ./w3.pc w3-*.zip w3-*.tar.gz w3-*.lzh ./Library/W3Version.h
	$(MAKE) -C ./Library clean
	$(MAKE) -C ./Example clean

install: ./w3.pc
	$(MAKE) -C ./Library install PREFIX=$(PREFIX) CC=$(CC)
	$(MAKE) -C ./Example install PREFIX=$(PREFIX) CC=$(CC)
	mkdir -p $(PREFIX)/lib/pkgconfig
	cp ./w3.pc $(PREFIX)/lib/pkgconfig/

archive: all
	mkdir -p w3-$(VERSION)/Library
	mkdir -p w3-$(VERSION)/Example/pop3-list
	mkdir -p w3-$(VERSION)/Example/interactive
	mkdir -p w3-$(VERSION)/Example/fetch
	cp -rf ./Library/*.h w3-$(VERSION)/Library/
ifdef WINDOWS
	cp ./Library/*.lib w3-$(VERSION)/Library/
	cp ./Library/*.dll w3-$(VERSION)/Library/
	cp ./Example/fetch/fetch.exe w3-$(VERSION)/Example/fetch/
	cp ./Example/interactive/interactive.exe w3-$(VERSION)/Example/interactive/
	cp ./Example/pop3-list/pop3-list.exe w3-$(VERSION)/Example/pop3-list/
else
	cp ./Library/*.so w3-$(VERSION)/Library/
	cp ./Example/fetch/fetch w3-$(VERSION)/Example/fetch/
	cp ./Example/interactive/interactive w3-$(VERSION)/Example/interactive/
	cp ./Example/pop3-list/pop3-list w3-$(VERSION)/Example/pop3-list/
endif
	-mv w3-$(VERSION)/*.h w3-$(VERSION)/Library/
	-mv w3-$(VERSION)/*.so w3-$(VERSION)/Library/
	-mv w3-$(VERSION)/*.dll w3-$(VERSION)/Library/
	-mv w3-$(VERSION)/*.lib w3-$(VERSION)/Library/
	-cp LICENSE w3-$(VERSION)/
	tar czvf w3-$(VERSION).tar.gz w3-$(VERSION)
	zip -rv w3-$(VERSION).zip w3-$(VERSION)
	-/usr/lha/bin/lha a w3-$(VERSION).lzh w3-$(VERSION)
	rm -rf w3-$(VERSION)

format:
	clang-format -i `find Library Example -name "*.h" -or -name "*.c"`
