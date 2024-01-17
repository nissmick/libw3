# $Id$
.PHONY: clean

./libw3.so: ./Core.o
	$(CC) $(LDFLAGS) -shared -o $@ $^

./%.o: ./%.c W3%.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.so *.core
