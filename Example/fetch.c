/*
 * $Id$
 *
 * Fetch a file using libw3.
 */

#include <W3Core.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>

void fetch_data(struct W3* w3, char* data, size_t size){
	write(1, data, size);
}

void status(struct W3* w3, int status){
	fprintf(stderr, "Response code is %d\n", status);
}

void header(struct W3* w3, char* key, char* value){
	fprintf(stderr, "Header: %s is `%s'\n", key, value);
	if(strcasecmp(key, "Server") == 0){
		/* For the example - We get the server software */
		fprintf(stderr, "Server is using `%s'\n", value);
	}
}

int main(int argc, char** argv){
	if(argv[1] != NULL && strcmp(argv[1], "--version") == 0){
		printf("LibW3 %s\n", LIBW3_VERSION);
		return 0;
	}
	if(argc < 3){
		fprintf(stderr, "Usage: %s URL Path [method [data]]\n", argv[0]);
		return 1;
	}
	W3_Library_Init();
	struct W3* w3 = W3_Create("http", argv[1], 80);
	if(w3 != NULL){
		W3_Set_Read_Size(w3, 1024);
		W3_Set_Method(w3, argv[3] == NULL ? "GET" : argv[3]);
		W3_Set_Path(w3, argv[2]);
		W3_On(w3, "status", (void*)status);
		W3_On(w3, "data", (void*)fetch_data);
		W3_On(w3, "header", (void*)header);
		if(argv[3] != NULL && strcmp(argv[3], "POST") == 0 && argv[4] != NULL){
			W3_Set_Data(w3, argv[4], strlen(argv[4]));
		}
		W3_Send_Request(w3);
		W3_Free(w3);
	}else{
		fprintf(stderr, "Failed to fetch\n");
		return 1;
	}
	return 0;
}
