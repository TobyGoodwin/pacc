#include "syntax.h"

int main(int argc, char **argv) {
    if (argc > 1)
	pparse(argv[1]);
    return 0;
}
