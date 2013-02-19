/*
type chars
parse 5 5
*/

#include <sys/types.h>

#include "syntax.h"

int pacc_wrap(const char *ign0, char *ign1, off_t ign2, struct s_node **result) {
    struct s_node *p, *q, *r, *s;

    /* Type char * with ref_str():
     *
     * char *A <- '5' { ref_str() }
     *
     */

    /* A <- 'a' */
    p = new_node(expr); p->text = "ref_str()"; q = p;
    p = new_node(lit); p->text = "5"; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "A"; p->first = q; r = p;

    r = cons(s_text(preamble, 0), r);
    p = s_kid(grammar, r);

    *result = p;
    return 1;
}
