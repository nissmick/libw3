/* $Id$ */
#ifndef __W3CORE_H__
#define __W3CORE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

#include "W3Version.h"

struct W3 {
	int sock;	 /* Socket */
	char* protocol;	 /* As you can read from its name */
	char* method;	 /* Used in HTTP	*/
	char* path;	 /* As you can read from its name */
	char* hostname;	 /* As you can read from its name */
	char** headers;	 /* As you can read from its name */
	void** events;	 /* As you can read from its name */
	int status;	 /* As you can read from its name */
	char* data;	 /* As you can read from its name */
	size_t size;	 /* Size of the data */
	size_t readsize; /* Read buffer size, default is 512 */
	char** props;	 /* Properties */
#ifdef SSL_SUPPORT
	void* ssl;     /* Actually SSL*, NULL if no SSL */
	void* ssl_ctx; /* Actually SSL_CTX* */
#endif
};

int W3_Library_Init(void);						    /* Initialize the Library */
struct W3* W3_Create(const char* protocol, const char* hostname, int port); /* Create the struct */

void W3_Set_Method(struct W3* w3, const char* method);		       /* Set the method */
void W3_Set_Path(struct W3* w3, const char* path);		       /* Set the path */
void W3_Send_Request(struct W3* w3);				       /* Send the request */
void W3_Set_Header(struct W3* w3, const char* key, const char* value); /* Set the header */
void W3_Free(struct W3* w3);					       /* Free the struct */
void W3_On(struct W3* w3, const char* eventname, void* func);	       /* Set Handlers */
void W3_Disconnect(struct W3* w3);				       /* Disconnect */
void W3_Set_Data(struct W3* w3, char* data, size_t size);	       /* Send the data - LibW3 won't free the data */
void W3_Set_Read_Size(struct W3* w3, size_t size);		       /* Change the read buffer size */

#ifdef __cplusplus
}
#endif

#endif
