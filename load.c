#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "error.h"

char *load(const char *n, off_t *size) {
    char *addr;
    int fd;
    struct stat sb;

    fd = open(n, O_RDONLY);
    if (fd == -1) fatal3x("cannot open `", n, "'");

    if (fstat(fd, &sb) == -1) fatal3x("cannot stat `", n, "'");

    *size = sb.st_size;
    addr = (char *)-1;
    addr = mmap(0, *size, PROT_READ, MAP_SHARED, fd, 0);

    /* XXX eventually, we must try to read the file if we cannot mmap it */
    if (addr == (char *)-1) fatal3x("cannot mmap `", n, "'");

    return addr;
}

void save(const char *n) {
    close(1);
    if (open(n, O_WRONLY | O_CREAT, 0666) != 1)
	fatal3x("cannot open `", n, "' for writing");
}
