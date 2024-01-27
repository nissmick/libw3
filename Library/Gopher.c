/* $Id$ */
#include "W3Gopher.h"

#include "W3Core.h"
#include "W3Util.h"

#include <string.h>
#include <stdlib.h>

void __W3_Gopher_Request(struct W3* w3) {
	__W3_Debug("LibW3-Gopher", "Sending the request");
	__W3_Auto_Write(w3, w3->path, strlen(w3->path));
	__W3_Auto_Write(w3, "\r\n", 2);
	char* buf = malloc(w3->readsize);
	while(true){
		int len = __W3_Auto_Read(w3, buf, w3->readsize);
		if(len <= 0) break;
		void* funcptr = __W3_Get_Event(w3, "data");
		if(funcptr != NULL) {
			void (*func)(struct W3*, char*, size_t) = (void (*)(struct W3*, char*, size_t))funcptr;
			func(w3, buf, len);
		}
	}	
	free(buf);
}
