/*
type int
parse 5 5
*/

#include <sys/types.h>

#include "syntax.h"

int pacc_wrap(const char *ign0, char *ign1, off_t ign2, struct s_node **result) {
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

    r = cons(s_text(preamble, 0), r);
    p = new_node(grammar); p->text = "pacc"; p->first = r;

    *result = p;
    return 1;
}
