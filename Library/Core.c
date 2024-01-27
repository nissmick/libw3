/* $Id$ */
#include "W3Core.h"

#include "W3DNS.h"
#include "W3Util.h"

#include "W3File.h"
#include "W3Gopher.h"
#include "W3HTTP.h"
#include "W3POP3.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef __MINGW32__
#include <windows.h>
#include <winsock.h>
#endif

#ifdef SSL_SUPPORT
#include <openssl/ssl.h>
#endif

int W3_Library_Init(void) {
	__W3_Debug("LibW3", "Initializing");
#ifdef SSL_SUPPORT
	if(SSL_library_init() >= 0) {
		__W3_Debug("LibW3-SSL", "Initialized");
	} else {
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

struct W3* W3_Create(const char* protocol, const char* hostname, int port) {
	__W3_Debug("Create", "Creating a struct");
	struct W3* w3 = malloc(sizeof(*w3));
	bool ssl = false;
	if(strcmp(protocol, "https") == 0) {
		ssl = true;
	}
	w3->props = NULL;
	w3->method = NULL;
	w3->path = NULL;
	w3->events = NULL;
	w3->headers = NULL;
	w3->size = 0;
	w3->data = NULL;
	w3->readsize = 512;
	w3->port = port;
	w3->protocol = __W3_Strdup(protocol);
#ifdef SSL_SUPPORT
	w3->ssl = NULL;
	w3->ssl_ctx = NULL;
#endif
	if(strcmp(protocol, "file") != 0) {
		if(strcmp(protocol, "http") == 0) {
#ifdef SSL_SUPPORT
		} else if(strcmp(protocol, "https") == 0) {
		} else if(strcmp(protocol, "pop3s") == 0){
#endif
		} else if(strcmp(protocol, "gopher") == 0) {
		} else if(strcmp(protocol, "pop3") == 0) {
		} else {
			__W3_Debug("Protocol", "Not suppported");
			W3_Free(w3);
			w3 = NULL;
			return w3;
		}
		w3->hostname = __W3_Strdup(hostname);
		if(ssl) __W3_Debug("Protocol", "Enabled SSL");
		w3->sock = __W3_DNS_Connect(hostname, ssl, port
#ifdef SSL_SUPPORT
					    ,
					    &w3->ssl, &w3->ssl_ctx
#endif
		);
		if(w3->sock == -1) {
			W3_Free(w3);
			w3 = NULL;
		}
	}
	return w3;
}

void W3_Set_Read_Size(struct W3* w3, size_t size) { w3->readsize = size; }

void W3_Set_Method(struct W3* w3, const char* method) {
	if(w3->method != NULL) free(w3->method);
	w3->method = __W3_Strdup(method);
}

void W3_Set_Path(struct W3* w3, const char* path) {
	if(w3->path != NULL) free(w3->path);
	w3->path = __W3_Strdup(path);
}

void W3_Send_Request(struct W3* w3) {
	if(strcmp(w3->protocol, "http") == 0
#ifdef SSL_SUPPORT
		|| strcmp(w3->protocol, "https") == 0
#endif
	) {
		__W3_HTTP_Request(w3);
	} else if(strcmp(w3->protocol, "gopher") == 0) {
		__W3_Gopher_Request(w3);
	} else if(strcmp(w3->protocol, "pop3") == 0
#ifdef SSL_SUPPORT
		|| strcmp(w3->protocol, "pop3s") == 0
#endif
	){
		__W3_POP3_Request(w3);
	} else if(strcmp(w3->protocol, "file") == 0) {
		__W3_File_Request(w3);
	}
}

void W3_Set_Data(struct W3* w3, char* data, size_t size) {
	w3->data = data;
	w3->size = size;
}

void W3_Set_Header(struct W3* w3, const char* key, const char* value) {
	char* tmp = __W3_Concat3("Setting the header `", key, "` to `");
	char* str = __W3_Concat3(tmp, value, "`");
	free(tmp);
	__W3_Debug("Header", str);
	free(str);
	int len = 0;
	if(w3->headers == NULL) {
		w3->headers = malloc(sizeof(*w3->headers) * (len + 3));
		w3->headers[len] = __W3_Strdup(key);
		w3->headers[len + 1] = __W3_Strdup(value);
		int i;
		for(i = 0; w3->headers[len][i] != 0; i++) {
			w3->headers[len][i] = tolower(w3->headers[len][i]);
		}
		w3->headers[len + 2] = NULL;
	} else {
		for(len = 0; w3->headers[len] != NULL; len++)
			;
		char** headers = w3->headers;
		w3->headers = malloc(sizeof(*w3->headers) * (len + 3));
		for(len = 0; headers[len] != NULL; len++) {
			w3->headers[len] = headers[len];
		}
		w3->headers[len] = __W3_Strdup(key);
		w3->headers[len + 1] = __W3_Strdup(value);
		w3->headers[len + 2] = NULL;
		int i;
		for(i = 0; w3->headers[len][i] != 0; i++) {
			w3->headers[len][i] = tolower(w3->headers[len][i]);
		}
		free(headers);
	}
}

void W3_On(struct W3* w3, const char* eventname, void* func) {
	int len = 0;
	if(w3->events == NULL) {
		w3->events = malloc(sizeof(*w3->events) * (len + 3));
		w3->events[len] = __W3_Strdup(eventname);
		w3->events[len + 1] = func;
		int i;
		for(i = 0; ((char*)w3->events[len])[i] != 0; i++) {
			((char*)w3->events[len])[i] = tolower(((char*)w3->events[len])[i]);
		}
		w3->events[len + 2] = NULL;
	} else {
		for(len = 0; w3->events[len] != NULL; len++)
			;
		void** events = w3->events;
		w3->events = malloc(sizeof(*w3->events) * (len + 3));
		for(len = 0; events[len] != NULL; len++) {
			w3->events[len] = events[len];
		}
		w3->events[len] = __W3_Strdup(eventname);
		w3->events[len + 1] = func;
		w3->events[len + 2] = NULL;
		int i;
		for(i = 0; ((char*)w3->events[len])[i] != 0; i += 2) {
			((char*)w3->events[len])[i] = tolower(((char*)w3->events[len])[i]);
		}
		free(events);
	}
}

void W3_Free(struct W3* w3) {
	__W3_Debug("LibW3", "Freeing");
	if(w3->method != NULL) free(w3->method);
	if(w3->path != NULL) free(w3->path);
	if(w3->protocol != NULL) free(w3->protocol);
	if(w3->hostname != NULL) free(w3->hostname);
	if(w3->headers != NULL) {
		int i;
		for(i = 0; w3->headers[i] != 0; i++) free(w3->headers[i]);
		free(w3->headers);
	}
	if(w3->props != NULL) {
		int i;
		for(i = 0; w3->props[i] != 0; i++) free(w3->props[i]);
		free(w3->props);
	}
	if(w3->events != NULL) {
		int i;
		for(i = 0; w3->events[i] != 0; i += 2) free(w3->events[i]);
		free(w3->events);
	}
	W3_Disconnect(w3);
	free(w3);
}

void W3_Disconnect(struct W3* w3) {
#ifdef SSL_SUPPORT
	if(w3->ssl != NULL) {
		SSL_shutdown(w3->ssl);
		SSL_CTX_free(w3->ssl_ctx);
		SSL_free(w3->ssl);
		w3->ssl = NULL;
	}
#endif
	if(w3->sock != -1) close(w3->sock);
}
