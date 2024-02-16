/* $Id$ */
#include "W3FTP.h"

#include "W3Core.h"
#include "W3DNS.h"
#include "W3Util.h"

#include <stdlib.h>
#include <string.h>

#ifdef __MINGW32__
#include <windows.h>
#include <winsock2.h>
#else
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#endif

#ifndef __MINGW32__
extern int strcasecmp(const char* s1, const char* s2);
#endif

void __W3_FTP_Start_Passive(struct W3* w3) { __W3_Auto_Write(w3, "PASV\r\n", 6); }

void __W3_FTP_Request(struct W3* w3) {
	if(__W3_Get_Prop(w3, "FTP_USERNAME") == NULL || __W3_Get_Prop(w3, "FTP_PASSWORD") == NULL) {
		__W3_Debug("LibW3-FTP", "Set the username/password");
		void* funcptr = __W3_Get_Event(w3, "error");
		if(funcptr != NULL) {
			void (*func)(struct W3*, const char*) = (void (*)(struct W3*, const char*))funcptr;
			func(w3, "did-not-auth");
		}
		return;
	}
	__W3_Debug("LibW3-FTP", "Sending the request");
	int cond = 1;
	w3->generic = &cond;
	char* buf = malloc(w3->readsize);
	bool cont = false;
	int status = 0;
	bool gotstatus = false;
	char* buffer = malloc(1);
	buffer[0] = 0;
	char* cbuf = malloc(2);
	cbuf[1] = 0;
	int auth = 0;
	int pasvsock = -1;
	while(true) {
		int len = __W3_Auto_Read(w3, buf, w3->readsize);
		if(len <= 0) break;
		int i;
		for(i = 0; i < len; i++) {
			if((buf[i] == ' ' || buf[i] == '-') && !gotstatus) {
				cont = buf[i] == '-';
				gotstatus = true;
			} else if(!gotstatus) {
				status = status * 10;
				status += buf[i] - '0';
			} else if(buf[i] != '\r' && buf[i] != '\n') {
				cbuf[0] = buf[i];
				char* tmp = buffer;
				buffer = __W3_Concat(tmp, cbuf);
				free(tmp);
			} else if(buf[i] == '\n') {
				gotstatus = false;
				cbuf[0] = '\n';
				char* tmp = buffer;
				buffer = __W3_Concat(tmp, cbuf);
				free(tmp);
				if(!cont) {
					if(status == 220 && auth == 0) {
						__W3_Auto_Write(w3, "USER ", 5);
						__W3_Auto_Write(w3, __W3_Get_Prop(w3, "FTP_USERNAME"), strlen(__W3_Get_Prop(w3, "FTP_USERNAME")));
						__W3_Auto_Write(w3, "\r\n", 2);
						auth = 1;
					} else if(status == 331 && auth == 1) {
						__W3_Auto_Write(w3, "PASS ", 5);
						__W3_Auto_Write(w3, __W3_Get_Prop(w3, "FTP_PASSWORD"), strlen(__W3_Get_Prop(w3, "FTP_PASSWORD")));
						__W3_Auto_Write(w3, "\r\n", 2);
						auth = 2;
					} else if(status == 230 && auth == 2) {
						auth = 3;
					} else if(auth < 3) {
						__W3_Debug("LibW3-FTP", "Login failed");
						void* funcptr = __W3_Get_Event(w3, "error");
						if(funcptr != NULL) {
							void (*func)(struct W3*, const char*) = (void (*)(struct W3*, const char*))funcptr;
							func(w3, "login-fail");
						}
						goto ftp_stop;
					} else if(auth == 3) {
						if(status == 227 && (cond & 1)) {
							/* Passive mode */
							int j;
							int cnt = 0;
							bool pasv = false;
							char* addr = malloc(1);
							addr[0] = 0;
							char* numbuf = malloc(1);
							numbuf[0] = 0;
							int port = 0;
							for(j = 0; buffer[j] != 0; j++) {
								if(buffer[j] == '(') {
									pasv = true;
								} else if(pasv) {
									if(buffer[j] == ',' || (pasv && buffer[j] == ')')) {
										cnt++;
										if(cnt < 4) {
											cbuf[0] = '.';
											char* tmp = addr;
											addr = __W3_Concat(tmp, cbuf);
											free(tmp);
										} else {
											port = port << 8;
											port |= atoi(numbuf);
											free(numbuf);
											numbuf = malloc(1);
											numbuf[0] = 0;
										}
										if(buffer[j] == ')') break;
									} else {
										if(cnt < 4) {
											cbuf[0] = buffer[j];
											char* tmp = addr;
											addr = __W3_Concat(tmp, cbuf);
											free(tmp);
										} else {
											cbuf[0] = buffer[j];
											char* tmp = numbuf;
											numbuf = __W3_Concat(tmp, cbuf);
											free(tmp);
										}
									}
								}
							}
							free(numbuf);
							pasvsock = __W3_DNS_Connect(addr, false, port
#ifdef SSL_SUPPORT
										    ,
										    NULL, NULL
#endif
							);
							free(addr);
							if(strcasecmp(w3->method, "LIST") == 0) {
								__W3_Auto_Write(w3, "LIST", 4);
								__W3_Auto_Write(w3, "\r\n", 2);
							}
							char* ftpbuf = malloc(w3->readsize);
							while(true) {
								int ftplen = recv(pasvsock, ftpbuf, w3->readsize, 0);
								if(ftplen <= 0) break;
								void* funcptr = __W3_Get_Event(w3, "data");
								if(funcptr != NULL) {
									void (*func)(struct W3*, char*, size_t) = (void (*)(struct W3*, char*, size_t))funcptr;
									func(w3, ftpbuf, ftplen);
								}
							}
							free(ftpbuf);
							cond &= ~1;
							free(w3->method);
							w3->method = NULL;
						}
					}
					void* funcptr = __W3_Get_Event(w3, "ftpresp");
					if(funcptr != NULL) {
						if(buffer[strlen(buffer) - 1] == '\n') buffer[strlen(buffer) - 1] = 0;
						void (*func)(struct W3*, int, char*) = (void (*)(struct W3*, int, char*))funcptr;
						func(w3, status, buffer);
					}
					free(buffer);
					buffer = malloc(1);
					buffer[0] = 0;
				}
				status = 0;
			}
		}
	}
ftp_stop:;
	free(cbuf);
	free(buffer);
	free(buf);
}

void W3_FTP_Set_Username(struct W3* w3, const char* username) { __W3_Add_Prop(w3, "FTP_USERNAME", username); }

void W3_FTP_Set_Password(struct W3* w3, const char* password) { __W3_Add_Prop(w3, "FTP_PASSWORD", password); }

void W3_FTP_Send_Request(struct W3* w3) {
	if(strcasecmp(w3->method, "LIST") == 0) {
		__W3_FTP_Start_Passive(w3);
		*((int*)w3->generic) |= 1;
	} else if(strcasecmp(w3->method, "CWD") == 0) {
		__W3_Auto_Write(w3, "CWD ", 4);
		__W3_Auto_Write(w3, w3->path, strlen(w3->path));
		__W3_Auto_Write(w3, "\r\n", 2);
	}
}

void W3_FTP_Disconnect(struct W3* w3) { __W3_Auto_Write(w3, "QUIT\r\n", 6); }
