#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "arg.h"

static const struct option long_opts[] = {
    { "output", required_argument, 0, 'o' },
    { 0, 0, 0, 0 }
};

static char *output = 0;
char *arg_output() {
    assert(output);
    return output;
}

void arg(int argc, char **argv) {
    int c, opt_i;

    while ((c = getopt_long(argc, argv, "o:", long_opts, &opt_i)) != -1) {
	printf("got an option!\n");
	switch (c) {
	case 'o':
	    output = optarg;
	    break;
	case '?':
	    /* getopt_long() has already complained */
	    break;
	default:
	    assert(0);
	}
    }
    printf("output is %s\n", arg_output());
    exit(0);
}

