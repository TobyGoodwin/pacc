#include "syntax.h"

int main(int argc, char **argv) {
    struct s_node *p;

    if (parse(&p, argv[1])) {
	s_dump(p);
	desugar(p);
	resolve(p);
	s_dump(p);
	emit(p);
    }

    return 0;
}
