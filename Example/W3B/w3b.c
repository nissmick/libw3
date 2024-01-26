/*
 * $Id$
 *
 * Simple browser
 */

#include <W3Core.h>

#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
	int i;
	for(i = 1; i < argc; i++){
		if(strcmp(argv[i], "--version") == 0){
			printf("LibW3 %s\n", LIBW3_VERSION);
			return 0;
		}
	}
}
