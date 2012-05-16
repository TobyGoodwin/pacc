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

#if 0
if (arg_feed()) {
fprintf(stderr, "we will be feeding, fn = %s, rule = %s\n", arg_feed(), arg_feed_rule());
} else {
fprintf(stderr, "not feeding\n");
}
#endif

    in = load(arg_input(), &size);

    if (parse(arg_input(), in, size, &p)) {
	if (strchr(arg_dump(), '0')) s_dump(p);
	desugar(p);
	preen(p);
	if (strchr(arg_dump(), '1')) s_dump(p);
	save(arg_output());
	emit(p);
	if (arg_feed()) {
	    cook(p);
	    if (strchr(arg_dump(), '2')) s_dump(p);
	    save(arg_feed());
	    emit(p);
	}
	return 0;
    }

    return 1;
}
