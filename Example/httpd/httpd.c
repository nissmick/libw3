/*
 * $Id$
 *
 * HTTP Daemon.
 */

#include <W3Core.h>
#include <W3HTTP.h>
#include <W3Util.h>

#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <signal.h>

char* root = NULL;

#ifndef __MINGW32__
extern int strcasecmp(const char* s1, const char* s2);
#endif

#define BUFFER_SIZE 512

char* badreq;
char* badreq_header;
char* notfound;
char* notfound_header;
char** types = NULL;
int ntypes = 0;

void add_type(char* key, char* value) {
	if(types == NULL) {
		types = malloc(sizeof(*types) * 2);
		types[0] = __W3_Strdup(key);
		types[1] = __W3_Strdup(value);
		ntypes = 2;
	} else {
		char** oldtypes = types;
		int i;
		types = malloc(sizeof(*types) * (ntypes + 2));
		for(i = 0; i < ntypes; i++) {
			types[i] = oldtypes[i];
		}
		types[ntypes] = __W3_Strdup(key);
		types[ntypes + 1] = __W3_Strdup(value);
		ntypes += 2;
		free(oldtypes);
	}
}

void http_handler(int sock) {
	__W3_Debug("HTTPd", "Got a connection");
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
	while(true) {
		int len = recv(sock, buf, BUFFER_SIZE, 0);
		if(len <= 0) break;
		int i;
		for(i = 0; i < len; i++) {
			if(phase == 0) {
				if(buf[i] == ' ') {
					phase++;
				} else {
					cbuf[0] = buf[i];
					char* tmp = method;
					method = __W3_Concat(tmp, cbuf);
					free(tmp);
				}
			} else if(phase == 1) {
				if(buf[i] == ' ') {
					phase++;
				} else {
					cbuf[0] = buf[i];
					char* tmp = path;
					path = __W3_Concat(tmp, cbuf);
					free(tmp);
				}
			} else if(phase == 2) {
				if(buf[i] == '\n') {
					phase++;
					if(strlen(version) < 8) {
						send(sock, badreq_header, strlen(badreq_header), 0);
						send(sock, badreq, strlen(badreq), 0);
						goto quit;
					} else {
						if(strcmp(version + 5, "1.1") == 0 || strcmp(version + 5, "1.0") == 0) {
						} else {
							send(sock, badreq_header, strlen(badreq_header), 0);
							send(sock, badreq, strlen(badreq), 0);
							goto quit;
						}
					}
				} else if(buf[i] != '\r') {
					cbuf[0] = buf[i];
					char* tmp = version;
					version = __W3_Concat(tmp, cbuf);
					free(tmp);
				}
			} else if(phase == 3) {
				if(buf[i] == '\n') {
					if(strcmp(line, "") == 0) phase++;
					free(line);
					line = malloc(1);
					line[0] = 0;
					if(phase == 4) {
						if(strcasecmp(method, "GET") == 0 || strcasecmp(method, "HEAD") == 0) goto response;
					}
				} else if(buf[i] != '\r') {
					cbuf[0] = buf[i];
					char* tmp = line;
					line = __W3_Concat(tmp, cbuf);
					free(tmp);
				}
			} else if(phase == 4) {
			}
		}
	}
	goto quit;
response : {
	/* Using goto is not a good idea! but hey, it works. */
	char* realpath = __W3_Concat3(root, "/", path);

	struct stat s;
	if(stat(realpath, &s) == 0) {
		if(S_ISDIR(s.st_mode)) {
			if(path[strlen(path) - 1] == '/') {
				struct stat old_s = s;
				char* index = __W3_Concat(path, "index.html");
				if(stat(index, &s)){
					send(sock, "HTTP/1.1 200 OK\r\n", 17, 0);
					send(sock, "Connection: close\r\n", 19, 0);
					char* length = malloc(1025);
					sprintf(length, "%d", s.st_size);
					send(sock, "Content-Type: ", 14, 0);
					if(types == NULL) {
						send(sock, "application/octet-stream", 24, 0);
					} else {
						int i;
						char* fpath = __W3_Strdup(index);
						bool found = false;
						for(i = strlen(fpath) - 1; i >= 0; i--) {
							if(fpath[i] == '.') {
								fpath[i] = 0;
								found = true;
								break;
							}
						}
						if(found) {
							found = false;
							int start = i + 1;
							for(i = 0; i < ntypes; i += 2) {
								if(strcasecmp(types[i], fpath + start) == 0) {
									found = true;
									send(sock, types[i + 1], strlen(types[i + 1]), 0);
									break;
								}
							}
							if(!found) {
								send(sock, "application/octet-stream", 24, 0);
							}
						} else {
							send(sock, "application/octet-stream", 24, 0);
						}
						free(fpath);
					}
					send(sock, "\r\n", 2, 0);
					send(sock, "Content-Length: ", 16, 0);
					send(sock, length, strlen(length), 0);
					free(length);
					send(sock, "\r\n", 2, 0);
					send(sock, "Server: ", 8, 0);
					send(sock, "LibW3-HTTPd (LibW3/", 19, 0);
					send(sock, LIBW3_VERSION, strlen(LIBW3_VERSION), 0);
					send(sock, ")", 1, 0);
					send(sock, "\r\n", 2, 0);
					send(sock, "\r\n", 2, 0);
					if(strcasecmp(method, "HEAD") != 0) {
						char* realpath_index = __W3_Concat3(root, "/", index);
						FILE* f = fopen(realpath_index, "r");
						free(realpath_index);
						char* buf = malloc(BUFFER_SIZE);
						while(true) {
							int len = fread(buf, 1, BUFFER_SIZE, f);
							if(len <= 0) break;
							send(sock, buf, len, 0);
						}
						fclose(f);
						free(buf);
					}
					free(index);
					goto out;
				}
				s = old_s;
				free(index);

				char* html = malloc(1);
				html[0] = 0;
				char* tmp;

				tmp = html;
				html = __W3_Concat3(tmp, "<html><head><meta charset=\"UTF-8\"><title>Index of ", path);
				free(tmp);

				tmp = html;
				html = __W3_Concat3(tmp, "</title></head><body><h1>Index of ", path);
				free(tmp);

				tmp = html;
				html = __W3_Concat(tmp, "</h1><hr><table width=\"100%\"><th width=\"20%\">Name</th><th>Size</th>");
				free(tmp);

				struct dirent** namelist;
				int n = scandir(realpath, &namelist, NULL, alphasort);

				if(n >= 0) {
					int i;
					bool dir = true;
				rep:;
					for(i = 0; i < n; i++) {
						char* fpath = __W3_Concat3(realpath, "/", namelist[i]->d_name);
						struct stat s2;
						if(stat(fpath, &s2) == 0) {
							if((dir ? S_ISDIR(s2.st_mode) : !S_ISDIR(s2.st_mode)) && strcmp(namelist[i]->d_name, ".") != 0) {
								tmp = html;
								html = __W3_Concat3(tmp, "<tr><td><a href=\"", namelist[i]->d_name);
								free(tmp);

								tmp = html;
								html = __W3_Concat(tmp, "\">");
								free(tmp);

								tmp = html;
								html = __W3_Concat(tmp, namelist[i]->d_name);
								free(tmp);

								tmp = html;
								html = __W3_Concat3(tmp, dir ? "/" : "", "</a></td>");
								free(tmp);

								char* bytes = malloc(2049);
								char* suffix = __W3_Strdup(" B");

								double bsize = s2.st_size;

								if(S_ISDIR(s2.st_mode)) {
									sprintf(bytes, "&lt;Directory&gt;");
									free(suffix);
									suffix = __W3_Strdup("");
								} else {
									if(s2.st_size > 1024 * 1024 * 1024) {
										free(suffix);
										suffix = __W3_Strdup(" GB");
										bsize /= 1024 * 1024 * 1024;
									} else if(s2.st_size > 1024 * 1024) {
										free(suffix);
										suffix = __W3_Strdup(" MB");
										bsize /= 1024 * 1024;
									} else if(s2.st_size > 1024) {
										free(suffix);
										suffix = __W3_Strdup(" KB");
										bsize /= 1024;
									}
									sprintf(bytes, "%.2f", bsize);
								}

								tmp = html;
								html = __W3_Concat3(tmp, "<td>", bytes);
								free(tmp);

								tmp = html;
								html = __W3_Concat3(tmp, suffix, "</td>");
								free(tmp);

								free(bytes);
								free(suffix);
							}
						}
						if(!dir) free(namelist[i]);
					}
					if(!dir) free(namelist);
					if(dir) {
						dir = false;
						goto rep;
					}
				}

				tmp = html;
				html = __W3_Concat(tmp, "</table><hr><i>LibW3-HTTPd (LibW3/" LIBW3_VERSION ")</i></body></html>");
				free(tmp);

				send(sock, "HTTP/1.1 200 OK\r\n", 17, 0);
				send(sock, "Connection: close\r\n", 19, 0);
				send(sock, "Server: ", 8, 0);
				send(sock, "LibW3-HTTPd (LibW3/", 19, 0);
				send(sock, LIBW3_VERSION, strlen(LIBW3_VERSION), 0);
				send(sock, ")", 1, 0);
				send(sock, "\r\n", 2, 0);
				send(sock, "Content-Type: text/html\r\n", 25, 0);
				char* length = malloc(1025);
				sprintf(length, "%d", strlen(html));
				send(sock, "Content-Length: ", 16, 0);
				send(sock, length, strlen(length), 0);
				send(sock, "\r\n", 2, 0);
				send(sock, "\r\n", 2, 0);
				if(strcasecmp(method, "HEAD") != 0) send(sock, html, strlen(html), 0);
				free(html);
out:;
			} else {
				send(sock, "HTTP/1.1 308 Permanent Redirect\r\n", 33, 0);
				send(sock, "Connection: close\r\n", 19, 0);
				send(sock, "Server: ", 8, 0);
				send(sock, "LibW3-HTTPd (LibW3/", 19, 0);
				send(sock, LIBW3_VERSION, strlen(LIBW3_VERSION), 0);
				send(sock, ")", 1, 0);
				send(sock, "\r\n", 2, 0);
				send(sock, "Location: ", 10, 0);
				send(sock, path, strlen(path), 0);
				send(sock, "/", 1, 0);
				send(sock, "\r\n", 2, 0);
				send(sock, "\r\n", 2, 0);
			}
		} else {
			send(sock, "HTTP/1.1 200 OK\r\n", 17, 0);
			send(sock, "Connection: close\r\n", 19, 0);
			char* length = malloc(1025);
			sprintf(length, "%d", s.st_size);
			send(sock, "Content-Type: ", 14, 0);
			if(types == NULL) {
				send(sock, "application/octet-stream", 24, 0);
			} else {
				int i;
				char* fpath = __W3_Strdup(path);
				bool found = false;
				for(i = strlen(fpath) - 1; i >= 0; i--) {
					if(fpath[i] == '.') {
						fpath[i] = 0;
						found = true;
						break;
					}
				}
				if(found) {
					found = false;
					int start = i + 1;
					for(i = 0; i < ntypes; i += 2) {
						if(strcasecmp(types[i], fpath + start) == 0) {
							found = true;
							send(sock, types[i + 1], strlen(types[i + 1]), 0);
							break;
						}
					}
					if(!found) {
						send(sock, "application/octet-stream", 24, 0);
					}
				} else {
					send(sock, "application/octet-stream", 24, 0);
				}
				free(fpath);
			}
			send(sock, "\r\n", 2, 0);
			send(sock, "Content-Length: ", 16, 0);
			send(sock, length, strlen(length), 0);
			free(length);
			send(sock, "\r\n", 2, 0);
			send(sock, "Server: ", 8, 0);
			send(sock, "LibW3-HTTPd (LibW3/", 19, 0);
			send(sock, LIBW3_VERSION, strlen(LIBW3_VERSION), 0);
			send(sock, ")", 1, 0);
			send(sock, "\r\n", 2, 0);
			send(sock, "\r\n", 2, 0);
			if(strcasecmp(method, "HEAD") != 0) {
				FILE* f = fopen(realpath, "r");
				char* buf = malloc(BUFFER_SIZE);
				while(true) {
					int len = fread(buf, 1, BUFFER_SIZE, f);
					if(len <= 0) break;
					send(sock, buf, len, 0);
				}
				fclose(f);
				free(buf);
			}
		}
	} else {
		send(sock, notfound_header, strlen(notfound_header), 0);
		send(sock, notfound, strlen(notfound), 0);
	}

	free(realpath);
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
	for(i = 1; argv[i] != NULL; i++) {
		if(strcmp(argv[i], "--version") == 0) {
			printf("LibW3 %s\n", LIBW3_VERSION);
			return 0;
		} else if(strcmp(argv[i], "--config") == 0 || strcmp(argv[i], "-c") == 0) {
			configfile = argv[i + 1];
		} else if(strcmp(argv[i], "--port") == 0 || strcmp(argv[i], "-p") == 0) {
			portstr = argv[i + 1];
		} else if(argv[i][0] == '-') {
			fprintf(stderr, "%s: invalid option: %s\n", argv[0], argv[i]);
			return 1;
		}
	}

	badreq = __W3_Strdup("<html>"
			     "	<head>"
			     "		<meta charset=\"UTF-8\">"
			     "	</head>"
			     "	<body>"
			     "		<h1>Bad request</h1>"
			     "		<hr>"
			     "		<i>LibW3-HTTPd (LibW3/" LIBW3_VERSION ")</i>"
			     "	</body>"
			     "</html>");
	badreq_header = malloc(2048);
	sprintf(badreq_header, "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: %d\r\nServer: LibW3-HTTPd (LibW3/" LIBW3_VERSION ")\r\nConnection: close\r\n\r\n", strlen(badreq));

	notfound = __W3_Strdup("<html>"
			       "	<head>"
			       "		<meta charset=\"UTF-8\">"
			       "	</head>"
			       "	<body>"
			       "		<h1>Not Found</h1>"
			       "		<hr>"
			       "		<i>LibW3-HTTPd (LibW3/" LIBW3_VERSION ")</i>"
			       "	</body>"
			       "</html>");
	notfound_header = malloc(2048);
	sprintf(notfound_header, "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: %d\r\nServer: LibW3-HTTPd (LibW3/" LIBW3_VERSION ")\r\nConnection: close\r\n\r\n", strlen(notfound));

	FILE* f = fopen(configfile, "r");
	if(f != NULL) {
		struct stat s;
		stat(configfile, &s);
		char* buf = malloc(s.st_size + 1);
		buf[s.st_size] = 0;
		fread(buf, s.st_size, 1, f);

		/* TODO: Parse the config file */
		char* line = malloc(1);
		line[0] = 0;
		char* cbuf = malloc(2);
		cbuf[1] = 0;
		int linenum = 0;
		int err = 0;
		for(i = 0;; i++) {
			if(buf[i] == '\n' || buf[i] == 0) {
				linenum++;
				if(line[0] != '#' && strcmp(line, "") != 0) {
					int j;
					bool hasparam = false;
					for(j = 0; line[j] != 0; j++) {
						if(line[j] == ' ') {
							line[j] = 0;
							hasparam = true;
							break;
						}
					}
					if(strcasecmp(line, "Root") == 0) {
						if(!hasparam) {
							fprintf(stderr, "%s: config line %d, directive needs a parameter\n", argv[0], linenum);
							err++;
						}
						if(root != NULL) free(root);
						root = __W3_Strdup(line + j + 1);
					}else if(strcasecmp(line, "Port") == 0) {
						if(!hasparam) {
							fprintf(stderr, "%s: config line %d, directive needs a parameter\n", argv[0], linenum);
							err++;
						}
						if(portstr != NULL) free(portstr);
						portstr = __W3_Strdup(line + j + 1);
					} else if(strcasecmp(line, "MIME") == 0) {
						if(!hasparam) {
							fprintf(stderr, "%s: config line %d, directive needs a parameter\n", argv[0], linenum);
							err++;
						}
						int k;
						char* param = line + j + 1;
						for(k = 0; param[k] != 0; k++) {
							if(param[k] == ' ') {
								param[k] = 0;
								add_type(param, param + k + 1);
								break;
							}
						}
					} else {
						fprintf(stderr, "%s: config line %d, unknown directive\n", argv[0], linenum);
						err++;
					}
				}
				free(line);
				line = malloc(1);
				line[0] = 0;
				if(buf[i] == 0) break;
			} else if(buf[i] != '\r') {
				/* Rejecting \r is a good idea for the time when I add the Windows support. */
				cbuf[0] = buf[i];
				char* tmp = line;
				line = __W3_Concat(tmp, cbuf);
				free(tmp);
			}
		}
		free(cbuf);
		free(line);

		free(buf);
		fclose(f);
		if(root == NULL) {
			fprintf(stderr, "%s: config has no root directive\n", argv[0]);
			err++;
		}
		if(err > 0) {
			fprintf(stderr, "%s: config has %d errors\n", argv[0], err);
			free(badreq_header);
			free(badreq);
			return 1;
		}
	} else {
		free(badreq_header);
		free(badreq);
		fprintf(stderr, "%s: failed to load the config file\n", argv[0]);
		return 1;
	}

	W3_Library_Init();
	int exitcode = 0;
	int port = atoi(portstr);
	int server_socket;
	if((server_socket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP)) < 0){
		return -1;goto exitnow;
	}
	struct sockaddr_in6 server_address;
	if((server_socket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP)) < 0){
		return -1;goto exitnow;
	}
	int yes = 1;
	if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0){
		close(server_socket);
		return -1;goto exitnow;
	}
	if(setsockopt(server_socket, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes)) < 0){
		close(server_socket);
		return 1;goto exitnow;
	}
	int no = 0;
	if(setsockopt(server_socket, IPPROTO_IPV6, IPV6_V6ONLY, &no, sizeof(no)) < 0){
		close(server_socket);
		return 1;goto exitnow;
	}
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin6_family = AF_INET6;
	server_address.sin6_addr = in6addr_any;
	server_address.sin6_port = htons(port);
	if(bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0){
		close(server_socket);
		return -1;goto exitnow;
	}
	if(listen(server_socket, 128) < 0){
		close(server_socket);
		return -1;goto exitnow;
	}
	signal(SIGCHLD, SIG_IGN);
	__W3_Debug("HTTPd", "Ready");
	while(1){
		struct sockaddr_in claddr;
		int clen = sizeof(claddr);
		int sock = accept(server_socket, (struct sockaddr*)&claddr, &clen);
		pid_t p = fork();
		if(p == 0){
			http_handler(sock);
			_exit(-1);
		}else{
			close(sock);
		}
	}
exitnow:
	if(exitcode != 0) {
		fprintf(stderr, "%s: failed to start the server\n", argv[0]);
		return 1;
	}
	return exitcode;
}
