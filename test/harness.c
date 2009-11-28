#include <stdio.h>
#include <string.h>

#include "parse.h"

int main(int argc, char **argv) {
    int r;

    parse(argv[1], strlen(argv[1]), &r);

    return 0;
}
