/* $Id$ */
#ifndef __W3URL_H__
#define __W3URL_H__

#ifdef __cplusplus
extern "C" {
#endif

struct W3URL {
	char* protocol;
	char* host;
	int port;
	char* path;
	char* username;
	char* password;
};

struct W3URL* W3_Parse_URL(const char* url);
void W3_Free_URL(struct W3URL* url);

#ifdef __cplusplus
}
#endif

#endif
