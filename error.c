#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

static void message2(const char *a, const char *b) {
    fprintf(stderr, "%s%s\n", a, b);
}

static void message4(const char *a, const char *b, const char *c,
        const char *d) {
    fprintf(stderr, "%s%s%s%s\n", a, b, c, d);
}

static void message6(const char *a, const char *b, const char *c,
        const char *d, const char *e, const char *f) {
    fprintf(stderr, "%s%s%s%s%s%s\n", a, b, c, d, e, f);
}

static void message10(const char *a, const char *b, const char *c,
        const char *d, const char *e, const char *f, const char *g,
        const char *h, const char *i, const char *j) {
    fprintf(stderr, "%s%s%s%s%s%s%s%s%s%s\n", a, b, c, d, e, f, g, h, i, j);
}

static const char *fatal = "pacc: fatal: ";

void fatal1(const char *a) {
    message2(fatal, a);
    exit(1);
}

void fatal3(const char *a, const char *b, const char *c) {
    message4(fatal, a, b, c);
    exit(1);
}

void fatal5(const char *a, const char *b, const char *c,
        const char *d, const char *e) {
    message6(fatal, a, b, c, d, e);
    exit(1);
}

void fatal9(const char *a, const char *b, const char *c,
        const char *d, const char *e, const char *f,
        const char *g, const char *h, const char *i) {
    message10(fatal, a, b, c, d, e, f, g, h, i);
    exit(1);
}

void fatal3x(const char *a, const char *b, const char *c) {
    fatal5(a, b, c, ": ", strerror(errno));
}

static const char *warning = "pacc: warning: ";

void warn3(const char *a, const char *b, const char *c) {
    message4(warning, a, b, c);
}

/* XXX how easy would it be to report the size of the failed allocation? */
void nomem(void) {
    fatal1("out of memory");
}
