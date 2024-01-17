/* $Id$ */
#include "W3Core.h"

#include "W3DNS.h"
#include "W3Util.h"
#include "W3HTTP.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>


#ifdef __MINGW32__
#include <windows.h>
#include <winsock.h>
#endif

#ifdef SSL_SUPPORT
#include <openssl/ssl.h>
#endif

int W3_Library_Init(void){
	__W3_Debug("LibW3", "Initializing");
#ifdef SSL_SUPPORT
	if(SSL_library_init() >= 0){
		__W3_Debug("LibW3-SSL", "Initialized");
	}else{
		return 1;
	}
#endif
#ifdef __MINGW32__
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
#ifdef SSL_SUPPORT
	__W3_Debug("LibW3", "This is LibW3, version " LIBW3_VERSION ", using " OPENSSL_VERSION_TEXT);
#else
	__W3_Debug("LibW3", "This is LibW3, version " LIBW3_VERSION ", without SSL support");
#endif
	return 0;
}

struct W3* W3_Create(const char* protocol, const char* hostname, int port){
	__W3_Debug("Create", "Creating a struct");
	struct W3* w3 = malloc(sizeof(*w3));
	bool ssl = false;
	if(
		strcmp(protocol, "https") == 0
	){
		ssl = true;
	}
	w3->method = NULL;
	w3->path = NULL;
	w3->protocol = __W3_Strdup(protocol);
	if(ssl) __W3_Debug("Protocol", "Enabled SSL");
	w3->sock = __W3_DNS_Connect(hostname, ssl, port
#ifdef SSL_SUPPORT
	,
	&w3->ssl,
	&w3->ssl_ctx
#endif
	);
	return w3;
}

void W3_Set_Method(struct W3* w3, const char* method){
	if(w3->method != NULL) free(w3->method);
	w3->method = __W3_Strdup(method);
}

void W3_Set_Path(struct W3* w3, const char* path){
	if(w3->path != NULL) free(w3->path);
	w3->path = __W3_Strdup(path);
}

void W3_Send_Request(struct W3* w3){
	if(strcmp(w3->protocol, "http") == 0 || strcmp(w3->protocol, "https") == 0){
		__W3_HTTP_Request(w3);
	}
}

void W3_Free(struct W3* w3){
	__W3_Debug("LibW3", "Freeing");
	if(w3->method != NULL) free(w3->method);
	if(w3->path != NULL) free(w3->path);
	if(w3->protocol != NULL) free(w3->protocol);
	free(w3);
}
