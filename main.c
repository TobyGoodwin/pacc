#include "syntax.h"

int main(int argc, char **argv) {
    struct s_node *p;

    p = create();
    s_dump(p);
    emit(p);

    if (argc > 1)
	pparse(argv[1]);
    return 0;
}
