# $Id$
.PHONY: clean install

OBJS = ./Core.o ./Util.o ./DNS.o ./HTTP.o ./File.o

ifeq ($(WINDOWS),YES)
./w3.dll: $(OBJS)
	$(CC) $(LDFLAGS) -shared -Wl,--out-implib,./w3.lib -o $@ $^ $(LIBS)
else
./libw3.so: $(OBJS)
	$(CC) $(LDFLAGS) -shared -o $@ $^ $(LIBS)
./libw3.a: $(OBJS)
	ar rcs $@ $^
endif

./%.o: ./%.c W3%.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.so *.core *~ *.dll *.lib *.a

install: ./libw3.so ./libw3.a
	mkdir -p $(PREFIX)/lib
	cp ./libw3.so $(PREFIX)/lib/
	cp ./libw3.a $(PREFIX)/lib/
	mkdir -p $(PREFIX)/include/W3
	cp *.h $(PREFIX)/include/W3/
