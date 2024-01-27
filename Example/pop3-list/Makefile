# $Id$
.PHONY: clean install

./pop3-list: ./pop3-list.o $(RESFILE)
	$(CC) -g -o $@ -L ../../Library $^ -lw3

./%.o: ./%.c
	$(CC) -g -c -o $@ -I ../../Library $<

../libw3.res:
	$(MAKE) -C .. ./libw3.res WINDRES=$(WINDRES)

clean:
	rm -f pop3-list *.o *.so *.core *~ *.exe *.res

install: ./pop3-list
	mkdir -p $(PREFIX)/bin
	cp ./pop3-list $(PREFIX)/bin/w3-pop3-list
