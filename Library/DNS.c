/* $Id$ */
#include "W3DNS.h"

#include "W3Util.h"

#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

int __W3_DNS_Connect(const char* hostname, bool ssl, uint16_t port){
	__W3_Debug("DNS-Connect", "Resolving");
	struct addrinfo hints;
	struct addrinfo* result;
	struct addrinfo* rp;
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
}
