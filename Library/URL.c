/* $Id$ */
#include "W3URL.h"

#include "W3Util.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct W3URL* W3_Parse_URL(const char* _url) {
	char* url = __W3_Strdup(_url);
	__W3_Debug("URL", "Parsing URL");
	struct W3URL* r = malloc(sizeof(*r));
	r->protocol = NULL;
	r->host = NULL;
	r->path = NULL;
	r->port = -1;
	if(strlen(url) > 3) {
		int i;
		bool found = false;
		for(i = 0; i < strlen(url) - 3; i++) {
			if(url[i] == ':' && url[i + 1] == '/' && url[i + 2] == '/') {
				found = true;
				break;
			}
		}
		if(!found) {
			__W3_Debug("URL", "Failed to parse");
			W3_Free_URL(r);
			r = NULL;
		} else {
			url[i] = 0;
			char* str = malloc(strlen(url) + 64);
			sprintf(str, "Protocol is %s", url);
			__W3_Debug("URL", str);
			free(str);
			r->protocol = __W3_Strdup(url);
			i += 3;
			int start = i;
			int port_start = -1;
			for(; url[i] != 0; i++) {
				if(url[i] == '/') {
					r->path = __W3_Strdup(url + i);
					url[i] = 0;
					break;
				} else if(url[i] == ':') {
					port_start = i + 1;
					url[i] = 0;
				}
			}
			if(port_start != -1) {
				r->port = atoi(url + port_start);
			}
			if(r->port == -1) {
				if(strcmp(r->protocol, "http") == 0) {
					r->port = 80;
				} else if(strcmp(r->protocol, "https") == 0) {
					r->port = 443;
				} else if(strcmp(r->protocol, "gopher") == 0) {
					r->port = 70;
				} else if(strcmp(r->protocol, "gemini") == 0) {
					r->port = 1965;
				}
			}
			r->host = __W3_Strdup(url + start);
			str = malloc(strlen(r->host) + 64);
			sprintf(str, "Host is %s", r->host);
			__W3_Debug("URL", str);
			free(str);
			if(r->path == NULL) r->path = __W3_Strdup("/");
			str = malloc(strlen(r->path) + 64);
			sprintf(str, "Path is %s", r->path);
			__W3_Debug("URL", str);
			free(str);
			str = malloc(64);
			sprintf(str, "Port is %d", r->port);
			__W3_Debug("URL", str);
			free(str);
		}
	} else {
		W3_Free_URL(r);
		r = NULL;
	}
	return r;
}

void W3_Free_URL(struct W3URL* url) {
	if(url == NULL) return;
	if(url->protocol != NULL) free(url->protocol);
	if(url->host != NULL) free(url->host);
	if(url->path != NULL) free(url->path);
	free(url);
}
