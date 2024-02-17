/*
 * $Id$
 *
 * HTTP Daemon.
 */

#include <W3Core.h>

#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
	if(argv[1] != NULL && strcmp(argv[1], "--version") == 0) {
		printf("LibW3 %s\n", LIBW3_VERSION);
		return 0;
	}
}
