/* XXX is this needed when all debugging done? */
#include <stdio.h>

#include "arg.h"
#include "emit.h"
#include "load.h"
#include "pacc.h"
#include "syntax.h"

int main(int argc, char **argv) {
    char *in;
    off_t size;
    struct s_node *p;

    arg(argc, argv);

    in = load(arg_input(), &size);

    /* XXX what about command line flags to enable / disable dumping of
     * the tree? */
    if (parse(arg_input(), in, size, &p)) {
	s_dump(p);
	desugar(p);
	resolve(p);
	s_dump(p);
	save(arg_output());
	emit(p);
	return 0;
    }

    return 1;
}
