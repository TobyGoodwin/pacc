#include "arg.h"
#include "load.h"
#include "syntax.h"

int main(int argc, char **argv) {
    off_t size;
    struct s_node *p;

    arg(argc, argv);

    size = load(arg_input());
    printf("loaded %ld bytes!\n", size);
    exit(0);

    if (parse(&p, argv[1])) {
	s_dump(p);
	desugar(p);
	resolve(p);
	s_dump(p);
	emit(p);
    }

    return 0;
}
