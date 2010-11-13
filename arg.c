#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arg.h"
#include "assert.h"
#include "error.h"

static const struct option long_opts[] = {
    { "output", required_argument, 0, 'o' },
    { "partial-rule", required_argument, 0, 'p' },
    { 0, 0, 0, 0 }
};

static char *input = 0;
char *arg_input() {
    assert(input);
    return input;
}

static char *output = 0;
char *arg_output() {
    assert(output);
    return output;
}

static char *partial = 0;
char *arg_partial() {
    return partial;
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

    while ((c = getopt_long(argc, argv, "o:p:", long_opts, &opt_i)) != -1) {
	switch (c) {
	case 'o':
	    output = optarg;
	    break;
	case 'p':
	    partial = optarg;
	    break;
	case '?':
	    usage();
	    break;
	default:
	    assert(0);
	}
    }
    if (argc - optind != 1) usage();
    input = argv[optind];
    if (!output) output = munge(input);
}
