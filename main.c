/* XXX is this needed when all debugging done? */
#include <stdio.h>
#include <string.h>

#include "arg.h"
#include "cook.h"
#include "emit.h"
#include "load.h"
#include "pacc.h"
#include "preen.h"
#include "syntax.h"

int main(int argc, char **argv) {
    char *in;
    off_t size;
    struct s_node *p;

    arg(argc, argv);

//if (arg_defines()) puts(arg_defines());
//exit(0);
#if 0
if (arg_feed()) {
fprintf(stderr, "we will be feeding, fn = %s, rule = %s\n", arg_feed(), arg_feed_rule());
} else {
fprintf(stderr, "not feeding\n");
}
#endif

    in = load(arg_input(), &size);

    if (!pacc_wrap(arg_input(), in, size, &p))
	return 1;

    if (strchr(arg_dump(), '0')) s_dump(p);

    desugar(p);
    preen(p);

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
