/* $Id$ */
#include "W3POP3.h"

#include "W3Core.h"
#include "W3Util.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void __W3_POP3_Request(struct W3* w3) {
	if(__W3_Get_Prop(w3, "POP3_USERNAME") == NULL || __W3_Get_Prop(w3, "POP3_PASSWORD") == NULL){
		__W3_Debug("LibW3-POP3", "Set the username/password");
		void* funcptr = __W3_Get_Event(w3, "error");
		if(funcptr != NULL){
			void(*func)(struct W3*, const char*) = (void(*)(struct W3*, const char*))funcptr;
			func(w3, "did-not-auth");
		}
		return;
	}
	char* buf = malloc(w3->readsize);
	int phase = 0;
	int login = 0;
	while(true) {
		int len = __W3_Auto_Read(w3, buf, w3->readsize);
		if(len <= 0) break;
		int i;
		for(i = 0; i < len; i++){
			write(1, buf + i, 1);
			char c = buf[i];
			if(c == '\r') continue;
			if(c == '\n'){
				if(login == 0){
					char* str = __W3_Concat3("USER ", __W3_Get_Prop(w3, "POP3_USERNAME"), "\r\n");
					__W3_Auto_Write(w3, str, strlen(str));
					free(str);
					login = 1;
				}else if(login == 1){
					char* str = __W3_Concat3("PASS ", __W3_Get_Prop(w3, "POP3_PASSWORD"), "\r\n");
					__W3_Auto_Write(w3, str, strlen(str));
					free(str);
					login = 2;
				}else{
					if(phase == 3){
						/* OK */
						if(login == 2){
							/* Login success */
							login = 3;
							void* funcptr = __W3_Get_Event(w3, "login");
							if(funcptr != NULL){
								void(*func)(struct W3*, const char*) = (void(*)(struct W3*, int))funcptr;
								func(w3, 512);
							}
							__W3_Debug("LibW3-POP3", "Login successful");
						}else{
						}
					}else if(phase == 4){
						/* ERR */
						if(login == 2){
							/* Login failed */
							void* funcptr = __W3_Get_Event(w3, "error");
							if(funcptr != NULL){
								void(*func)(struct W3*, const char*) = (void(*)(struct W3*, const char*))funcptr;
								func(w3, "login-fail");
							}
							login = 0;
							return;
						}else{
						}
					}
				}
				phase = 0;
				continue;
			}
			if(phase == 0){
				if(c == '+'){
					phase = 1;
				}else if(c == '-'){
					phase = 2;
				}
			}else if(phase == 1){
				if(c == ' ') phase += 2;
			}
		}
	}
	free(buf);
}

void W3_POP3_Set_Username(struct W3* w3, const char* username){
	__W3_Add_Prop(w3, "POP3_USERNAME", username);
}


void W3_POP3_Set_Password(struct W3* w3, const char* password){
	__W3_Add_Prop(w3, "POP3_PASSWORD", password);
}
