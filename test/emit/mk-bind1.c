/*
type chars
parse xyzx yz
*/

#include <sys/types.h>

#include "syntax.h"

int pacc_wrap(const char *ign0, char *ign1, off_t ign2, struct s_node **result) {
    struct s_node *p, *r;

    /* ... but binding to anything other than a call is sugar
     *
     * S :: char * <- "x" r:("y" "z") "x" { ref_dup(r) }
     *
     */

    p = s_both(expr, "ref_dup(r)", s_text(ident, "r"));
    p = cons(s_text(lit, "x"), p);
    p = cons(s_both(bind, "r", s_kid(seq, cons(s_text(lit, "y"), s_text(lit, "z")))), p);
    p = cons(s_text(lit, "x"), p);
    p = s_kid(seq, p);
    p = cons(s_text(type, "char *"), p);
    r = s_both(rule, "S", p);

    r = cons(s_text(preamble, 0), r);
    p = s_kid(grammar, r);

    *result = p;
    return 1;
}
