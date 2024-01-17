/* $Id$ */
#include "W3HTTP.h"

#include "W3Core.h"
#include "W3Util.h"

#include <string.h>

void __W3_HTTP_Request(struct W3* w3){
	__W3_Debug("LibW3-HTTP", "Sending the request");
	__W3_Auto_Write(w3, w3->method, strlen(w3->method));
	__W3_Auto_Write(w3, " ", 1);
	__W3_Auto_Write(w3, w3->path, strlen(w3->path));
	__W3_Auto_Write(w3, " ", 1);
	__W3_Auto_Write(w3, "HTTP/1.1", 8);
	__W3_Auto_Write(w3, "\r\n", 2);
	__W3_Auto_Write(w3, "Host: ", 6);
	__W3_Auto_Write(w3, w3->hostname, strlen(w3->hostname));
	__W3_Auto_Write(w3, "\r\n", 2);
	__W3_Auto_Write(w3, "\r\n", 2);
}
