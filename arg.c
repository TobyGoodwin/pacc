#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arg.h"
#include "assert.h"
#include "error.h"

static const struct option long_opts[] = {
    { "dump-ast", required_argument, 0, 'd' },
    { "feed", required_argument, 0, 'f' },
    { "feed-rule", required_argument, 0, 'r' },
    { "help", no_argument, 0, '?' },
    { "output", required_argument, 0, 'o' },
    { 0, 0, 0, 0 }
};

/* Use "-d 0" to dump the AST before desugaring; 1 after desugaring, 2
 * after cooking. They can be combined, e.g. "-d02". */
static char *dump = "";
char *arg_dump(void) {
    return dump;
}

static char *input = 0;
char *arg_input(void) {
    assert(input);
    return input;
}

static char *feed = 0;
char *arg_feed(void) {
    return feed;
}

static char *feed_rule = "__";
char *arg_feed_rule(void) {
    return feed_rule;
}

static char *output = 0;
char *arg_output(void) {
    assert(output);
    return output;
}

static void usage(void) {
    fprintf(stderr, "Usage: pacc [OPTION]... FILE\n");
    exit(1);
}

/* The inventively named munge() returns a newly-allocated string
 * holding the basename of its input, with suffix replacing the existing
 * suffix (or appended if there was no suffix). For example,
 * munge("/foo/x.pacc") ==> "x.c".
 */
static char suffix[] = ".c";
static char *munge(const char *i) {
    const char *b, *d, *p;
    char *r;
    size_t l;

    d = 0;
    p = b = i;
    while (*p) {
	if (*p == '/') b = p + 1;
	if (*p == '.') d = p;
	++p;
    }
    if (!d) d = p;
    l = d - b;
    r = malloc(l + sizeof suffix);
    if (!r) nomem();
    strncpy(r, b, l);
    r[l] = '\0';
    strcat(r, suffix);
    return r;
}

void arg(int argc, char **argv) {
    int c, opt_i;

    while ((c = getopt_long(argc, argv, "d:f:o:r:", long_opts, &opt_i)) != -1) {
	switch (c) {
	case 'd': dump = optarg; break;
	case 'f': feed = optarg; break;
	case 'o': output = optarg; break;
	case 'r': feed_rule = optarg; break;
	case '?': usage(); break;
	default: assert(0);
	}
    }
    if (argc - optind != 1) usage();
    input = argv[optind];
    if (!output) output = munge(input);
}
