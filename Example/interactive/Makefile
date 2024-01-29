# $Id$
.PHONY: clean install

./interactive$(SUFFIX): ./interactive.o $(RESFILE)
	$(CC) -g -o $@ -L ../../Library $^ -lw3

./%.o: ./%.c
	$(CC) -g -c -o $@ -I ../../Library $<

../libw3.res:
	$(MAKE) -C .. ./libw3.res WINDRES=$(WINDRES)

clean:
	rm -f interactive *.o *.so *.core *~ *.exe *.res

install: ./interactive
	mkdir -p $(PREFIX)/bin
	cp ./interactive $(PREFIX)/bin/w3-interactive
