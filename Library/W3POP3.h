/* $Id$ */
#ifndef __W3POP3_H__
#define __W3POP3_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "W3Core.h"

void __W3_POP3_Request(struct W3* w3);
void W3_POP3_Set_Username(struct W3* w3, const char* username);
void W3_POP3_Set_Password(struct W3* w3, const char* password);
void W3_POP3_Send_Request(struct W3* w3);
void W3_POP3_Disconnect(struct W3* w3);

#ifdef __cplusplus
}
#endif

#endif
