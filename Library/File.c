/* $Id$ */
#include "W3File.h"

#include "W3Core.h"
#include "W3Util.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void __W3_File_Request(struct W3* w3) {
	__W3_Debug("LibW3-File", "Sending the request");
	FILE* f = fopen(w3->path, "r");
	if(f == NULL) {
		void* funcptr = __W3_Get_Event(w3, "status");
		if(funcptr != NULL) {
			void (*func)(struct W3*, int) = (void (*)(struct W3*, int))funcptr;
			func(w3, LIBW3_FILE_NOT_FOUND);
		}
	} else {
		void* funcptr = __W3_Get_Event(w3, "status");
		if(funcptr != NULL) {
			void (*func)(struct W3*, int) = (void (*)(struct W3*, int))funcptr;
			func(w3, LIBW3_FILE_FOUND);
		}
		char* buf = malloc(w3->readsize);
		while(true) {
			int len = fread(buf, 1, w3->readsize, f);
			void* funcptr = __W3_Get_Event(w3, "data");
			if(funcptr != NULL) {
				void (*func)(struct W3*, char*, size_t) = (void (*)(struct W3*, char*, size_t))funcptr;
				char* buffer = malloc(len);
				memcpy(buffer, buf, len);
				func(w3, buffer, len);
				free(buffer);
			}
			if(feof(f)) break;
		}
		free(buf);
	}
}
