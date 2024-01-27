/* $Id$ */
#include "W3HTTP.h"

#include "W3Core.h"
#include "W3DNS.h"
#include "W3URL.h"
#include "W3Util.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

extern int strcasecmp(const char* s1, const char* s2);

void __W3_HTTP_Request(struct W3* w3) {
	__W3_Debug("LibW3-HTTP", "Sending the request");
	__W3_Auto_Write(w3, w3->method, strlen(w3->method));
	__W3_Auto_Write(w3, " ", 1);
	__W3_Auto_Write(w3, w3->path, strlen(w3->path));
	__W3_Auto_Write(w3, " ", 1);
	__W3_Auto_Write(w3, "HTTP/1.1", 8);
	__W3_Auto_Write(w3, "\r\n", 2);
	__W3_Auto_Write(w3, "Host: ", 6);
	__W3_Auto_Write(w3, w3->hostname, strlen(w3->hostname));
	__W3_Auto_Write(w3, "\r\n", 2);
	if(!__W3_Have_Header(w3, "user-agent")) {
		__W3_Auto_Write(w3, "User-Agent: ", 12);
		__W3_Auto_Write(w3, "LibW3/", 6);
		__W3_Auto_Write(w3, LIBW3_VERSION, strlen(LIBW3_VERSION));
		char* platform = __W3_Get_Platform();
		__W3_Auto_Write(w3, " (", 2);
		__W3_Auto_Write(w3, platform, strlen(platform));
		free(platform);
		__W3_Auto_Write(w3, ")", 1);
		__W3_Auto_Write(w3, "\r\n", 2);
	}
	__W3_Auto_Write(w3, "Connection: ", 12);
	__W3_Auto_Write(w3, "closed", 6);
	__W3_Auto_Write(w3, "\r\n", 2);
	if(w3->headers != NULL) {
		int i;
		for(i = 0; w3->headers[i] != NULL; i += 2) {
			if(strcmp(w3->headers[i], "connection") == 0) continue;
			if(strcmp(w3->headers[i], "content-length") == 0) continue;
			__W3_Auto_Write(w3, w3->headers[i], strlen(w3->headers[i]));
			__W3_Auto_Write(w3, ": ", 2);
			__W3_Auto_Write(w3, w3->headers[i + 1], strlen(w3->headers[i + 1]));
			__W3_Auto_Write(w3, "\r\n", 2);
		}
	}
	if(w3->data != NULL) {
		char* len = malloc(129);
		memset(len, 0, 129);
		sprintf(len, "%d", w3->size);
		__W3_Auto_Write(w3, "content-length: ", 16);
		__W3_Auto_Write(w3, len, strlen(len));
		__W3_Auto_Write(w3, "\r\n", 2);
		free(len);
	}
	__W3_Auto_Write(w3, "\r\n", 2);
	if(w3->data != NULL) {
		__W3_Auto_Write(w3, w3->data, w3->size);
	}
	char* buf = malloc(w3->readsize);
	char* statusbuf = malloc(1);
	statusbuf[0] = 0;
	char* headerbuf = malloc(1);
	headerbuf[0] = 0;
	int phase = 0;
	bool chunked = false;
	int chunkphase = 0;
	char* chunklen = NULL;
	int chunkincr = 0;
	char* chunk = NULL;
	bool breakchunk = false;
	int chunksize;
	size_t allsize = -1;
	char* redir = NULL;
	while(true) {
		int l = __W3_Auto_Read(w3, buf, w3->readsize);
		if(l <= 0) break;
		int i;
		for(i = 0; i < l; i++) {
			if(phase == 0) {
				if(buf[i] == '\r') {
					int phase2 = 0;
					int j = 0;
					int start_status = 0;
					for(j = 0; statusbuf[j] != 0; j++) {
						if(phase2 == 0) {
							if(statusbuf[j] == ' ') {
								phase2++;
								start_status = j + 1;
							}
						} else if(phase2 == 1) {
							if(statusbuf[j] == ' ') {
								char* code = malloc(j - start_status + 1);
								code[j - start_status] = 0;
								memcpy(code, statusbuf + start_status, j - start_status);
								w3->status = atoi(code);
								void* funcptr = __W3_Get_Event(w3, "status");
								if(funcptr != NULL) {
									void (*func)(struct W3*, int) = (void (*)(struct W3*, int))funcptr;
									func(w3, w3->status);
								}
								free(code);
								break;
							}
						}
					}
					phase++;
				} else {
					char* oldbuf = statusbuf;
					statusbuf = malloc(strlen(oldbuf) + 2);
					strcpy(statusbuf, oldbuf);
					statusbuf[strlen(oldbuf)] = buf[i];
					statusbuf[strlen(oldbuf) + 1] = 0;
					free(oldbuf);
				}
			} else if(phase == 1) {
				char* oldbuf = headerbuf;
				headerbuf = malloc(strlen(oldbuf) + 2);
				strcpy(headerbuf, oldbuf);
				headerbuf[strlen(oldbuf)] = buf[i];
				headerbuf[strlen(oldbuf) + 1] = 0;
				free(oldbuf);
				int len = strlen(headerbuf);
				if(len >= 4) {
					if(headerbuf[len - 1] == '\n' && headerbuf[len - 2] == '\r' && headerbuf[len - 3] == '\n' && headerbuf[len - 4] == '\r') {
						headerbuf[len - 4] = 0;
						char* headers = malloc(strlen(headerbuf) + 1);
						int j;
						int incr = 0;
						int start = 0;
						for(j = 1;; j++) {
							char c = headerbuf[j];
							if(c == '\r' || c == 0) {
								headers[incr] = 0;
								headers[incr + 1] = 0;
								char* data = __W3_Strdup(headers + start);
								int k;
								for(k = 0; data[k] != 0; k++) {
									if(data[k] == ':') {
										data[k] = 0;
										k++;
										for(; data[k] != 0 && data[k] != ' ' && data[k] != '\t'; k++) data[k] = 0;
										if(data[k] == ' ' || data[k] == '\t') {
											void* funcptr = __W3_Get_Event(w3, "header");
											if(funcptr != NULL) {
												void (*func)(struct W3*, char*, char*) = (void (*)(struct W3*, char*, char*))funcptr;
												func(w3, data, data + k + 1);
											}
											if(strcasecmp(data, "transfer-encoding") == 0 && strcasecmp(data + k + 1, "chunked") == 0) {
												chunked = true;
											} else if(strcasecmp(data, "content-length") == 0) {
												allsize = atoi(data + k + 1);
											} else if(strcasecmp(data, "location") == 0) {
												if(redir != NULL) free(redir);
												redir = __W3_Strdup(data + k + 1);
											}
										}
										break;
									}
								}
								free(data);
								start = incr + 1;
								incr++;
								j++;
								if(c == 0) break;
							} else {
								headers[incr] = c;
								headers[incr + 1] = 0;
								incr++;
							}
						}
						free(headers);
						phase++;
					}
				}
			} else if(phase == 2) {
				if(chunked) {
					int j;
					for(j = i; j < l; j++) {
						char c = buf[j];
						if(chunkphase == 0) {
							if(c == '\n') {
								if(chunklen != NULL) {
									chunkphase = 1;
									chunkincr = strtol(chunklen, NULL, 16);
									free(chunklen);
									chunklen = NULL;
									if(chunkincr == 0) {
										breakchunk = true;
										break;
									} else {
										chunk = malloc(chunkincr);
										chunksize = chunkincr;
									}
								}
								continue;
							} else if(c == '\r')
								continue;
							if(chunklen == NULL) {
								chunklen = malloc(1);
								chunklen[0] = 0;
							}
							char* cbuf = malloc(2);
							cbuf[0] = c;
							cbuf[1] = 0;
							char* tmp = chunklen;
							chunklen = __W3_Concat(tmp, cbuf);
							free(tmp);
							free(cbuf);
						} else if(chunkphase == 1) {
							chunkincr--;
							if(chunkincr >= 0) {
								chunk[chunksize - chunkincr - 1] = c;
							}
							if(chunkincr == -2) {
								chunkphase = 0;
								free(chunk);
								chunk = NULL;
							} else if(chunkincr == 0) {
								void* funcptr = __W3_Get_Event(w3, "data");
								if(funcptr != NULL) {
									void (*func)(struct W3*, char*, size_t) = (void (*)(struct W3*, char*, size_t))funcptr;
									func(w3, chunk, chunksize);
								}
							}
						}
					}
				} else {
					void* funcptr = __W3_Get_Event(w3, "data");
					if(funcptr != NULL) {
						void (*func)(struct W3*, char*, size_t) = (void (*)(struct W3*, char*, size_t))funcptr;
						char* buffer = malloc(l - i);
						memcpy(buffer, buf + i, l - i);
						func(w3, buffer, l - i);
						free(buffer);
					}
					allsize -= l - i;
					if(allsize == 0) {
						breakchunk = 1;
					}
				}
				break;
			}
		}
		if(breakchunk) break;
	}
	free(headerbuf);
	free(statusbuf);
	free(buf);
	if(chunklen != NULL) free(chunklen);
	if(chunk != NULL) free(chunk);
	if(redir != NULL && __W3_Have_Prop(w3, "HTTP_REDIRECT")) {
		__W3_Debug("LibW3-HTTP", "Redirecting");
		W3_Disconnect(w3);
		struct W3URL* u = W3_Parse_URL(redir);
		if(u != NULL) {
			w3->sock = __W3_DNS_Connect(u->host, strcmp(u->protocol, "https") == 0 ? true : false, u->port
#ifdef SSL_SUPPORT
						    ,
						    &w3->ssl, &w3->ssl_ctx
#endif
			);
			free(w3->protocol);
			w3->protocol = __W3_Strdup(u->protocol);
			free(w3->hostname);
			w3->hostname = __W3_Strdup(u->host);
			W3_Set_Path(w3, u->path);
			w3->port = u->port;
			W3_Free_URL(u);
		} else if(redir[0] == '/') {
			w3->sock = __W3_DNS_Connect(w3->hostname, strcmp(w3->protocol, "https") == 0 ? true : false, w3->port
#ifdef SSL_SUPPORT
						    ,
						    &w3->ssl, &w3->ssl_ctx
#endif
			);
			W3_Set_Path(w3, redir);
		}
		W3_Send_Request(w3);
	}
	if(redir != NULL) free(redir);
}

void W3_Enable_Redirect(struct W3* w3) { __W3_Add_Prop(w3, "HTTP_REDIRECT", "YES"); }
