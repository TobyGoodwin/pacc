#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "error.h"

char *load(const char *n, size_t *size) {
    char *addr;
    struct stat sb;
    FILE *fp;

    close(0);
    if (open(n, O_RDONLY) != 0) fatal3x("cannot open `", n, "'");

    if (fstat(0, &sb) == -1) fatal3x("cannot stat `", n, "'");

    *size = sb.st_size;

    /* It is an error to mmap nothing. */
    if (*size == 0) return "";

    addr = (char *)-1;
    addr = mmap(0, *size, PROT_READ, MAP_SHARED, 0, 0);
    if (addr != (char *)-1)
        return addr;

    /* Fail to mmap, try to read it. */
    fp = fopen(n, "r");
    if (fp == NULL)
        fatal3x("cannot fopen `", n, "'");

    addr = malloc(*size);
    if (addr == NULL)
        fatal3x("cannot allocate to read `", n, "'");

    fread(addr, 1, *size, fp);
    fclose(fp);
    return addr;
}

void save(const char *n) {
    close(1);
    if (open(n, O_WRONLY | O_CREAT | O_TRUNC, 0666) != 1)
	fatal3x("cannot open `", n, "' for writing");
}
