/*
 * $Id$
 *
 * Simple browser
 */

#include <W3Core.h>
#include <W3HTTP.h>
#include <W3Tag.h>

#include <W3URL.h>
#include <W3Util.h> /* It has some useful functions, you know */

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef __MINGW32__
#include <windows.h>
HANDLE winstdout;
#else
#include <sys/ioctl.h>
#endif

int termw, termh;

extern int strcasecmp(const char* s1, const char* s2);

void clear_console() {
#ifdef __MINGW32__
	DWORD written = 0;
	const char* seq = "\x1b[2J\x1b[1;1H";
	WriteConsole(winstdout, seq, strlen(seq), &written, NULL);
#else
	printf("\x1b[2J\x1b[1;1H");
	fflush(stdout);
#endif
}

void get_terminal_size(int* width, int* height) {
#ifdef __MINGW32__
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(winstdout, &csbi);
	*width = csbi.dwSize.X;
	*height = csbi.dwSize.Y;
#else
	struct winsize ws;
	ioctl(0, TIOCGWINSZ, &ws);
	*width = ws.ws_col;
	*height = ws.ws_row;
#endif
}

char* databuf;
int datalen;
bool isredir;

void status_handler(struct W3* w3, int st) {
	if(st >= 300 && st < 400) {
		isredir = true;
	} else {
		isredir = false;
	}
}

void data_handler(struct W3* w3, char* data, size_t size) {
	if(isredir) return;
	if(databuf == NULL) {
		databuf = malloc(size);
		datalen = size;
		memcpy(databuf, data, size);
	} else {
		char* oldbuf = databuf;
		databuf = malloc(datalen + size);
		memcpy(databuf, oldbuf, datalen);
		memcpy(databuf + datalen, data, size);
		datalen += size;
		free(oldbuf);
	}
}

void access_site(const char* url) {
	struct W3URL* u = W3_Parse_URL(url);
	if(u != NULL) {
		struct W3* w3 = W3_Create(u->protocol, u->host, u->port);
		if(w3 != NULL) {
			if(databuf != NULL) {
				free(databuf);
			}
			databuf = NULL;
			datalen = 0;
			W3_Set_Method(w3, "GET");
			W3_Set_Path(w3, u->path);
			W3_HTTP_Enable_Redirect(w3);
			W3_On(w3, "status", (void*)status_handler);
			W3_On(w3, "data", (void*)data_handler);
			W3_Send_Request(w3);
			W3_Free(w3);
			printf("%d bytes\n", datalen);
		} else {
			fprintf(stderr, "Failed to connect\n");
		}
		W3_Free_URL(u);
	} else {
		fprintf(stderr, "Failed to parse\n");
	}
}

bool title;
bool italic;
bool bold;
bool pre;
char* titlebuf = NULL;

int nl;
int start = 0;

void html_handler(char* tagname, char* attr) {
	if(nl - start > termh - 1) return;
	if(strcasecmp(tagname, "title") == 0) {
		title = true;
	} else if(strcasecmp(tagname, "/title") == 0) {
		title = false;
	} else if(strcasecmp(tagname, "i") == 0) {
		italic = true;
	} else if(strcasecmp(tagname, "/i") == 0) {
		italic = false;
	} else if(strcasecmp(tagname, "pre") == 0) {
		pre = true;
		if(nl >= start) printf("\n");
		nl++;
	} else if(strcasecmp(tagname, "/pre") == 0) {
		pre = false;
		if(nl >= start) printf("\n");
		nl++;
	} else if(strcasecmp(tagname, "br") == 0) {
		if(nl >= start) printf("\n");
		nl++;
	} else if(strcasecmp(tagname, "h1") == 0) {
		if(nl >= start) printf("\n");
		nl++;
	} else if(strcasecmp(tagname, "/h1") == 0) {
		if(nl >= start)
			if(nl >= start) printf("\n");
		nl++;
	} else if(strcasecmp(tagname, "h2") == 0) {
		if(nl >= start) printf("\n");
		nl++;
	} else if(strcasecmp(tagname, "/h2") == 0) {
		if(nl >= start) printf("\n");
		nl++;
	} else if(strcasecmp(tagname, "h3") == 0) {
		if(nl >= start) printf("\n");
		nl++;
	} else if(strcasecmp(tagname, "/h3") == 0) {
		if(nl >= start) printf("\n");
		nl++;
	} else if(strcasecmp(tagname, "h4") == 0) {
		if(nl >= start) printf("\n");
		nl++;
	} else if(strcasecmp(tagname, "/h4") == 0) {
		if(nl >= start) printf("\n");
		nl++;
	} else if(strcasecmp(tagname, "h5") == 0) {
		if(nl >= start) printf("\n");
		nl++;
	} else if(strcasecmp(tagname, "/h5") == 0) {
		if(nl >= start) printf("\n");
		nl++;
	} else if(strcasecmp(tagname, "h6") == 0) {
		if(nl >= start) printf("\n");
		nl++;
	} else if(strcasecmp(tagname, "/h6") == 0) {
		if(nl >= start) printf("\n");
		nl++;
	} else if(strcasecmp(tagname, "img") == 0) {
		if(nl >= start) printf("[IMG]");
	} else if(strcasecmp(tagname, "hr") == 0) {
		int i;
		if(nl >= start) printf("\n ");
		nl++;
		for(i = 0; i < termw - 2; i++) {
			if(nl >= start) printf("-");
		}
		if(nl >= start) printf("\n");
		nl++;
	}
	fflush(stdout);
}

void text_handler(char* data) {
	if(title) {
		if(titlebuf == NULL) {
			titlebuf = malloc(1);
			titlebuf[0] = 0;
		}
		char* tmp = titlebuf;
		titlebuf = __W3_Concat(tmp, data);
		free(tmp);
	} else {
		if(nl - start > termh - 1) return;
		char* text = malloc(strlen(data) + 64);
		int i;
		char* fmt_data = malloc(strlen(data) * 2);
		if(pre) {
			free(fmt_data);
			fmt_data = __W3_Strdup(data);
			for(i = 0; i < strlen(data); i++) {
				if(data[i] == '\n') {
					nl++;
				}
			}
		} else {
			fmt_data[0] = 0;
			char* cbuf = malloc(2);
			cbuf[1] = 0;
			bool spc = false;
			bool firstspc = true;
			for(i = 0; i < strlen(data); i++) {
				cbuf[0] = data[i];
				if(data[i] == '\n' || data[i] == '\t') {
					if(!spc && !firstspc) {
						spc = true;
						char* tmp = fmt_data;
						fmt_data = __W3_Concat(tmp, " ");
						free(tmp);
					}
				} else {
					firstspc = false;
					spc = false;
					char* tmp = fmt_data;
					fmt_data = __W3_Concat(tmp, cbuf);
					free(tmp);
				}
			}
			free(cbuf);
		}
		sprintf(text, "%s%s%s\x1b[m", italic ? "\x1b[3m" : "", bold ? "\x1b[1m" : "", fmt_data);
		if(nl >= start) printf("%s", text);
		fflush(stdout);
		free(text);
		free(fmt_data);
	}
}

void render_site() {
	if(databuf != NULL) {
		if(titlebuf != NULL) {
			free(titlebuf);
		}
		title = false;
		italic = false;
		bold = false;
		pre = false;
		titlebuf = NULL;
		nl = 0;
		W3_Tag_Parse(databuf, datalen, html_handler, text_handler);
		char* seq = malloc(1024);
		sprintf(seq, "\x1b[1;%dH", termw - strlen(titlebuf != NULL ? titlebuf : "No title") - 1);
#ifdef __MINGW32__
		WriteConsole(winstdout, seq, strlen(seq), &written, NULL);
#else
		printf("%s\x1b[7m %s \x1b[m", seq, titlebuf != NULL ? titlebuf : "No title");
		fflush(stdout);
#endif
		free(seq);
	}
}

int main(int argc, char** argv) {
	int i;
	databuf = NULL;
	datalen = 0;
	char* url = NULL;
	for(i = 1; i < argc; i++) {
		if(strcmp(argv[i], "--version") == 0) {
			printf("LibW3 %s\n", LIBW3_VERSION);
			return 0;
		} else if(argv[i][0] == '-') {
			fprintf(stderr, "%s: unknown option: %s\n", argv[0], argv[i]);
			return 1;
		} else {
			if(url != NULL) {
				free(url);
				fprintf(stderr, "%s: garbage argument found\n", argv[0]);
				return 1;
			}
			url = __W3_Strdup(argv[i]);
		}
	}
	W3_Library_Init();
	int phase = 0;
	char c = 0;
	bool acc = false;
	if(url != NULL) {
		access_site(url);
		acc = true;
	}
#ifdef __MINGW32__
	winstdout = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD mode = 0;
	GetConsoleMode(winstdout, &mode);
	const DWORD origmode = mode;
	mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(winstdout, mode);
#endif
	bool rendered = false;
	clear_console();
	start = 0;
	get_terminal_size(&termw, &termh);
	while(true) { /* Loop */
		if(!rendered) {
			clear_console();
			render_site();
			rendered = true;
		}
		if(c != '\n' && c != '\r') {
			printf("\x1b[%d;1H(O)pen, (Q)uit", termh);
			printf("? ");
			fflush(stdout);
		}
		if(scanf("%c", &c) < 0) break;
		switch(tolower(c)) {
		case 'q':
			goto exitnow;
		case 'o':
			printf("URL: ");
			fflush(stdout);
			if(url != NULL) free(url);
			url = malloc(2049);
			scanf("%s", url);
			acc = false;
			break;
		case 'd':
			rendered = false;
			start++;
			break;
		case '\n':
		case '\r':
			break;
		default:
			printf("What do you mean?\n");
			break;
		}
		if(!acc && url != NULL) {
			start = 0;
			access_site(url);
			acc = true;
			rendered = false;
		}
	}
	printf("\n");
exitnow:;
#ifdef __MINGW32__
	SetConsoleMode(winstdout, origmode);
#endif
}
