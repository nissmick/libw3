# $Id$

.PHONY: clean examples install

./libw3.res: ./libw3.rc
	$(WINDRES) $< -O coff -o $@

examples:
	$(MAKE) -C ./fetch CC=$(CC)
	$(MAKE) -C ./interactive CC=$(CC)
	$(MAKE) -C ./pop3-list CC=$(CC)
	$(MAKE) -C ./w3b CC=$(CC)

install:
	$(MAKE) -C ./fetch CC=$(CC) PREFIX=$(PREFIX) install
	$(MAKE) -C ./interactive CC=$(CC) PREFIX=$(PREFIX) install
	$(MAKE) -C ./pop3-list CC=$(CC) PREFIX=$(PREFIX) install
	$(MAKE) -C ./w3b CC=$(CC) PREFIX=$(PREFIX) install

clean:
	rm -f *.o *.so *.core *~ *.exe *.res
	$(MAKE) -C ./fetch clean
	$(MAKE) -C ./interactive clean
	$(MAKE) -C ./pop3-list clean
	$(MAKE) -C ./w3b clean
