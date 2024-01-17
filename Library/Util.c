/* $Id$ */
#include "W3Util.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef __MINGW32__
#include <windows.h>
#include <winsock.h>
#else
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#define __DEBUG_LEN 12

void __W3_Debug(const char* title, const char* message){
#ifdef __DEBUG__
	char* periods = malloc(__DEBUG_LEN - strlen(title) + 1);
	periods[__DEBUG_LEN - strlen(title)] = 0;
	memset(periods, '.', __DEBUG_LEN - strlen(title));
	fprintf(stderr, "%s%s %s\n", title, periods, message);
#endif
}

char* __W3_Concat(const char* str1, const char* str2){
	char* str = malloc(strlen(str1) + strlen(str2) + 1);
	strcpy(str, str1);
	strcpy(str + strlen(str1), str2);
	str[strlen(str1) + strlen(str2)] = 0;
	return str;
}

char* __W3_Concat3(const char* str1, const char* str2, const char* str3){
	char* tmp = __W3_Concat(str1, str2);
	char* str = __W3_Concat(tmp, str3);
	free(tmp);
	return str;
}

char* __W3_Strdup(const char* str){
	char* result = malloc(strlen(str) + 1);
	memcpy(result, str, strlen(str) + 1);
	return result;
}
