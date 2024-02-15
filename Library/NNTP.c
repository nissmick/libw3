/* $Id$ */
#include "W3NNTP.h"

#include "W3Core.h"
#include "W3DNS.h"
#include "W3Util.h"

#include <stdlib.h>
#include <string.h>

extern int strcasecmp(const char* s1, const char* s2);

void __W3_NNTP_Request(struct W3* w3) {
	__W3_Debug("LibW3-NNTP", "Sending the request");
	char* buf = malloc(w3->readsize);
	while(true) {
		int len = __W3_Auto_Read(w3, buf, w3->readsize);
		if(len <= 0) break;
	}
	free(buf);
}
