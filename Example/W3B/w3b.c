/*
 * $Id$
 *
 * Simple browser
 */

#include <W3Core.h>
#include <W3Util.h> /* It has some useful functions, you know */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

int main(int argc, char** argv) {
	int i;
	char* url = NULL;
	for(i = 1; i < argc; i++){
		if(strcmp(argv[i], "--version") == 0){
			printf("LibW3 %s\n", LIBW3_VERSION);
			return 0;
		}else if(argv[i][0] == '-'){
			fprintf(stderr, "%s: unknown option: %s\n", argv[0], argv[i]);
			return 1;
		}else{
			if(url != NULL){
				free(url);
				fprintf(stderr, "%s: garbage argument found\n", argv[0]);
				return 1;
			}
			url = __W3_Strdup(argv[i]);
		}
	}
	int phase = url == NULL ? 0 : 1;
	char c = 0;
	while(true){	/* Loop */
		if(phase == 0){
			if(c != '\n' && c != '\r'){
				printf("(O)pen, (Q)uit? ");
				fflush(stdout);
			}
			if(scanf("%c", &c) < 0) break;
			switch(tolower(c)){
				case 'q':
					goto exitnow;
				case 'o':
					printf("URL: ");
					fflush(stdout);
					char* url = malloc(2049);
					scanf("%s", url);
					free(url);
					break;
				case '\n':
				case '\r':
					break;
				default:
					printf("What do you mean?\n");
					break;
			}
		}
	}
	printf("\n");
exitnow:;
}
