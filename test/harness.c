#include <stdio.h>
#include <string.h>

#include "parse.h"

int main(int argc, char **argv) {
    PACC_TYPE r;

    if (pacc_parse("arg", argv[1], strlen(argv[1]), &r))
	printf("parsed with value " PACC_TYPE_FORMAT "\n", r);

    return 0;
}
