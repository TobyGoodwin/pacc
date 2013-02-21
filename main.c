/* XXX is this needed when all debugging done? */
#include <stdio.h>
#include <string.h>

#include "arg.h"
#include "cook.h"
#include "emit.h"
#include "load.h"
#include "preen.h"
#include "syntax.h"

extern int pacc_wrap(const char *, char *, size_t, struct s_node **);

int main(int argc, char **argv) {
    char *in;
    off_t size;
    struct s_node *p;

    arg(argc, argv);

    in = load(arg_input(), &size);

    if (!pacc_wrap(arg_input(), in, size, &p))
	return 1;

    if (strchr(arg_dump(), '0')) s_dump(p);

    desugar(p);
    preen(p, arg_name());

    if (strchr(arg_dump(), '1')) s_dump(p);

    save(arg_output());
    emit(p);

    if (arg_defines()) {
	save(arg_defines());
	header(p);
    }

    if (arg_feed()) {
	cook(p);
	if (strchr(arg_dump(), '2')) s_dump(p);
	save(arg_feed());
	emit(p);
    }

    return 0;
}
