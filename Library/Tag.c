/* $Id$ */
#include "W3Tag.h"

#include "W3Util.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

extern int strcasecmp(const char* s1, const char* s2);

void W3_Tag_Parse(char* data, size_t size, void (*tagfunc)(char* tagname, char* attr), void (*textfunc)(char* data)) {
	int i;
	int phase = 0;
	char* tagn = malloc(1);
	tagn[0] = 0;
	char* cbuf = malloc(2);
	cbuf[1] = 0;
	char* attrlist = NULL;
	char* attr = malloc(1);
	attr[0] = 0;
	char* text = malloc(1);
	text[0] = 0;
	bool dq = false;
	for(i = 0; i < size; i++) {
		if(phase == 1 && data[i] != ' ' && data[i] != '>') {
			cbuf[0] = data[i];
			char* tmp = tagn;
			tagn = __W3_Concat(tmp, cbuf);
			free(tmp);
		} else if(phase == 2 && (dq || data[i] != '>')) {
			if(data[i] == '"') dq = !dq;
			cbuf[0] = data[i];
			char* tmp = attr;
			attr = __W3_Concat(tmp, cbuf);
			free(tmp);
		}
		if(phase == 0 && data[i] == '<') {
			if(strlen(text) > 0) {
				textfunc(text);
			}
			free(text);
			text = malloc(1);
			text[0] = 0;
			phase = 1;
			free(tagn);
			tagn = malloc(1);
			tagn[0] = 0;
		} else if(phase == 0) {
			cbuf[0] = data[i];
			char* tmp = text;
			text = __W3_Concat(tmp, cbuf);
			free(tmp);
		} else if(phase == 1 && (data[i] == ' ' || data[i] == '>')) {
			if(data[i] == ' ') {
				phase = 2;
				free(attr);
				attr = malloc(1);
				attr[0] = 0;
			} else if(data[i] == '>') {
				tagfunc(tagn, NULL);
				phase = 0;
			}
		} else if(phase == 2 && data[i] == '>') {
			tagfunc(tagn, attr);
			phase = 0;
		}
	}
	free(text);
	free(attr);
	free(tagn);
	free(cbuf);
}

char* W3_Tag_Attr(char* data, const char* name) {
	char* ret = NULL;
	int i;
	int phase = 0;
	char* cbuf = malloc(2);
	cbuf[1] = 0;
	char* key = malloc(1);
	key[0] = 0;
	bool dq = false;
	for(i = 0;; i++) {
		cbuf[0] = data[i];
		if(phase == 0 && data[i] != 0 && data[i] != '=') {
			char* tmp = key;
			key = __W3_Concat(tmp, cbuf);
			free(tmp);
		} else if(phase == 0) {
			phase = 2;
			if(strcasecmp(key, name) == 0) phase = 1;
			free(key);
			key = malloc(1);
			key[0] = 0;
			if(data[i] == 0) break;
		} else if(phase == 1 && (dq ? dq : (data[i] != 0 && data[i] != ' '))) {
			if(data[i] == '"') {
				dq = !dq;
			} else {
				if(ret == NULL) {
					ret = malloc(1);
					ret[0] = 0;
				}
				char* tmp = ret;
				ret = __W3_Concat(tmp, cbuf);
				free(tmp);
			}
		} else if(phase == 1) {
			phase = 0;
			break;
		} else if(phase == 2 && (dq ? dq : (data[i] != 0 && data[i] != ' '))) {
			if(data[i] == '"') {
				dq = !dq;
			}
		} else if(phase == 2) {
			phase = 0;
			if(data[i] == 0) break;
		}
	}
	free(key);
	free(cbuf);
	return ret;
}
