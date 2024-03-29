/* $Id$ */
#ifndef __W3TAG_H__
#define __W3TAG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

void W3_Tag_Parse(char* data, size_t size, void (*tagfunc)(char* tagname, char* attr), void (*textfunc)(char* data));
char* W3_Tag_Attr(char* data, const char* name);

#ifdef __cplusplus
}
#endif

#endif
