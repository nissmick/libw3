# $Id$

.PHONY: clean examples install

./libw3.res: ./libw3.rc
	$(WINDRES) $< -O coff -o $@

examples:
	$(MAKE) -C ./fetch CC=$(CC) SUFFIX=$(SUFFIX)
	$(MAKE) -C ./interactive CC=$(CC) SUFFIX=$(SUFFIX)
	$(MAKE) -C ./pop3-list CC=$(CC) SUFFIX=$(SUFFIX)
	$(MAKE) -C ./w3b CC=$(CC) SUFFIX=$(SUFFIX)

install:
	$(MAKE) -C ./fetch CC=$(CC) PREFIX=$(PREFIX) SUFFIX=$(SUFFIX) install
	$(MAKE) -C ./interactive CC=$(CC) PREFIX=$(PREFIX) SUFFIX=$(SUFFIX) install
	$(MAKE) -C ./pop3-list CC=$(CC) PREFIX=$(PREFIX) SUFFIX=$(SUFFIX) install
	$(MAKE) -C ./w3b CC=$(CC) PREFIX=$(PREFIX) SUFFIX=$(SUFFIX) install

clean:
	rm -f *.o *.so *.core *~ *.exe *.res
	$(MAKE) -C ./fetch SUFFIX=$(SUFFIX) clean
	$(MAKE) -C ./interactive SUFFIX=$(SUFFIX) clean
	$(MAKE) -C ./pop3-list SUFFIX=$(SUFFIX) clean
	$(MAKE) -C ./w3b SUFFIX=$(SUFFIX) clean
