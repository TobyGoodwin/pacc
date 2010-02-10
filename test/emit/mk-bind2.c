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

int parse(char *ignore0, off_t ignore1, struct s_node **result) {
    struct s_node *p, *r;

    /* ... but binding to anything other than a call is sugar
     *
     * S :: char * <- "x" y:"y"* "x" { ref_str(y) }
     *
     */

    p = s_both(expr, "ref_str(y)", s_text(ident, "y"));
    p = cons(s_text(lit, "x"), p);
    p = cons(s_both(bind, "y", s_both(rep, 0, s_text(lit, "y"))), p);
    p = cons(s_text(lit, "x"), p);
    p = s_kid(seq, p);
    p = cons(s_text(type, "char *"), p);
    r = s_both(rule, "S", p);

    r = cons(s_text(preamble, 0), r);
    p = s_both(grammar, "yy", r);

    *result = p;
    return 1;
}
