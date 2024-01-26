/*
 * $Id$
 *
 * Simple browser
 */

#include <W3Core.h>
#include <W3HTTP.h>

#include <W3URL.h>
#include <W3Util.h> /* It has some useful functions, you know */

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char* databuf;
int datalen;

void status_handler(struct W3* w3, int status) { printf("Response code: %d\n", status); }
void header_handler(struct W3* w3, char* key, char* value) { printf("Header: %s: %s\n", key, value); }

void data_handler(struct W3* w3, char* data, size_t size) {
	if(databuf == NULL) {
		databuf = malloc(size);
		datalen = size;
		memcpy(databuf, data, size);
	} else {
		char* oldbuf = databuf;
		databuf = malloc(datalen + size);
		memcpy(databuf, oldbuf, datalen);
		memcpy(databuf + datalen, data, size);
		datalen += size;
		free(oldbuf);
	}
}

void access_site(const char* url) {
	struct W3URL* u = W3_Parse_URL(url);
	if(u != NULL) {
		struct W3* w3 = W3_Create(u->protocol, u->host, u->port);
		if(w3 != NULL) {
			if(databuf != NULL) {
				free(databuf);
			}
			databuf = NULL;
			datalen = 0;
			W3_Set_Method(w3, "GET");
			W3_Set_Path(w3, u->path);
			W3_Enable_Redirect(w3);
			W3_On(w3, "status", (void*)status_handler);
			W3_On(w3, "header", (void*)header_handler);
			W3_On(w3, "data", (void*)data_handler);
			W3_Send_Request(w3);
			W3_Free(w3);
			printf("%d bytes\n", datalen);
		} else {
			fprintf(stderr, "Failed to connect\n");
		}
		W3_Free_URL(u);
	} else {
		fprintf(stderr, "Failed to parse\n");
	}
}

int main(int argc, char** argv) {
	int i;
	databuf = NULL;
	datalen = 0;
	char* url = NULL;
	for(i = 1; i < argc; i++) {
		if(strcmp(argv[i], "--version") == 0) {
			printf("LibW3 %s\n", LIBW3_VERSION);
			return 0;
		} else if(argv[i][0] == '-') {
			fprintf(stderr, "%s: unknown option: %s\n", argv[0], argv[i]);
			return 1;
		} else {
			if(url != NULL) {
				free(url);
				fprintf(stderr, "%s: garbage argument found\n", argv[0]);
				return 1;
			}
			url = __W3_Strdup(argv[i]);
		}
	}
	W3_Library_Init();
	int phase = 0;
	char c = 0;
	bool acc = false;
	if(url != NULL) {
		access_site(url);
		acc = true;
	}
	while(true) { /* Loop */
		if(c != '\n' && c != '\r') {
			printf("(O)pen, (Q)uit");
			if(acc) printf(", (P)rint all");
			printf("? ");
			fflush(stdout);
		}
		if(scanf("%c", &c) < 0) break;
		switch(tolower(c)) {
		case 'q':
			goto exitnow;
		case 'o':
			printf("URL: ");
			fflush(stdout);
			if(url != NULL) free(url);
			url = malloc(2049);
			scanf("%s", url);
			acc = false;
			break;
		case 'p':
			if(acc) {
				write(1, databuf, datalen);
			}
			break;
		case '\n':
		case '\r':
			break;
		default:
			printf("What do you mean?\n");
			break;
		}
		if(!acc && url != NULL) {
			access_site(url);
			acc = true;
		}
	}
	printf("\n");
exitnow:;
}
