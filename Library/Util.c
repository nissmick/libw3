/* $Id$ */
#include "W3Util.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define __DEBUG_LEN 12

void __W3_Debug(const char* title, const char* message){
#ifdef __DEBUG__
	char* periods = malloc(__DEBUG_LEN - strlen(title) + 1);
	periods[__DEBUG_LEN - strlen(title)] = 0;
	memset(periods, '.', __DEBUG_LEN - strlen(title));
	fprintf(stderr, "%s%s %s\n", title, periods, message);
#endif
}
