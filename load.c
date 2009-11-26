#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "error.h"

off_t load(const char *n) {
    char *p;
    int fd;
    off_t size;
    struct stat sb;

    fd = open(n, O_RDONLY);
    if (fd == -1) fatal3x("cannot open `", n, "'");

    if (fstat(fd, &sb) == -1) fatal3x("cannot stat `", n, "'");

    size = sb.st_size;
    p = (char *)-1;
    p = mmap(0, size, PROT_READ, MAP_SHARED, fd, 0);

    /* XXX eventually, we must try to read the file if we cannot mmap it */
    if (p == (char *)-1) fatal3x("cannot mmap `", n, "'");

    return size;
}
