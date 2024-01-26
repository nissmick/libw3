# $Id$
.PHONY: clean install

./w3b: ./w3b.o $(RESFILE)
	$(CC) -g -o $@ -L ../../Library $^ -lw3

./%.o: ./%.c
	$(CC) -g -c -o $@ -I ../../Library $<

../libw3.res:
	$(MAKE) -C .. ./libw3.res WINDRES=$(WINDRES)

clean:
	rm -f w3b *.o *.so *.core *~ *.exe *.res

install: ./w3b
	mkdir -p $(PREFIX)/bin
	cp ./w3b $(PREFIX)/bin/w3b
