# $Id$
.PHONY: clean install

./tclw3$(SUFFIX): ./tclw3.o $(RESFILE)
	$(CC) -g -o $@ -L ../../Library $^ -lw3 $(TCL_LIBS)

./%.o: ./%.c
	$(CC) $(TCL_CFLAGS) -g -c -o $@ -I ../../Library $<

../libw3.res:
	$(MAKE) -C .. ./libw3.res WINDRES=$(WINDRES)

clean:
	rm -f tclw3 *.o *.so *.core *~ *.exe *.res

install: ./tclw3
	mkdir -p $(PREFIX)/bin
	cp ./tclw3 $(PREFIX)/bin/tclw3
