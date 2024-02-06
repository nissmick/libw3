/* $Id$ */
#include "W3Tcl.h"

#include <tcl.h>

#include "W3Util.h"
#include "W3Core.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

extern int strcasecmp(const char* s1, const char* s2);

void** __dictionary;

int Tcl_W3Cmd(ClientData dummy, Tcl_Interp* interp, int objc, Tcl_Obj* CONST objv[]){
	if(objc < 2){
		const char* errmsg = "argument error: arguments should be \"w3 subcommand arg1 arg2 ...\"";
		Tcl_SetObjResult(interp, Tcl_NewStringObj(errmsg, strlen(errmsg)));
		return TCL_ERROR;
	}
	char* subcommand = Tcl_GetString(objv[1]);
	if(strcasecmp(subcommand, "create") == 0){
		if(objc != 6){
			const char* errmsg = "argument error: arguments should be \"w3 create name protocol hostname port\"";
			Tcl_SetObjResult(interp, Tcl_NewStringObj(errmsg, strlen(errmsg)));
			return TCL_ERROR;
		}
		char* name = Tcl_GetString(objv[2]);
		char* protocol = Tcl_GetString(objv[3]);
		char* hostname = Tcl_GetString(objv[4]);
		int port = atoi(Tcl_GetString(objv[5]));
		struct W3* w3 = W3_Create(protocol, hostname, port);
		if(w3 == NULL){
			const char* errmsg = "connect error: failed to connect";
			Tcl_SetObjResult(interp, Tcl_NewStringObj(errmsg, strlen(errmsg)));
			return TCL_ERROR;
		}else{
			int len = 0;
			if(__dictionary == NULL) {
				__dictionary = malloc(sizeof(*__dictionary) * (len + 3));
				__dictionary[len] = __W3_Strdup(name);
				__dictionary[len + 1] = w3;
				int i;
				for(i = 0; ((char*)__dictionary[len])[i] != 0; i++) {
					((char*)__dictionary[len])[i] = tolower(((char*)__dictionary[len])[i]);
				}
				__dictionary[len + 2] = NULL;
			} else {
				for(len = 0; __dictionary[len] != NULL; len++)
					;
				void** __dictionary2 = __dictionary;
				__dictionary = malloc(sizeof(*__dictionary) * (len + 3));
				for(len = 0; __dictionary2[len] != NULL; len++) {
					__dictionary[len] = __dictionary2[len];
				}
				__dictionary[len] = __W3_Strdup(name);
				__dictionary[len + 1] = w3;
				__dictionary[len + 2] = NULL;
				int i;
				for(i = 0; ((char*)__dictionary[len])[i] != 0; i++) {
					((char*)__dictionary[len])[i] = tolower(((char*)__dictionary[len])[i]);
				}
				free(__dictionary2);
			}
			Tcl_SetObjResult(interp, Tcl_NewIntObj(0));
		}
	}else if(strcasecmp(subcommand, "set_method") == 0 || strcasecmp(subcommand, "set_path") == 0){
		if(objc != 4){
			if(strcasecmp(subcommand, "set_method") == 0){
				const char* errmsg = "argument error: arguments should be \"w3 set_method name method\"";
				Tcl_SetObjResult(interp, Tcl_NewStringObj(errmsg, strlen(errmsg)));
			}else{
				const char* errmsg = "argument error: arguments should be \"w3 set_path name path\"";
				Tcl_SetObjResult(interp, Tcl_NewStringObj(errmsg, strlen(errmsg)));
			}
			return TCL_ERROR;
		}
		char* name = Tcl_GetString(objv[2]);
		if(__dictionary == NULL){
			const char* errmsg = "argument error: a client named that does not exist";
			Tcl_SetObjResult(interp, Tcl_NewStringObj(errmsg, strlen(errmsg)));
		}else{
			int i;
			for(i = 0; __dictionary[i] != NULL; i += 2){
				if(strcasecmp((char*)__dictionary[i], name) == 0){
					if(strcasecmp(subcommand, "set_method") == 0){
						W3_Set_Method(__dictionary[i + 1], Tcl_GetString(objv[3]));
					}else{
						W3_Set_Path(__dictionary[i + 1], Tcl_GetString(objv[3]));
					}
					Tcl_SetObjResult(interp, Tcl_NewIntObj(0));
					return TCL_OK;
				}
			}
			const char* errmsg = "argument error: a client named that does not exist";
			Tcl_SetObjResult(interp, Tcl_NewStringObj(errmsg, strlen(errmsg)));
		}
	}else if(strcasecmp(subcommand, "send_request") == 0){
		if(objc != 3){
			const char* errmsg = "argument error: arguments should be \"w3 send_request name\"";
			Tcl_SetObjResult(interp, Tcl_NewStringObj(errmsg, strlen(errmsg)));
			return TCL_ERROR;
		}
		char* name = Tcl_GetString(objv[2]);
		if(__dictionary == NULL){
			const char* errmsg = "argument error: a client named that does not exist";
			Tcl_SetObjResult(interp, Tcl_NewStringObj(errmsg, strlen(errmsg)));
		}else{
			int i;
			for(i = 0; __dictionary[i] != NULL; i += 2){
				if(strcasecmp((char*)__dictionary[i], name) == 0){
					W3_Send_Request(__dictionary[i + 1]);
					Tcl_SetObjResult(interp, Tcl_NewIntObj(0));
					return TCL_OK;
				}
			}
			const char* errmsg = "argument error: a client named that does not exist";
			Tcl_SetObjResult(interp, Tcl_NewStringObj(errmsg, strlen(errmsg)));
		}
	}else{
		const char* errmsg = "argument error: invalid subcommand";
		Tcl_SetObjResult(interp, Tcl_NewStringObj(errmsg, strlen(errmsg)));
		return TCL_ERROR;
	}
	return TCL_OK;
}

int W3_Init(Tcl_Interp* interp){
	__W3_Debug("Tcl", "Initializing the binding");
	W3_Library_Init();
	__dictionary = malloc(sizeof(*__dictionary));
	__dictionary[0] = NULL;
	Tcl_CreateObjCommand(interp, "w3", Tcl_W3Cmd, NULL, NULL);
	__W3_Debug("Tcl", "Initialized the binding");
	return TCL_OK;
}
