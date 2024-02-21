/* $Id$ */
#ifndef __W3IRC_H__
#define __W3IRC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "W3Core.h"

void __W3_IRC_Request(struct W3* w3);
void W3_IRC_Set_Username(struct W3* w3, const char* username);
void W3_IRC_Set_Hostname(struct W3* w3, const char* hostname);
void W3_IRC_Set_Realname(struct W3* w3, const char* realname);
void W3_IRC_Set_Servername(struct W3* w3, const char* servername);
void W3_IRC_Set_Nickname(struct W3* w3, const char* nickname);
void W3_IRC_Send_Request(struct W3* w3);
void W3_IRC_Disconnect(struct W3* w3);

#ifdef __cplusplus
}
#endif

#endif
