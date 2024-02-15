/* $Id$ */
#ifndef __W3NNTP_H__
#define __W3NNTP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "W3Core.h"

void __W3_NNTP_Request(struct W3* w3);
void W3_NNTP_Send_Request(struct W3* w3);
void W3_NNTP_Disconnect(struct W3* w3);

#ifdef __cplusplus
}
#endif

#endif
