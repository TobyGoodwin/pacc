#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arg.h"
#include "assert.h"
#include "error.h"

/* The inventively named munge() returns a newly-allocated string
 * holding the basename of its input, with suffix replacing the existing
 * suffix (or appended if there was no suffix). For example,
 * munge("/foo/x.pacc", ".c") ==> "x.c".
 */
static char *munge(const char *i, const char *suffix) {
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

static const struct option long_opts[] = {
    { "dump-ast", required_argument, 0, 'D' },
    { "defines", optional_argument, 0, 'd' },
    { "feed", optional_argument, 0, 'f' },
    { "output", required_argument, 0, 'o' },
    { "help", no_argument, 0, 'h' },
    { "version", no_argument, 0, 'v' },
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

static int defining = 0;
static char *defines = 0;
char *arg_defines(void) {
    return defines;
}

static int feeding = 0;
static char *feed = 0;
char *arg_feed(void) {
    if (!feeding) return 0;
    if (!feed) return "__";
    return feed;
}

static char *output = 0;
char *arg_output(void) {
    assert(output);
    return output;
}

static void usage(void) {
    puts("Usage: pacc [OPTION]... FILE");
    puts("");
    puts("Operation modes");
    puts("  -h, --help               display this help and exit");
    puts("  -v, --version            report version number and exit");
    puts("  -D, --dump-ast=WHEN      dump the parse tree at various points");
    puts("");
    puts("Parser:");
    puts("  -f, --feed[=RULE]          produce two parsers for feeding");
    puts("");
    puts("Output:");
    puts("  -d, --defines[=FILE]       also produce a header file");
    puts("  -o, --output=FILE          write output to FILE");
    puts("");
    puts("Report bugs to <bug@paccrat.org>.");
    exit(0);
}

static void version(void) {
    puts("pacc 0.0 (rōnin)");
    puts("Written by Tobias Jayne Goodwin <toby@paccrat.org>");
    puts("");
    puts("Copyright (C) 2012 Free Software Foundation, Inc.");
    puts("This is free software; see the source for copying conditions.  There is NO");
    puts("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.");
    exit(0);
}

void arg(int argc, char **argv) {
    int c, opt_i;

    while ((c = getopt_long(argc, argv, "D:d::f::ho:v", long_opts, &opt_i)) != -1) {
	switch (c) {
	case 'D': dump = optarg; break;
	case 'd': defining = 1; defines = optarg; break;
	case 'f': feeding = 1; feed = optarg; break;
	case 'o': output = optarg; break;
	case 'v': version(); break;
	case 'h': usage(); break;
	case '?': exit(1); break;
	default: assert(0);
	}
    }
    if (argc - optind != 1) usage();
    input = argv[optind];
    if (!output) output = munge(input, ".c");
    if (defining && !defines) defines = munge(output, ".h");
}
