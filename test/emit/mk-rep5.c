/*
type int
parse xx 0
parse xyx 1
parse xyyx 2
parse xyyyx 3
parse xyyyyyyyyyyx 10
*/

#include <sys/types.h>

#include "syntax.h"

/* First test of new-style repetitions */

int pacc_wrap(const char *ign0, char *ign1, off_t ign2, struct s_node **result) {
    struct s_node *p, *q, *r;

    /* The * operator:
     *
     * S <- "x" u:Unary "x" → u
     * Unary* ← { 0 } "y" { a + 1 }
     *
     */

    /*
    p = s_text(lit, "y");
    p = s_both(rep, 0, p);
    p = s_kid(seq, p);
    p = cons(s_text(type, "void"), p);
    r = s_both(rule, "Y", p);
    */

    //p = s_both(expr, "a", s_text(ident, "a"));
    //q = s_both(expr, "a+1", s_text(ident, "a"));
    q = 0;
    q = cons(s_text(lit, "y"), q);
    q = cons(s_text(expr, "0"), s_kid(seq, q));
    q = s_both(rep, 0, q);
    p = cons(q, p);
    p = s_kid(seq, p);
    p = cons(s_text(type, "int"), p);
    r = s_both(rule, "Unary", p);

    p = s_both(expr, "u", s_text(ident, "u"));
    p = cons(s_text(lit, "x"), p);
    p = cons(s_both(bind, "u", s_text(call, "Unary")), p);
    p = cons(s_text(lit, "x"), p);
    p = s_kid(seq, p);
    p = cons(s_text(type, "int"), p);
    r = cons(s_both(rule, "S", p), r);

    r = cons(s_text(preamble, 0), r);
    p = s_kid(grammar, r);

    *result = p;
    return 1;
}
