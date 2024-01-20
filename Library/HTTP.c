/* $Id$ */
#include "W3HTTP.h"

#include "W3Core.h"
#include "W3Util.h"

#include <string.h>
#include <stdlib.h>

void __W3_HTTP_Request(struct W3* w3){
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
	if(!__W3_Have_Header(w3, "user-agent")){
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
	if(w3->headers != NULL){
		int i;
		for(i = 0; w3->headers[i] != NULL; i += 2){
			if(strcmp(w3->headers[i], "connection") == 0) continue;
			__W3_Auto_Write(w3, w3->headers[i], strlen(w3->headers[i]));
			__W3_Auto_Write(w3, ": ", 2);
			__W3_Auto_Write(w3, w3->headers[i + 1], strlen(w3->headers[i + 1]));
			__W3_Auto_Write(w3, "\r\n", 2);
		}
	}
	__W3_Auto_Write(w3, "\r\n", 2);
	__W3_Auto_Write(w3, "\r\n", 2);
	char* buf = malloc(512);
	char* statusbuf = malloc(1);
	statusbuf[0] = 0;
	char* headerbuf = malloc(1);
	headerbuf[0] = 0;
	int phase = 0;
	while(1){
		int l = __W3_Auto_Read(w3, buf, 512);
		if(l <= 0) break;
		int i;
		for(i = 0; i < l; i++){
			if(phase == 0){
				if(buf[i] == '\r'){
					int phase2 = 0;
					int j = 0;
					int start_status = 0;
					for(j = 0; statusbuf[j] != 0; j++){
						if(phase2 == 0){
							if(statusbuf[j] == ' '){
								phase2++;
								start_status = j + 1;
							}
						}else if(phase2 == 1){
							if(statusbuf[j] == ' '){
								char* code = malloc(j - start_status + 1);
								code[j - start_status] = 0;
								memcpy(code, statusbuf + start_status, j - start_status);
								w3->status = atoi(code);
								void* funcptr = __W3_Get_Event(w3, "status");
								if(funcptr != NULL){
									void(*func)(struct W3*, int) = (void(*)(struct W3*, int))funcptr;
									func(w3, w3->status);
								}
								free(code);
								break;
							}
						}
					}
					phase++;
					break;
				}else{
					char* oldbuf = statusbuf;
					statusbuf = malloc(strlen(oldbuf) + 2);
					strcpy(statusbuf, oldbuf);
					statusbuf[strlen(oldbuf)] = buf[i];
					statusbuf[strlen(oldbuf) + 1] = 0;
					free(oldbuf);
				}
			}
		}
	}
	free(headerbuf);
	free(statusbuf);
	free(buf);
}
