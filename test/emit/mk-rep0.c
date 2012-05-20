/*
type chars
parse xx xx
parse xyx xyx
parse xyyx xyyx
parse xyyyx xyyyx
parse xyyyyyyyyyyx xyyyyyyyyyyx
*/

#include <sys/types.h>

#include "syntax.h"

int pacc_parse(const char *ign0, char *ign1, off_t ign2, struct s_node **result) {
    struct s_node *p, *r;

    /* The * operator:
     *
     * char *S <- "x" Y "x" → { ref_str() }
     * Y ← "y" *
     *
     */

    p = s_text(lit, "y");
    p = s_both(rep, 0, p);
    p = s_kid(seq, p);
    p = cons(s_text(type, "char *"), p);
    r = s_both(rule, "Y", p);

    p = s_text(expr, "ref_str()");
    p = cons(s_text(lit, "x"), p);
    p = cons(s_text(call, "Y"), p);
    p = cons(s_text(lit, "x"), p);
    p = s_kid(seq, p);
    p = cons(s_text(type, "char *"), p);
    r = cons(s_both(rule, "S", p), r);

    r = cons(s_text(preamble, 0), r);
    p = s_both(grammar, "pacc", r);

    *result = p;
    return 1;
}
