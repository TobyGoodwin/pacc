/*
type int
parse 5 5
parse 6 5
parse x 5
parse '£' 5
noparse '' A 1
*/

#include <sys/types.h>

#include "syntax.h"

int pacc_wrap(const char *ign0, char *ign1, off_t ign2, struct s_node **result) {
    struct s_node *p, *q, *r, *s;

    /* A single character with value:
     *
     * A :: int <- . { 5 }
     *
     */

    /* A <- 'a' */
    p = cons(s_new(any), s_text(expr, "5"));
    p = s_kid(seq, p);
    p = cons(s_text(type, "int"), p);
    p = s_both(rule, "A", p);

    p = cons(s_text(preamble, 0), p);
    p = s_kid(grammar, p);

    *result = p;
    return 1;
}
