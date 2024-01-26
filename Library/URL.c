/* $Id$ */
#include "W3URL.h"

#include "W3Util.h"

#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

struct W3URL* W3_Parse_URL(const char* _url){
	char* url = __W3_Strdup(_url);
	__W3_Debug("URL", "Parsing URL");
	struct W3URL* r = malloc(sizeof(*r));
	r->protocol = NULL;
	r->host = NULL;
	r->path = NULL;
	if(strlen(url) > 3){
		int i;
		bool found = false;
		for(i = 0; i < strlen(url) - 3; i++){
			if(url[i] == ':' && url[i + 1] == '/' && url[i + 2] == '/'){
				found = true;
				break;
			}
		}
		if(!found){
			__W3_Debug("URL", "Failed to parse");
			W3_Free_URL(r);
			r = NULL;
		}else{
			url[i] = 0;
			char* str = malloc(strlen(url) + 64);
			sprintf(str, "Protocol is %s", url);
			__W3_Debug("URL", str);
			free(str);
			r->protocol = __W3_Strdup(url);
			i += 3;
			int start = i;
			for(; url[i] != 0; i++){
				if(url[i] == '/'){
					r->path = __W3_Strdup(url + i);
					url[i] = 0;
					break;
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
		}
	}
	return r;
}

void W3_Free_URL(struct W3URL* url){
	if(url == NULL) return;
	if(url->protocol != NULL) free(url->protocol);
	if(url->host != NULL) free(url->host);
	if(url->path != NULL) free(url->path);
	free(url);
}
