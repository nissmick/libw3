# $Id$:
.PHONY: clean install

./httpd$(SUFFIX): ./httpd.c $(RESFILE)
	$(CC) -g -o $@ -I ../../Library -L ../../Library $^ -lw3

../libw3.res:
	$(MAKE) -C .. ./libw3.res WINDRES=$(WINDRES)

clean:
	rm -f httpd *.o *.so *.core *~ *.exe *.res

install: ./httpd
	mkdir -p $(PREFIX)/bin
	cp ./httpd $(PREFIX)/bin/w3-httpd
