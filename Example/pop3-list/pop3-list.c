/*
 * $Id$
 *
 * Lists the POP3 mails
 */

#include <W3Core.h>
#include <W3POP3.h>

#include <W3URL.h>
#include <W3Util.h> /* It has some useful functions, you know */

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void list_handler(struct W3* w3, bool ok, char* data) {
	printf("[%s] %s\n", ok ? "  OK  " : " FAIL ", data);
	W3_POP3_Disconnect(w3);
}

void login_handler(struct W3* w3){
	printf("Logged in\n");
	W3_Set_Method(w3, "LIST");
	W3_POP3_Send_Request(w3);
}

int main(int argc, char** argv) {
	if(argc < 4) {
		return 1;
	}
	W3_Library_Init();
	struct W3* w3 = W3_Create("pop3", argv[1], 110);
	W3_POP3_Set_Username(w3, argv[2]);
	W3_POP3_Set_Password(w3, argv[3]);
	W3_On(w3, "login", login_handler);
	W3_On(w3, "pop3data", list_handler);
	W3_Send_Request(w3);
}
