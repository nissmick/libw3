/* $Id$ */
#include "W3POP3.h"

#include "W3Core.h"
#include "W3Util.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

extern int strcasecmp(const char* s1, const char* s2);

void __W3_POP3_Request(struct W3* w3) {
	if(__W3_Get_Prop(w3, "POP3_USERNAME") == NULL || __W3_Get_Prop(w3, "POP3_PASSWORD") == NULL) {
		__W3_Debug("LibW3-POP3", "Set the username/password");
		void* funcptr = __W3_Get_Event(w3, "error");
		if(funcptr != NULL) {
			void (*func)(struct W3*, const char*) = (void (*)(struct W3*, const char*))funcptr;
			func(w3, "did-not-auth");
		}
		return;
	}
	char* buf = malloc(w3->readsize);
	int phase = 0;
	int login = 0;
	char* message = malloc(1);
	message[0] = 0;
	char* cbuf = malloc(2);
	cbuf[1] = 0;
	bool newl_cond = true;
	w3->generic = &newl_cond;
	while(true) {
		int len = __W3_Auto_Read(w3, buf, w3->readsize);
		if(len <= 0) break;
		int i;
		for(i = 0; i < len; i++) {
			char c = buf[i];
			if(c == '\r') continue;
			bool newl = false;
			if(c == '\n' && newl_cond) {
				newl = true;
			} else if(c == '.' && !newl_cond) {
				newl_cond = true;
			}
			if(newl) {
				newl_cond = true;
				if(login == 0) {
					char* str = __W3_Concat3("USER ", __W3_Get_Prop(w3, "POP3_USERNAME"), "\r\n");
					__W3_Auto_Write(w3, str, strlen(str));
					free(str);
					login = 1;
				} else if(login == 1) {
					char* str = __W3_Concat3("PASS ", __W3_Get_Prop(w3, "POP3_PASSWORD"), "\r\n");
					__W3_Auto_Write(w3, str, strlen(str));
					free(str);
					login = 2;
				} else {
					if(phase == 3) {
						/* OK */
						if(login == 2) {
							/* Login success */
							__W3_Debug("LibW3-POP3", "Login successful");
							login = 3;
							void* funcptr = __W3_Get_Event(w3, "pop3login");
							if(funcptr != NULL) {
								void (*func)(struct W3*, int) = (void (*)(struct W3*, int))funcptr;
								func(w3, 512);
							}
						} else {
							void* funcptr = __W3_Get_Event(w3, "pop3data");
							if(funcptr != NULL) {
								void (*func)(struct W3*, bool, char*) = (void (*)(struct W3*, bool, char*))funcptr;
								func(w3, true, message);
							}
						}
					} else if(phase == 4) {
						/* ERR */
						if(login == 2) {
							/* Login failed */
							__W3_Debug("LibW3-POP3", "Login failed");
							void* funcptr = __W3_Get_Event(w3, "error");
							if(funcptr != NULL) {
								void (*func)(struct W3*, const char*) = (void (*)(struct W3*, const char*))funcptr;
								func(w3, "login-fail");
							}
							login = 0;
							return;
						} else {
							void* funcptr = __W3_Get_Event(w3, "pop3data");
							if(funcptr != NULL) {
								void (*func)(struct W3*, bool, char*) = (void (*)(struct W3*, bool, char*))funcptr;
								func(w3, false, message);
							}
						}
					}
				}
				free(message);
				message = malloc(1);
				message[0] = 0;
				phase = 0;
				continue;
			} else if(phase == 3 || phase == 4) {
				cbuf[0] = buf[i];
				char* tmp = message;
				message = __W3_Concat(tmp, cbuf);
				free(tmp);
			}
			if(phase == 0) {
				if(c == '+') {
					phase = 1;
				} else if(c == '-') {
					phase = 2;
				}
			} else if(phase == 1 || phase == 2) {
				if(c == ' ') phase += 2;
			}
		}
	}
	free(message);
	free(cbuf);
	free(buf);
}

void W3_POP3_Set_Username(struct W3* w3, const char* username) { __W3_Add_Prop(w3, "POP3_USERNAME", username); }

void W3_POP3_Set_Password(struct W3* w3, const char* password) { __W3_Add_Prop(w3, "POP3_PASSWORD", password); }

void W3_POP3_Send_Request(struct W3* w3) {
	if(strcasecmp(w3->method, "LIST") == 0) {
		__W3_Auto_Write(w3, "LIST ", 5);
		if(w3->path != NULL && strlen(w3->path) != 0) {
			__W3_Auto_Write(w3, w3->path, strlen(w3->path));
		} else {
			*((bool*)w3->generic) = false;
		}
		__W3_Auto_Write(w3, "\r\n", 2);
	} else if(strcasecmp(w3->method, "RETR") == 0) {
		__W3_Auto_Write(w3, "RETR ", 5);
		if(w3->path != NULL && strlen(w3->path) != 0) {
			__W3_Auto_Write(w3, w3->path, strlen(w3->path));
			*((bool*)w3->generic) = false;
		}
		__W3_Auto_Write(w3, "\r\n", 2);
	}
}

void W3_POP3_Disconnect(struct W3* w3) { __W3_Auto_Write(w3, "QUIT\r\n", 6); }
