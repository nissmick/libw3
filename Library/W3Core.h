/* $Id$ */
#ifndef __W3CORE_H__
#define __W3CORE_H__

#include <stdbool.h>

#define LIBW3_VERSION "0.0"

struct W3 {
	int sock;	/* Socket */
#ifdef SSL_SUPPORT
	void* ssl;	/* Actually SSL*, NULL if no SSL */
	void* ssl_ctx;	/* Actually SSL_CTX* */
#endif
};

int W3_Library_Init(void);							/* Initialize the Library */
struct W3* W3_Create(const char* protocol, const char* hostname, int port);	/* Create the struct */

#endif
