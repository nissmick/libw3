/* $Id$ */
#ifndef __W3IRC_H__
#define __W3IRC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "W3Core.h"

void __W3_IRC_Request(struct W3* w3);
void W3_IRC_Send_Request(struct W3* w3);
void W3_IRC_Disconnect(struct W3* w3);

#ifdef __cplusplus
}
#endif

#endif
