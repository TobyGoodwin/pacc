#include <stdio.h>
#include <string.h>

#include "parse.h"

int main(int argc, char **argv) {
    int r;

    if (parse(argv[1], strlen(argv[1]), &r))
	printf("parsed with value %d\n", r);
    else
	printf("not parsed\n");

    return 0;
}
