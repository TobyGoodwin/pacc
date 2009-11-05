#include "syntax.h"

char *prefix = 0;

struct s_node *create(void) {
    struct s_node *p, *r;

    /* The * operator with binding:
     *
     * char *S <- "x" y:Y "x" → y
     * Y ← "y" * { match() }
     *
     */

    p = s_text(expr, "match()");
    p = cons(s_both(rep, 0, s_text(lit, "y")), p);
    p = s_kid(seq, p);
    p = cons(s_text(type, "char *"), p);
    r = s_both(rule, "Y", p);

    p = s_both(expr, "y", s_text(ident, "y"));
    p = cons(s_text(lit, "x"), p);
    p = cons(s_both(bind, "y", s_text(call, "Y")), p);
    p = cons(s_text(lit, "x"), p);
    p = s_kid(seq, p);
    p = cons(s_text(type, "char *"), p);
    r = cons(s_both(rule, "S", p), r);

    p = s_both(grammar, "yy", r);

    return p;
}
