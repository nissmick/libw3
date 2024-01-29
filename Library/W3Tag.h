/* $Id$ */
#ifndef __W3TAG_H__
#define __W3TAG_H__

#include <stddef.h>

void W3_Tag_Parse(char* data, size_t size, void(*func)(char* tagname, char** attr));

#endif
