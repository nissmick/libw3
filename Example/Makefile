# $Id$

.PHONY: clean

./libw3.res: ./libw3.rc
	$(WINDRES) $< -O coff -o $@

clean:
	rm -f *.o *.so *.core *~ *.exe *.res
