/* $Id$
 *
 * Fetch a file using libw3.
 *
 */

#include <W3Core.h>

#include <stdio.h>
#include <stdbool.h>

int main(int argc, char** argv){
	if(argc < 2){
		fprintf(stderr, "Usage: %s URL\n", argv[0]);
		return 1;
	}
	W3_Library_Init();
	struct W3* w3 = W3_Create("http", argv[1], 80);
	W3_Set_Method(w3, "GET");
	W3_Set_Path(w3, "/sunhttpd");
	W3_Send_Request(w3);
	W3_Free(w3);
}
