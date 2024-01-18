/*
 * $Id$
 *
 * Fetch a file using libw3.
 */

#include <W3Core.h>

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

int main(int argc, char** argv){
	if(argv[1] != NULL && strcmp(argv[1], "--version") == 0){
		printf("LibW3 %s\n", LIBW3_VERSION);
		return 0;
	}
	if(argc < 3){
		fprintf(stderr, "Usage: %s URL Path\n", argv[0]);
		return 1;
	}
	W3_Library_Init();
	struct W3* w3 = W3_Create("http", argv[1], 80);
	if(w3 != NULL){
		W3_Set_Method(w3, "GET");
		W3_Set_Path(w3, argv[2]);
		W3_Send_Request(w3);
		W3_Free(w3);
	}else{
		fprintf(stderr, "Failed to fetch\n");
		return 1;
	}
	return 0;
}
