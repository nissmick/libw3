# $Id$:
.PHONY: clean install

./fetch$(SUFFIX): ./fetch.c $(RESFILE)
	$(CC) -g -o $@ -I ../../Library -L ../../Library $^ -lw3

../libw3.res:
	$(MAKE) -C .. ./libw3.res WINDRES=$(WINDRES)

clean:
	rm -f fetch *.o *.so *.core *~ *.exe *.res

install: ./fetch
	mkdir -p $(PREFIX)/bin
	cp ./fetch $(PREFIX)/bin/w3-fetch
