#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arg.h"
#include "error.h"

/* The function repsuf() returns a newly-allocated string holding a copy
 * of its input, with suffix replacing the existing suffix (or appended
 * if there was no suffix). For example, repsuf("/foo/x.pacc", ".c") ==>
 * "/foo/x.c".
 */
static char *repsuf(const char *i, const char *suffix) {
    const char *d, *p;
    char *r;
    size_t l;

    d = 0;
    for (p = i; *p; ++p) 
	if (*p == '.') d = p;
    if (!d) d = p;
    l = d - i;
    r = malloc(l + strlen(suffix) + 1);
    if (!r) nomem();
    strncpy(r, i, l);
    r[l] = '\0';
    strcat(r, suffix);
    return r;
}

static const struct option long_opts[] = {
    { "dump-ast", required_argument, 0, 'D' },
    { "defines", optional_argument, 0, 'd' },
    { "feed", required_argument, 0, 'f' },
    { "name", required_argument, 0, 'n' },
    { "output", required_argument, 0, 'o' },
    { "help", no_argument, 0, 'h' },
    { "version", no_argument, 0, 'v' },
    { 0, 0, 0, 0 }
};

/* Use "-D 0" to dump the AST before desugaring; 1 after desugaring, 2
 * after cooking. They can be combined, e.g. "-D02". */
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

static char *feed = 0;
char *arg_feed(void) {
    return feed;
}

static char *name = "pacc";
char *arg_name(void) {
    return name;
}

static char *output = 0;
char *arg_output(void) {
    assert(output);
    return output;
}

static void usage(void) {
    puts("Usage: pacc [OPTION]... FILE");
    puts("");
    puts("Operation modes:");
    puts("  -h, --help               display this help and exit");
    puts("  -v, --version            report version number and exit");
    puts("  -D, --dump-ast=WHEN      dump the parse tree at various points");
    puts("");
    puts("Parser:");
    puts("  -n, --name=NAME          name the grammar (default: pacc)");
    puts("  -f, --feed=FILE          write extra feed parser to FILE");
    puts("");
    puts("Output:");
    puts("  -d, --define[=FILE]      also produce a header file (default: BASE.h)");
    puts("  -o, --output=FILE        write output to FILE (default: BASE.c)");
    puts("");
    puts("Report bugs to <bug@paccrat.org>.");
    exit(0);
}

static void version(void) {
    puts("pacc 0.2 (ashigaru)");
    puts("Written by Tobold Jayne Goodwin <toby@paccrat.org>");
    puts("");
    puts("Copyright (C) 2012 - 2015 Free Software Foundation, Inc.");
    puts("This is free software; see the source for copying conditions.  There is NO");
    puts("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.");
    exit(0);
}

void arg(int argc, char **argv) {
    int c, opt_i;

    while ((c = getopt_long(argc, argv, "D:d::f:hn:o:v", long_opts, &opt_i))
            != -1) {
	switch (c) {
	case 'D': dump = optarg; break;
	case 'd': defining = 1; defines = optarg; break;
	case 'f': feed = optarg; break;
	case 'n': name = optarg; break;
	case 'o': output = optarg; break;
	case 'v': version(); break;
	case 'h': usage(); break;
	case '?': exit(1); break;
	default: assert(0);
	}
    }
    if (argc - optind != 1) usage();
    input = argv[optind];
    if (!output) output = repsuf(input, ".c");
    if (defining && !defines) defines = repsuf(output, ".h");
}
