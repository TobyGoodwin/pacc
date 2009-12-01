#include "syntax.h"

char *prefix = 0;

struct s_node *create(void) {
    struct s_node *p, *q, *r, *s;

    /*
     * Regression. Eventually, some day, pacc should notice that rule A
     * never produces a value, and so it is an error to bind a name to
     * it. However, we're not that clever yet. It is important, though,
     * that the parser terminates! Till 2009-10-13 it didn't, because
     * "evaluating" a rule with no exprs left that rule in the state
     * "parsed", and never shifted it to "evaluated".
     *
     *     P ← a:A { 5 }
     *     A ← "5"
     */

    p = s_text(lit, "5"); q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_text(type, "int"); p->next = q; q = p;
    p = s_text(rule, "A"); p->first = q; r = p;

    p = s_text(ident, "a"); q = p;
    p = s_text(expr, "5"); p->first = q; q = p;
    p = s_text(call, "A"); s = p;
    p = s_text(bind, "a"); p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_text(type, "int"); p->next = q; q = p;
    p = s_text(rule, "P"); p->first = q; p->next = r; r = p;

    p = s_new(grammar); p->text = "yy"; p->first = r;

    return p;
}
