/* $Id$ */
#include "W3IRC.h"

#include "W3Core.h"
#include "W3DNS.h"
#include "W3Util.h"

#include <stdlib.h>
#include <string.h>

#ifndef __MINGW32__
extern int strcasecmp(const char* s1, const char* s2);
#endif

void __W3_IRC_Request(struct W3* w3) { __W3_Debug("LibW3-IRC", "Sending the request"); }
