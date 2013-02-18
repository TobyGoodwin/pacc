#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* XXX we include parse.h for PACC_TYPE_FORMAT but is that the best way
 * to do it? */

#include "parse.h"
#include "parsefeed.h"

int main(int argc, char **argv) {
    char *text = 0;
    int len = 0, n = 1, parsed;
    struct pacc_parser *p0 = pacc_new();
    struct pacc_parser *p1 = pacc_feed_new();

    while (n < argc) {
	len += strlen(argv[n]) + 1;
	text = realloc(text, len + 1);
	if (!text) exit(1);
	if (n == 1) *text = '\0';
	strcat(text, argv[n]);
	strcat(text, "\n");
    //fprintf(stderr, "text is now >%s<\n", text);

	pacc_input(p0, text, len);
	parsed = pacc_parse(p0);
	if (parsed) {
	    printf("parsed with value " PACC_TYPE_FORMAT "\n", pacc_result(p0));
	    return 0;
	} else {
	    pacc_feed_input(p1, text, len);
	    parsed = pacc_feed_parse(p1);

	    if (!parsed) {
		char *e = pacc_feed_error(p1);
		fprintf(stderr, "arg:%s\n", e);
		free(e);
		return 1;
	    }
	    ++n;
	    //fprintf(stderr, "about to feed...\n");
	}
    }
    fprintf(stderr, "unexpected end of input\n");
    return 0;
}
