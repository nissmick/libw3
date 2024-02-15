# $Id$
PKGCONF := pkg-config

PREFIX := /usr/local
SSL := YES
TCL := NO
TCL_PKGCONF := YES
DEBUG := NO

ifeq ("$(wildcard cache.mk)","")
$(info Creating cache.mk)
IGNORE := $(shell echo "PREFIX=$(PREFIX)" > cache.mk)
IGNORE := $(shell echo "TCL_PKGCONF=$(TCL_PKGCONF)" >> cache.mk)
IGNORE := $(shell echo "TCL=$(TCL)" >> cache.mk)
IGNORE := $(shell echo "SSL=$(SSL)" >> cache.mk)
IGNORE := $(shell echo "DEBUG=$(DEBUG)" >> cache.mk)
include cache.mk
else
include cache.mk
endif

include ./Library/protocol.mk

ifeq ($(shell uname -s),SunOS)
GREP = ggrep
else
GREP = grep
endif

ifeq ($(TCL),YES)
ifneq ($(TCL_PKGCONF),NO)
TCL_CFLAGS += $(shell $(PKGCONF) --cflags tcl) -DTCL_BINDING
TCL_LIBS += $(shell $(PKGCONF) --libs tcl)
else
TCL_LIBS += -ltcl
endif
endif

CC := cc
CFLAGS := -g -std=c99 -fPIC -D_XOPEN_SOURCE=600 $(TCL_CFLAGS)
LDFLAGS :=
LIBS := $(TCL_LIBS)
VERSION = $(shell cat Library/W3Version.h | $(GREP) -m 1 LIBW3_VERSION | sed -E "s/.+\"([^\"]+)\".+/\1/g")$(shell cat Library/W3Version.h | grep -A 1 -E "LIBW3_VERSION" | sed "s/LIBW3_VERSION//g" | tail -n1 | grep -Eo "W")

ifeq ($(SSL),YES)
CFLAGS += -DSSL_SUPPORT
LIBS += -lssl -lcrypto
endif

ifeq ($(SSL),YES)
ifneq ($(GEMINI),NO)
FLAGS += -DGEMINI_SUPPORT
endif
endif

ifneq ($(HTTP),NO)
FLAGS += -DHTTP_SUPPORT
endif

ifneq ($(GOPHER),NO)
FLAGS += -DGOPHER_SUPPORT
endif

ifneq ($(POP3),NO)
FLAGS += -DPOP3_SUPPORT
endif

ifneq ($(FINGER),NO)
FLAGS += -DFINGER_SUPPORT
endif

ifneq ($(FILE),NO)
FLAGS += -DFILE_SUPPORT
endif

ifneq ($(NEX),NO)
FLAGS += -DNEX_SUPPORT
endif

ifneq ($(FTP),NO)
FLAGS += -DFTP_SUPPORT
endif

CFLAGS += $(FLAGS)

ifeq ($(WIN32),YES)
CC := i686-w64-mingw32-gcc
WINDRES := i686-w64-mingw32-windres
WINDOWS := YES
WINARCH := x86
MINGW := i686-w64-mingw32
endif

ifeq ($(WIN64),YES)
CC := x86_64-w64-mingw32-gcc
WINDRES := x86_64-w64-mingw32-windres
WINDOWS := YES
WINARCH := x64
MINGW := x86_64-w64-mingw32
endif

ifeq ($(WINDOWS),YES)
LIBS += -lws2_32
endif

ifneq ($(WINDOWS),YES)
ifeq ($(shell uname -s),SunOS)
LIBS += -lsocket
endif
ifeq ($(shell uname -s),Haiku)
LIBS += -lnetwork
endif
endif

ifeq ($(DEBUG),YES)
CFLAGS += -g
endif

ifeq ($(WINDOWS),YES)
.PHONY: all clean ./Library/w3.dll ./Example format src-archive archive

ALL := ./Library/w3.dll ./Example

all: ./Library/W3Version.h ./w3.pc $(ALL)

./Library/w3.dll:
	$(MAKE) -C ./Library CC=$(CC) CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="$(LIBS)" WINDOWS=YES WINARCH=$(WINARCH) TCL=$(TCL) SSL=$(SSL)

./Example: ./Library/w3.dll
	$(MAKE) -C ./Example CC=$(CC) TCL=$(TCL) TCL_LIBS="$(TCL_LIBS)" TCL_CFLAGS="$(TCL_CFLAGS)" examples SUFFIX=.exe

./Library/W3Version.h: ./W3Version.h.m4
	m4 -DSUFFIX=\"W\" $(FLAGS) $< > $@

else

.PHONY: all clean ./Library/libw3.so ./Library/libw3.a ./Example format src-archive archive

ALL := ./Library/libw3.so ./Library/libw3.a ./Example

all: ./Library/W3Version.h ./w3.pc $(ALL)

./Library/libw3.so:
	$(MAKE) -C ./Library CC=$(CC) CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="$(LIBS)" TCL=$(TCL) SSL=$(SSL) ./libw3.so

./Library/libw3.a:
	$(MAKE) -C ./Library CC=$(CC) CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="$(LIBS)" TCL=$(TCL) SSL=$(SSL) ./libw3.a

./Example: ./Library/libw3.so
	$(MAKE) -C ./Example CC=$(CC) TCL_LIBS="$(TCL_LIBS)" TCL_CFLAGS="$(TCL_CFLAGS)" TCL=$(TCL) examples

./Library/W3Version.h: ./W3Version.h.m4
	m4 -DSUFFIX=\"\" $(FLAGS) $< > $@

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
	echo "Libs: -L\$${libdir} -lw3" >> $@

clean:
	-rm -f ./w3.pc w3-*.zip w3-*.tar.gz w3-*.lzh ./Library/W3Version.h *~ cache.mk
	$(MAKE) -C ./Library clean
	$(MAKE) -C ./Example clean

install: ./w3.pc
	$(MAKE) -C ./Library install PREFIX=$(PREFIX) CC=$(CC)
	$(MAKE) -C ./Example install PREFIX=$(PREFIX) CC=$(CC) TCL=$(TCL)
	mkdir -p $(PREFIX)/lib/pkgconfig
	cp ./w3.pc $(PREFIX)/lib/pkgconfig/

archive: all
	mkdir -p w3-$(VERSION)/Library
	mkdir -p w3-$(VERSION)/Example/w3b
ifeq ($(TCL),YES)
	mkdir -p w3-$(VERSION)/Example/tclw3/bin
	mkdir -p w3-$(VERSION)/Example/tclw3/lib
endif
	mkdir -p w3-$(VERSION)/Example/nntp-list
	mkdir -p w3-$(VERSION)/Example/ftp-list
	mkdir -p w3-$(VERSION)/Example/pop3-list
	mkdir -p w3-$(VERSION)/Example/interactive
	mkdir -p w3-$(VERSION)/Example/fetch
	cp -rf ./Library/*.h w3-$(VERSION)/Library/
ifeq ($(WINDOWS),YES)
	cp ./Library/*.lib w3-$(VERSION)/Library/
	cp ./Library/*.dll w3-$(VERSION)/Library/
	cp ./Example/fetch/fetch.exe w3-$(VERSION)/Example/fetch/
	cp ./Example/interactive/interactive.exe w3-$(VERSION)/Example/interactive/
	cp ./Example/pop3-list/pop3-list.exe w3-$(VERSION)/Example/pop3-list/
	cp ./Example/w3b/w3b.exe w3-$(VERSION)/Example/w3b/
	cp ./Example/ftp-list/ftp-list.exe w3-$(VERSION)/Example/ftp-list/
	cp ./Example/nntp-list/nntp-list.exe w3-$(VERSION)/Example/nntp-list/
ifeq ($(TCL),YES)
	cp ./Example/tclw3/tclw3.exe w3-$(VERSION)/Example/tclw3/bin/
	cp /usr/$(MINGW)/sys-root/mingw/bin/tcl86.dll w3-$(VERSION)/Example/tclw3/bin/
	cp /usr/$(MINGW)/sys-root/mingw/bin/tk86.dll w3-$(VERSION)/Example/tclw3/bin/
	cp /usr/$(MINGW)/sys-root/mingw/bin/libssp-0.dll w3-$(VERSION)/Example/tclw3/bin/
	cp /usr/$(MINGW)/sys-root/mingw/bin/zlib1.dll w3-$(VERSION)/Example/tclw3/bin/
	cp -rf /usr/$(MINGW)/sys-root/mingw/share/tcl8.6 w3-$(VERSION)/Example/tclw3/lib/tcl8.6
endif
else
	cp ./Library/*.so w3-$(VERSION)/Library/
	cp ./Example/fetch/fetch w3-$(VERSION)/Example/fetch/
	cp ./Example/interactive/interactive w3-$(VERSION)/Example/interactive/
	cp ./Example/pop3-list/pop3-list w3-$(VERSION)/Example/pop3-list/
	cp ./Example/w3b/w3b w3-$(VERSION)/Example/w3b/
	cp ./Example/ftp-list/ftp-list w3-$(VERSION)/Example/ftp-list/
	cp ./Example/nntp-list/nntp-list w3-$(VERSION)/Example/nntp-list/
ifeq ($(TCL),YES)
	cp ./Example/tclw3/tclw3 w3-$(VERSION)/Example/tclw3/bin/
endif
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

src-archive: clean ./Library/W3Version.h
	rm -rf /tmp/w3-$(VERSION)
	mkdir /tmp/w3-$(VERSION)
	cp -rf * /tmp/w3-$(VERSION)/
	rm -rf `find /tmp/w3-$(VERSION) -name ".svn"`
	rm -rf `find /tmp/w3-$(VERSION) -name "openssl"`
	cd /tmp && tar czvf /tmp/w3-$(VERSION).tar.gz w3-$(VERSION)
	mv /tmp/w3-$(VERSION).tar.gz ./
	cd /tmp && zip -rv /tmp/w3-$(VERSION).zip w3-$(VERSION)
	mv /tmp/w3-$(VERSION).zip ./

format:
	clang-format -i `find Library Example -name "*.h" -or -name "*.c"`
