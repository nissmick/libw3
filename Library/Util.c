/* $Id$ */
#include "W3Util.h"

#include "W3Core.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __MINGW32__
#include <windows.h>
#include <winsock.h>
#else
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/utsname.h>
#endif

#ifdef SSL_SUPPORT
#include <openssl/ssl.h>
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

unsigned long __W3_Auto_Write(struct W3* w3, char* data, unsigned long length){
#ifdef SSL_SUPPORT
	if(w3->ssl != NULL){
		return SSL_write(w3->ssl, data, length);
	}else{
		return send(w3->sock, data, length, 0);
	}
#else
	return send(w3->sock, data, length, 0);
#endif
}

unsigned long __W3_Auto_Read(struct W3* w3, char* data, unsigned long length){
#ifdef SSL_SUPPORT
	if(w3->ssl != NULL){
		return SSL_read(w3->ssl, data, length);
	}else{
		return recv(w3->sock, data, length, 0);
	}
#else
	return recv(w3->sock, data, length, 0);
#endif
}

void* __W3_Get_Event(struct W3* w3, const char* eventname){
	if(w3->events == NULL) return NULL;
	int i;
	for(i = 0; w3->events[i] != NULL; i += 2){
		if(strcmp(w3->events[i], eventname) == 0){
			return w3->events[i + 1];
		}
	}
	return NULL;
}

bool __W3_Have_Header(struct W3* w3, const char* name){
	if(w3->headers == NULL) return false;
	int i;
	for(i = 0; w3->headers[i] != NULL; i += 2){
		if(strcmp(w3->headers[i], name) == 0){
			return true;
		}
	}
	return false;
}

char* __W3_Get_Platform(void){
#ifdef __MINGW32__
	return __W3_Strdup("Windows");
#else
	struct utsname un;
	uname(&un);
	return __W3_Concat3(un.sysname, "/", un.release);
#endif
}
