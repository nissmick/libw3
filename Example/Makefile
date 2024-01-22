# $Id$:
.PHONY: clean install

./fetch: ./fetch.c $(RESFILE)
	$(CC) -o $@ -I ../Library -L ../Library $^ -lw3

./libw3.res: ./libw3.rc
	$(WINDRES) $< -O coff -o $@

clean:
	rm -f fetch *.o *.so *.core *~ *.exe

install: ./fetch
	mkdir -p $(PREFIX)/bin
	cp ./fetch $(PREFIX)/bin/w3-fetch
