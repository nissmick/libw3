/* $Id$ */
#ifndef __W3UTIL_H__
#define __W3UTIL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "W3Core.h"

#include <stdbool.h>

void __W3_Debug(const char* title, const char* message);
char* __W3_Concat(const char* str1, const char* str2);
char* __W3_Concat3(const char* str1, const char* str2, const char* str3);
char* __W3_Strdup(const char* str);
unsigned long __W3_Auto_Write(struct W3* w3, char* data, unsigned long length);
unsigned long __W3_Auto_Read(struct W3* w3, char* data, unsigned long length);
bool __W3_Have_Header(struct W3* w3, const char* name);
void* __W3_Get_Event(struct W3* w3, const char* eventname);
char* __W3_Get_Platform(void);

#ifdef __cplusplus
}
#endif

#endif
