/* $Id$ */
#include "W3Tag.h"

#include "W3Util.h"

#include <stddef.h>
#include <stdlib.h>

void W3_Tag_Parse(char* data, size_t size, void(*tagfunc)(char* tagname, char** attr)){
	int i;
	int phase = 0;
	char* tagn = malloc(1);
	tagn[0] = 0;
	char* cbuf = malloc(2);
	cbuf[1] = 0;
	char* attrlist = NULL;
	for(i = 0; i < size; i++){
		if(phase == 1 && data[i] != ' ' && data[i] != '>'){
			cbuf[0] = data[i];
			char* tmp = tagn;
			tagn = __W3_Concat(tmp, cbuf);
			free(tmp);
		}
		if(phase == 0 && data[i] == '<'){
			phase = 1;
			free(tagn);
			tagn = malloc(1);
			tagn[0] = 0;
		}else if(phase == 1 && (data[i] == ' ' || data[i] == '>')){
			if(data[i] == ' '){
				phase = 2;
			}else if(data[i] == '>'){
				tagfunc(tagn, NULL);
				phase = 0;
			}
		}else if(phase == 2 && data[i] == '>'){
			tagfunc(tagn, attrlist);
			phase = 0;
		}
	}
	free(tagn);
	free(cbuf);
}
