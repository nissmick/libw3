# $Id$:
.PHONY: clean install

./fetch: ./fetch.c
	$(CC) -o $@ -I ../Library -L ../Library $< -lw3

clean:
	rm -f fetch *.o *.so *.core

install: ./fetch
	mkdir -p $(PREFIX)/bin
	cp ./fetch $(PREFIX)/bin/w3-fetch
