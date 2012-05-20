/*
chars
parse xx ''
parse xyx y
parse xyyx yy
parse xyyyx yyy
parse xyyyyyyyyyyx yyyyyyyyyy
*/

#include <sys/types.h>

#include "syntax.h"

int pacc_parse(const char *ign0, char *ign1, off_t ign2, struct s_node **result) {
    struct s_node *p, *r;

    /* Binding to a call is not sugar...
     *
     * char *S <- "x" y:Y "x" → y
     * Y ← "y" * { ref_str() }
     *
     */

    p = s_text(expr, "ref_str()");
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

    r = cons(s_text(preamble, 0), r);
    p = s_both(grammar, "pacc", r);

    *result = p;
    return 1;
}
