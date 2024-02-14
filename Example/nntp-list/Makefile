# $Id$
.PHONY: clean install

./nntp-list$(SUFFIX): ./nntp-list.o $(RESFILE)
	$(CC) -g -o $@ -L ../../Library $^ -lw3

./%.o: ./%.c
	$(CC) -g -c -o $@ -I ../../Library $<

../libw3.res:
	$(MAKE) -C .. ./libw3.res WINDRES=$(WINDRES)

clean:
	rm -f nntp-list *.o *.so *.core *~ *.exe *.res

install: ./nntp-list
	mkdir -p $(PREFIX)/bin
	cp ./nntp-list $(PREFIX)/bin/w3-nntp-list
