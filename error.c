#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

static char *fatal = "pacc: fatal: ";

void fatal1(const char *a) {
    fprintf(stderr, "%s%s\n", fatal, a);
    exit(1);
}

void fatal3(const char *a, const char *b, const char *c) {
    fprintf(stderr, "%s%s%s%s\n", fatal, a, b, c);
    exit(1);
}

void fatal5(const char *a, const char *b, const char *c, const char *d, const char *e) {
    fprintf(stderr, "%s%s%s%s%s%s\n", fatal, a, b, c, d, e);
    exit(1);
}

void fatal3x(const char *a, const char *b, const char *c) {
    fatal5(a, b, c, ": ", strerror(errno));
}

void nomem(void) {
    fatal1("out of memory");
}
