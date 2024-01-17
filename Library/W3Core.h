/* $Id$ */
#ifndef __W3CORE_H__
#define __W3CORE_H__

#include <stdbool.h>

#define LIBW3_VERSION "0.0"

struct W3 {
	int sock;	/* Socket */
	char* protocol;	/* As you can read from its name */
	char* method;	/* Used in HTTP	*/
	char* path;	/* As you can read from its name */
#ifdef SSL_SUPPORT
	void* ssl;	/* Actually SSL*, NULL if no SSL */
	void* ssl_ctx;	/* Actually SSL_CTX* */
#endif
};

int W3_Library_Init(void);							/* Initialize the Library */
struct W3* W3_Create(const char* protocol, const char* hostname, int port);	/* Create the struct */

void W3_Set_Method(struct W3* w3, const char* method);				/* Set the method */
void W3_Set_Path(struct W3* w3, const char* path);				/* Set the path */
void W3_Send_Request(struct W3* w3);						/* Send the request */
void W3_Free(struct W3* w3);							/* Free the struct */

#endif
