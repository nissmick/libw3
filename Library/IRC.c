/* $Id$ */
#include "W3IRC.h"

#include "W3Core.h"
#include "W3DNS.h"
#include "W3Util.h"

#include <stdlib.h>
#include <string.h>

#ifndef __MINGW32__
extern int strcasecmp(const char* s1, const char* s2);
#endif

void __W3_IRC_Request(struct W3* w3) {
	if(__W3_Get_Prop(w3, "IRC_USERNAME") == NULL || __W3_Get_Prop(w3, "IRC_NICKNAME") == NULL) {
		__W3_Debug("LibW3-IRC", "Set the username/nickname at least");
		void* funcptr = __W3_Get_Event(w3, "error");
		if(funcptr != NULL) {
			void (*func)(struct W3*, const char*) = (void (*)(struct W3*, const char*))funcptr;
			func(w3, "did-not-auth");
		}
		return;
	}
	if(__W3_Get_Prop(w3, "IRC_VERSION") == NULL) {
		__W3_Add_Prop(w3, "IRC_VERSION", "LibW3 " LIBW3_VERSION);
	}
	char* username = __W3_Get_Prop(w3, "IRC_USERNAME");
	char* nickname = __W3_Get_Prop(w3, "IRC_NICKNAME");
	char* realname = __W3_Get_Prop(w3, "IRC_REALNAME");
	char* servername = __W3_Get_Prop(w3, "IRC_SERVERNAME");
	char* hostname = __W3_Get_Prop(w3, "IRC_HOSTNAME");
	if(realname == NULL) realname = username;
	if(servername == NULL) servername = username;
	if(hostname == NULL) hostname = username;
	__W3_Auto_Write(w3, "USER ", 5);
	__W3_Auto_Write(w3, username, strlen(username));
	__W3_Auto_Write(w3, " ", 1);
	__W3_Auto_Write(w3, hostname, strlen(hostname));
	__W3_Auto_Write(w3, " ", 1);
	__W3_Auto_Write(w3, servername, strlen(servername));
	__W3_Auto_Write(w3, " ", 1);
	__W3_Auto_Write(w3, realname, strlen(realname));
	__W3_Auto_Write(w3, "\r\n", 2);
	__W3_Auto_Write(w3, "NICK ", 5);
	__W3_Auto_Write(w3, nickname, strlen(nickname));
	__W3_Auto_Write(w3, "\r\n", 2);
	if(__W3_Get_Prop(w3, "IRC_PASSWORD") != NULL) {
		__W3_Auto_Write(w3, "PASS :", 6);
		__W3_Auto_Write(w3, __W3_Get_Prop(w3, "IRC_PASSWORD"), strlen(__W3_Get_Prop(w3, "IRC_PASSWORD")));
		__W3_Auto_Write(w3, "\r\n", 2);
	}
	char* buf = malloc(w3->readsize);
	int phase = 0; /* 0: before the prefix
			* 1: prefix
			* 2: command
			* 3: parms
			*/
	/* <message>  ::= [':' <prefix> <SPACE> ] <command> <params> <crlf>
	 * <prefix>   ::= <servername> | <nick> [ '!' <user> ] [ '@' <host> ]
	 * <command>  ::= <letter> { <letter> } | <number> <number> <number>
	 * <SPACE>    ::= ' ' { ' ' }
	 * <params>   ::= <SPACE> [ ':' <trailing> | <middle> <params> ]
	 *
	 * <middle>   ::= <Any *non-empty* sequence of octets not including SPACE
	 *                or NUL or CR or LF, the first of which may not be ':'>
	 * <trailing> ::= <Any, possibly *empty*, sequence of octets not including
	 *                  NUL or CR or LF>
	 *
	 * <crlf>     ::= CR LF
	 *
	 * https://datatracker.ietf.org/doc/html/rfc1459#section-2.3.1
	 */
	char* prefix = malloc(1);
	prefix[0] = 0;
	char* command = malloc(1);
	command[0] = 0;
	char* params = malloc(1);
	params[0] = 0;
	char* cbuf = malloc(2);
	cbuf[1] = 0;
	bool colon = false;
	while(true) {
		int len = __W3_Auto_Read(w3, buf, w3->readsize);
		if(len <= 0) break;
		int i;
		for(i = 0; i < len; i++) {
			if(phase == 0) {
				if(buf[i] == ':') {
					phase = 1;
				} else {
					phase = 2;
					cbuf[0] = buf[i];
					char* tmp = command;
					command = __W3_Concat(tmp, cbuf);
					free(tmp);
				}
			} else if(phase == 1) {
				if(buf[i] == ' ') {
					phase = 2;
				} else {
					cbuf[0] = buf[i];
					char* tmp = prefix;
					prefix = __W3_Concat(tmp, cbuf);
					free(tmp);
				}
			} else if(phase == 2) {
				if(buf[i] == ' ') {
					phase = 3;
				} else {
					cbuf[0] = buf[i];
					char* tmp = command;
					command = __W3_Concat(tmp, cbuf);
					free(tmp);
				}
			} else if(phase == 3) {
				if(buf[i] == '\n') {
					phase = 0;

					/* Parse commands here */
					if(strcasecmp(command, "PRIVMSG") == 0) {
						int j;
						bool msgcont = false;
						char* username = NULL;
						char* content = NULL;
						for(j = 0; params[j] != 0; j++) {
							if(params[j] == ' ') {
								params[j] = 0;
								username = params;
								content = params + j + 1;
								break;
							}
						}
						if(username != NULL && content != NULL) {
							if(content[0] == 1 && content[strlen(content) - 1] == 1) {
								char* sentfrom = __W3_Strdup(prefix);
								int j;
								for(j = 0; sentfrom[j] != 0; j++) {
									if(sentfrom[j] == '!') {
										sentfrom[j] = 0;
										break;
									}
								}
								/* CTCP Request */
								if(strcasecmp(content, "\x01"
										       "VERSION"
										       "\x01") == 0) {
									__W3_Auto_Write(w3, "NOTICE ", 7);
									__W3_Auto_Write(w3, prefix, strlen(prefix));
									__W3_Auto_Write(w3, " :", 2);
									__W3_Auto_Write(w3, __W3_Get_Prop(w3, "IRC_VERSION"), strlen(__W3_Get_Prop(w3, "IRC_VERSION")));
									__W3_Auto_Write(w3, "\r\n", 2);
								} else {
									/* client is sending the non-standard CTCP ... */
								}
								free(sentfrom);
							}
							void* funcptr = __W3_Get_Event(w3, "message");
							if(funcptr != NULL) {
								void (*func)(struct W3 * w3, char* on, char* message) = (void (*)(struct W3 * w3, char* from, char* message)) funcptr;
								func(w3, username[0] == '#' ? username : prefix, content);
							}
						}
					} else if(strcasecmp(command, "PING") == 0) {
						__W3_Auto_Write(w3, "PONG ", 5);
						__W3_Auto_Write(w3, params, strlen(params));
						__W3_Auto_Write(w3, "\r\n", 2);
					} else {
						void* funcptr = __W3_Get_Event(w3, "unknown");
						if(funcptr != NULL) {
							void (*func)(struct W3 * w3, char* prefix, char* command, char* data) = (void (*)(struct W3 * w3, char* prefix, char* command, char* data)) funcptr;
							func(w3, prefix, command, params);
						}
					}

					prefix = malloc(1);
					prefix[0] = 0;
					command = malloc(1);
					command[0] = 0;
					params = malloc(1);
					params[0] = 0;
					cbuf = malloc(2);
					cbuf[1] = 0;
					colon = false;
				} else if(buf[i] != '\r') {
					cbuf[0] = buf[i];
					if(cbuf[0] != ':' || colon) {
						char* tmp = params;
						params = __W3_Concat(tmp, cbuf);
						free(tmp);
					} else if(cbuf[0] == ':') {
						colon = true;
					}
				}
			}
		}
	}
	free(cbuf);
	free(params);
	free(command);
	free(prefix);
	free(buf);
}

void W3_IRC_Set_Username(struct W3* w3, const char* username) { __W3_Add_Prop(w3, "IRC_USERNAME", username); }

void W3_IRC_Set_Hostname(struct W3* w3, const char* hostname) { __W3_Add_Prop(w3, "IRC_HOSTNAME", hostname); }

void W3_IRC_Set_Realname(struct W3* w3, const char* realname) { __W3_Add_Prop(w3, "IRC_REALNAME", realname); }

void W3_IRC_Set_Servername(struct W3* w3, const char* servername) { __W3_Add_Prop(w3, "IRC_SERVERNAME", servername); }

void W3_IRC_Set_Nickname(struct W3* w3, const char* nickname) { __W3_Add_Prop(w3, "IRC_NICKNAME", nickname); }

void W3_IRC_Set_Password(struct W3* w3, const char* password) { __W3_Add_Prop(w3, "IRC_PASSWORD", password); }

void W3_IRC_Disconnect(struct W3* w3) { __W3_Auto_Write(w3, "QUIT\r\n", 6); }

void W3_IRC_Send_Request(struct W3* w3) {
	__W3_Auto_Write(w3, w3->method, strlen(w3->method));
	__W3_Auto_Write(w3, " ", 1);
	__W3_Auto_Write(w3, w3->path, strlen(w3->path));
	__W3_Auto_Write(w3, "\r\n", 2);
}
