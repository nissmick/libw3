# $Id$

.PHONY: clean examples install

./libw3.res: ./libw3.rc
	$(WINDRES) $< -O coff -o $@

examples:
	$(MAKE) -C ./fetch CC=$(CC) SUFFIX=$(SUFFIX)
	$(MAKE) -C ./interactive CC=$(CC) SUFFIX=$(SUFFIX)
	$(MAKE) -C ./pop3-list CC=$(CC) SUFFIX=$(SUFFIX)
	$(MAKE) -C ./w3b CC=$(CC) SUFFIX=$(SUFFIX)
	$(MAKE) -C ./ftp-list CC=$(CC) SUFFIX=$(SUFFIX)
	$(MAKE) -C ./nntp-list CC=$(CC) SUFFIX=$(SUFFIX)
ifeq ($(TCL),YES)
	$(MAKE) -C ./tclw3 CC=$(CC) TCL_LIBS="$(TCL_LIBS)" TCL_CFLAGS="$(TCL_CFLAGS)" SUFFIX=$(SUFFIX)
endif

install:
	$(MAKE) -C ./fetch CC=$(CC) PREFIX=$(PREFIX) SUFFIX=$(SUFFIX) install
	$(MAKE) -C ./interactive CC=$(CC) PREFIX=$(PREFIX) SUFFIX=$(SUFFIX) install
	$(MAKE) -C ./pop3-list CC=$(CC) PREFIX=$(PREFIX) SUFFIX=$(SUFFIX) install
	$(MAKE) -C ./w3b CC=$(CC) PREFIX=$(PREFIX) SUFFIX=$(SUFFIX) install
	$(MAKE) -C ./ftp-list CC=$(CC) PREFIX=$(PREFIX) SUFFIX=$(SUFFIX) install
	$(MAKE) -C ./nntp-list CC=$(CC) PREFIX=$(PREFIX) SUFFIX=$(SUFFIX) install
ifeq ($(TCL),YES)
	$(MAKE) -C ./tclw3 CC=$(CC) PREFIX=$(PREFIX) SUFFIX=$(SUFFIX) install
endif

clean:
	rm -f *.o *.so *.core *~ *.exe *.res
	$(MAKE) -C ./fetch SUFFIX=$(SUFFIX) clean
	$(MAKE) -C ./interactive SUFFIX=$(SUFFIX) clean
	$(MAKE) -C ./pop3-list SUFFIX=$(SUFFIX) clean
	$(MAKE) -C ./w3b SUFFIX=$(SUFFIX) clean
	$(MAKE) -C ./ftp-list SUFFIX=$(SUFFIX) clean
	$(MAKE) -C ./nntp-list SUFFIX=$(SUFFIX) clean
ifeq ($(TCL),YES)
	$(MAKE) -C ./tclw3 CC=$(CC) SUFFIX=$(SUFFIX) clean
endif
