#include <stdio.h>

#include "error.h"

static char *fatal = "pacc: fatal: ";

void fatal3(char *a, char *b, char *c) {
    fprintf(stderr, "%s%s%s%s\n", fatal, a, b, c);
    _exit(1);
}
