#include "syntax.h"

int main(int argc, char **argv) {
    struct s_node *p;

    if (parse(&p, argv[1])) {
	s_dump(p);
	desugar(p);
	resolve(p);
	s_dump(p);
	printf("\n---cut here\n");
	emit(p);
	printf("\n---cut here\n");
    }

    return 0;
}
