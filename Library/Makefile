# $Id$
.PHONY: clean install

OBJS = ./Core.o ./Util.o ./DNS.o ./NNTP.o ./URL.o ./Tag.o

include ./protocol.mk

ifeq ($(TCL),YES)
OBJS += ./Tcl.o
endif
ifeq ($(SSL),YES)
ifneq ($(GEMINI),NO)
OBJS += ./Gemini.o
endif
endif

ifneq ($(HTTP),NO)
OBJS += ./HTTP.o
endif

ifneq ($(GOPHER),NO)
OBJS += ./Gopher.o
endif

ifneq ($(POP3),NO)
OBJS += ./POP3.o
endif

ifneq ($(FINGER),NO)
OBJS += ./Finger.o
endif

ifneq ($(FILE),NO)
OBJS += ./File.o
endif

ifneq ($(NEX),NO)
OBJS += ./Nex.o
endif

ifneq ($(FTP),NO)
OBJS += ./FTP.o
endif

ifeq ($(WINDOWS),YES)
./w3.dll: $(OBJS)
	$(CC) $(LDFLAGS) -fstack-protector -L../openssl/lib/mingw/$(WINARCH) -L . -shared -Wl,--out-implib,./w3.lib -o $@ $^ $(LIBS)
else

./libw3.so: $(OBJS)
	$(CC) $(LDFLAGS) -shared -o $@ $^ $(LIBS)
./libw3.a: $(OBJS)
	ar rcs $@ $^
endif

./%.o: ./%.c W3%.h
	$(CC) $(CFLAGS) -I../openssl/include -c -o $@ $<

clean:
	rm -f *.o *.so *.core *~ *.dll *.lib *.a *.res

install: ./libw3.so ./libw3.a
	mkdir -p $(PREFIX)/lib
	cp ./libw3.so $(PREFIX)/lib/
	cp ./libw3.a $(PREFIX)/lib/
	mkdir -p $(PREFIX)/include/W3
	cp *.h $(PREFIX)/include/W3/
