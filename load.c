#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "error.h"

char *load(const char *n, off_t *size) {
    char *addr;
    struct stat sb;

    close(0);
    if (open(n, O_RDONLY) != 0) fatal3x("cannot open `", n, "'");

    if (fstat(0, &sb) == -1) fatal3x("cannot stat `", n, "'");

    *size = sb.st_size;
    addr = (char *)-1;
    addr = mmap(0, *size, PROT_READ, MAP_SHARED, 0, 0);

    /* XXX eventually, we must try to read the file if we cannot mmap it */
    if (addr == (char *)-1) fatal3x("cannot mmap `", n, "'");

    return addr;
}

void save(const char *n) {
    close(1);
    if (open(n, O_WRONLY | O_CREAT | O_TRUNC, 0666) != 1)
	fatal3x("cannot open `", n, "' for writing");
}
