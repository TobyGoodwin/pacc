#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse.h"

#define LINE 80
#define P1 "$ "
#define P2 "> "

int main(void) {
    char *text = 0;
    char *prompt = P1;
    char line[LINE + 1];
    int len, parsed;
    struct pacc_parser *p0 = pacc_new();
    struct pacc_parser *p1 = pacc_feed_new();

    text = malloc(1); if (!text) exit(1);
    len = 0; *text = '\0'; prompt = P1;
    for (;;) {
	printf("%s", prompt); fflush(stdout);
	if (!fgets(line, LINE, stdin)) break;
	len += strlen(line);
	text = realloc(text, len + 1);
	if (!text) exit(1);
	strcat(text, line);

	pacc_input(p0, text, len);
	parsed = pacc_parse(p0);
	if (parsed) {
	    printf("%d\n", pacc_result(p0));
	    len = 0; *text = '\0'; prompt = P1;
	} else {
	    pacc_feed_input(p1, text, len);
	    parsed = pacc_feed_parse(p1);
	    if (parsed) {
		prompt = P2;
	    } else {
		char *e = pacc_feed_error(p1);
		fprintf(stderr, "%s\n", e);
		free(e);
		len = 0; *text = '\0'; prompt = P1;
	    }
	}
    }
    if (len) fprintf(stderr, "unexpected end of input\n");
    else fprintf(stderr, "\n");
    return 0;
}
