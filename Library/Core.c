/* $Id$ */
#include "W3Core.h"

#include "W3DNS.h"
#include "W3Util.h"

#include <stdio.h>
#include <stdbool.h>

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
#ifdef SSL_SUPPORT
	__W3_Debug("LibW3", "This is LibW3, version " LIBW3_VERSION ", using " OPENSSL_VERSION_TEXT);
#else
	__W3_Debug("LibW3", "This is LibW3, version " LIBW3_VERSION ", without SSL support");
#endif
	return 0;
}

struct W3* W3_Create(bool ssl, const char* hostname, int port){
	__W3_Debug("Create", "Creating a struct");
	struct W3* w3 = malloc(sizeof(*w3));
	w3->sock = __W3_DNS_Connect(hostname, ssl, port
#ifdef SSL_SUPPORT
	,
	&w3->ssl,
	&w3->ssl_ctx
#endif
	);
	return w3;
}
