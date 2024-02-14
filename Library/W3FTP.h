/* $Id$ */
#ifndef __W3FTP_H__
#define __W3FTP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "W3Core.h"

void __W3_FTP_Request(struct W3* w3);
void W3_FTP_Set_Username(struct W3* w3, const char* username);
void W3_FTP_Set_Password(struct W3* w3, const char* password);
void W3_FTP_Send_Request(struct W3* w3);
void W3_FTP_Disconnect(struct W3* w3);

#ifdef __cplusplus
}
#endif

#endif
