/*
 * $Id$
 *
 * Lists the FTP files
 */

#include <W3Core.h>
#include <W3FTP.h>

#include <W3URL.h>
#include <W3Util.h> /* It has some useful functions, you know */

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct W3URL* w3url;

void resp_handler(struct W3* w3, int status, char* data) {
	printf("%d\n%s\n", status, data);
	if(status == 230) {
		W3_Set_Method(w3, "LIST");
		W3_FTP_Send_Request(w3);
	} else if(status == 226) {
		W3_FTP_Disconnect(w3);
	}
}

void data_handler(struct W3* w3, char* data, size_t size) { write(1, data, size); }

int main(int argc, char** argv) {
	if(argc < 2) {
		fprintf(stderr, "%s: usage: %s url\n", argv[0], argv[0]);
		return 1;
	}
	W3_Library_Init();
	w3url = W3_Parse_URL(argv[1]);
	if(w3url != NULL) {
		struct W3* w3 = W3_Create("nntp", w3url->host, w3url->port);
		W3_On(w3, "ftpresp", resp_handler);
		W3_On(w3, "data", data_handler);
		W3_Send_Request(w3);
		W3_Free_URL(w3url);
	} else {
		return 1;
	}
}
