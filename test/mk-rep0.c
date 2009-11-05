#include "syntax.h"

char *prefix = 0;

struct s_node *create(void) {
    struct s_node *p, *r;

    /* The * operator:
     *
     * char *S <- "x" Y "x" → { match() }
     * Y ← "y" *
     *
     */

    p = s_text(lit, "y");
    p = s_both(rep, 0, p);
    p = s_kid(seq, p);
    p = cons(s_text(type, "char *"), p);
    r = s_both(rule, "Y", p);

    p = s_text(expr, "match()");
    p = cons(s_text(lit, "x"), p);
    p = cons(s_text(call, "Y"), p);
    p = cons(s_text(lit, "x"), p);
    p = s_kid(seq, p);
    p = cons(s_text(type, "char *"), p);
    r = cons(s_both(rule, "S", p), r);

    p = s_both(grammar, "yy", r);

    return p;
}
