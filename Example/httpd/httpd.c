/*
 * $Id$
 *
 * HTTP Daemon.
 */

#include <W3Core.h>
#include <W3HTTP.h>
#include <W3Util.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <ls_server.h>

#define BUFFER_SIZE 512

char* badreq;
char* badreq_header;

void http_handler(int sock){
	char* buf = malloc(BUFFER_SIZE);
	int phase = 0;
	/*	0: Method
		1: Path
		2: Version
		3: Header
		4: Body
	 */
	char* method = malloc(1);
	method[0] = 0;
	char* path = malloc(1);
	path[0] = 0;
	char* version = malloc(1);
	version[0] = 0;
	char* cbuf = malloc(2);
	cbuf[1] = 0;
	char* line = malloc(1);
	line[0] = 0;
	while(true){
		int len = recv(sock, buf, BUFFER_SIZE, 0);
		if(len <= 0) break;
		int i;
		for(i = 0; i < len; i++){
			if(phase == 0){
				if(buf[i] == ' '){
					phase++;
				}else{
					cbuf[0] = buf[i];
					char* tmp = method;
					method = __W3_Concat(tmp, cbuf);
					free(tmp);
				}
			}else if(phase == 1){
				if(buf[i] == ' '){
					phase++;
				}else{
					cbuf[0] = buf[i];
					char* tmp = path;
					path = __W3_Concat(tmp, cbuf);
					free(tmp);
				}
			}else if(phase == 2){
				if(buf[i] == '\n'){
					phase++;
					if(strlen(version) < 8){
						send(sock, badreq_header, strlen(badreq_header), 0);
						send(sock, badreq, strlen(badreq), 0);
						goto quit;
					}else{
						if(strcmp(version + 5, "1.1") == 0 || strcmp(version + 5, "1.0") == 0){
						}else{
							send(sock, badreq_header, strlen(badreq_header), 0);
							send(sock, badreq, strlen(badreq), 0);
							goto quit;
						}
					}
				}else if(buf[i] != '\r'){
					cbuf[0] = buf[i];
					char* tmp = version;
					version = __W3_Concat(tmp, cbuf);
					free(tmp);
				}
			}else if(phase == 3){
				if(buf[i] == '\n'){
					if(strcmp(line, "") == 0) phase++;
					free(line);
					line = malloc(1);
					line[0] = 0;
					if(phase == 4){
						if(strcmp(method, "GET") == 0) goto quit;
					}
				}else if(buf[i] != '\r'){
					cbuf[0] = buf[i];
					char* tmp = line;
					line = __W3_Concat(tmp, cbuf);
					free(tmp);
				}
			}else if(phase == 4){
			}
		}
	}
quit:;
	free(line);
	free(method);
	free(path);
	free(version);
	free(cbuf);
}

int main(int argc, char** argv) {
	int i;
	char* portstr = NULL;
	char* configfile = "/etc/httpd.conf";
	for(i = 1; argv[i] != NULL; i++){
		if(strcmp(argv[i], "--version") == 0) {
			printf("LibW3 %s\n", LIBW3_VERSION);
			return 0;
		}else if(strcmp(argv[i], "--config") == 0 || strcmp(argv[i], "-c") == 0) {
			configfile = argv[i + 1];
		}else if(strcmp(argv[i], "--port") == 0 || strcmp(argv[i], "-p") == 0) {
			portstr = argv[i + 1];
		}else if(argv[i][0] == '-'){
			fprintf(stderr, "%s: invalid option: %s\n", argv[0], argv[i]);
			return 1;
		}
	}
	badreq = __W3_Strdup(
"<html>" \
"	<head>" \
"		<meta charset=\"UTF-8\">" \
"	</head>" \
"	<body>" \
"		<h1>Bad request</h1>" \
"		<hr>" \
"		<i>LibW3/" LIBW3_VERSION "</i>" \
"	</body>" \
"</html>"
);
	badreq_header = malloc(2048);
	sprintf(badreq_header, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n", strlen(badreq));

	FILE* f = fopen(configfile, "r");
	if(f != NULL){
		struct stat s;
		stat(configfile, &s);
		char* file = malloc(s.st_size + 1);
		file[s.st_size] = 0;
		fread(file, s.st_size, 1, f);

		/* TODO: Parse the config file */

		free(file);
		fclose(f);
	}else{
		free(badreq_header);
		free(badreq);
		fprintf(stderr, "%s: failed to load the config file\n", argv[0]);
		return 1;
	}

	W3_Library_Init();
	int st = ls_start_server(portstr == NULL ? 80 : atoi(portstr), http_handler);
	if(st == -1){
		fprintf(stderr, "%s: failed to start the server\n", argv[0]);
		return 1;
	}
}
