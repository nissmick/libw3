# $Id$
.PHONY: clean install

ifdef WINDOWS
./w3.dll: ./Core.o ./Util.o ./DNS.o ./HTTP.o
	$(CC) $(LDFLAGS) -shared -o $@ $^ $(LIBS)
else
./libw3.so: ./Core.o ./Util.o ./DNS.o ./HTTP.o
	$(CC) $(LDFLAGS) -shared -o $@ $^ $(LIBS)
endif

./%.o: ./%.c W3%.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.so *.core *~ *.dll

install: ./libw3.so
	mkdir -p $(PREFIX)/lib
	cp ./libw3.so $(PREFIX)/lib/
	mkdir -p $(PREFIX)/include/W3
	cp *.h $(PREFIX)/include/W3/
