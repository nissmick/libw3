# $Id$:
.PHONY: clean install

ifeq ($(WINDOWS),YES)
./fetch.exe: ./fetch.c $(RESFILE)
	$(CC) -o $@ -I ../Library -L ../Library $^ -lw3
else
./fetch: ./fetch.c $(RESFILE)
	$(CC) -o $@ -I ../Library -L ../Library $^ -lw3
endif

./libw3.res: ./libw3.rc
	$(WINDRES) $< -O coff -o $@

clean:
	rm -f fetch *.o *.so *.core *~ *.exe *.res

install: ./fetch
	mkdir -p $(PREFIX)/bin
	cp ./fetch $(PREFIX)/bin/w3-fetch
