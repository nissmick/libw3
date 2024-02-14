# $Id$
.PHONY: clean install

./ftp-list$(SUFFIX): ./ftp-list.o $(RESFILE)
	$(CC) -g -o $@ -L ../../Library $^ -lw3

./%.o: ./%.c
	$(CC) -g -c -o $@ -I ../../Library $<

../libw3.res:
	$(MAKE) -C .. ./libw3.res WINDRES=$(WINDRES)

clean:
	rm -f ftp-list *.o *.so *.core *~ *.exe *.res

install: ./ftp-list
	mkdir -p $(PREFIX)/bin
	cp ./ftp-list $(PREFIX)/bin/w3-ftp-list
