/* $Id$ */
#include "W3DNS.h"

#include "W3Util.h"

#include <stdlib.h>
#include <string.h>

#ifdef __MINGW32__
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#else
#include <sys/socket.h>
#include <netdb.h>
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#ifdef SSL_SUPPORT
#include <openssl/ssl.h>
#endif

#ifndef __MINGW32__
#define ADDRINFO struct addrinfo
#endif

int __W3_DNS_Connect(const char* hostname, bool ssl, uint16_t port
#ifdef SSL_SUPPORT
	,
	void** o_ssl,
	void** o_ctx
#endif
){
	__W3_Debug("DNS-Connect", "Resolving");
	ADDRINFO hints;
	ADDRINFO* result;
	ADDRINFO* rp;
	int s;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;
	char* strport = malloc(6); /* Enough for 5 digits */
	memset(strport, 0, 6);
	sprintf(strport, "%d", port);
	s = getaddrinfo(hostname, strport, &hints, &result);
	if(s != 0){
		__W3_Debug("Resolve", "Failed");
		return -1;	/* Failed to resolve */
	}
	int sock;
	for(rp = result; rp != NULL; rp = rp->ai_next){
		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if(sock == -1) continue;
		if(connect(sock, rp->ai_addr, rp->ai_addrlen) != -1) break;
		close(sock);
	}
	freeaddrinfo(result);
	free(strport);
	if(rp == NULL){
		__W3_Debug("Connect", "Failed to connect");
		return -1;	/* Failed to connect */
	}
	__W3_Debug("Connect", "Conencted");
#ifdef SSL_SUPPORT
	if(ssl){
		__W3_Debug("SSL", "Initializing");
		const SSL_METHOD* method = TLSv1_2_client_method();
		*o_ctx = SSL_CTX_new(method);
		*o_ssl = SSL_new(*o_ctx);
		SSL_set_fd(*o_ssl, sock);
		if(SSL_connect(*o_ssl) != 1){
			SSL_CTX_free(*o_ctx);
			SSL_free(*o_ssl);
			*o_ctx = NULL;
			*o_ssl = NULL;
			close(sock);
			sock = -1;

		}else{
			__W3_Debug("SSL", "Connected");
		}
	}
#endif
	return sock;
}
