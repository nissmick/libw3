.PHONY: clean

./fetch: ./fetch.c
	$(CC) -o $@ $< ../Library/libw3.so

clean:
	rm -f fetch *.o *.so *.core
