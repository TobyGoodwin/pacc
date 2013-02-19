/*
type chars
parse ac ac
noparse a '"b"' 2
noparse ab A 1
*/

/* XXX obviously we really don't mean: expected "b" */

#include <sys/types.h>

#include "syntax.h"

int pacc_wrap(const char *ign0, char *ign1, off_t ign2, struct s_node **result) {
    struct s_node *p, *q, *r, *s;

    /* A single character with value:
     *
     * char *A ← 'a' !( 'b' ) . { ref_str() }
     *
     */

    p = new_node(expr); p->text = "ref_str()"; q = p;
    q = cons(s_new(any), q);
    p = new_node(lit); p->text = "b"; s = p;
    p = new_node(seq); p->first = s; s = p;
    p = new_node(not); p->first = s; p->next = q; q = p;
    p = new_node(lit); p->text = "a"; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "A"; p->first = q; r = p;

    r = cons(s_text(preamble, 0), r);
    p = s_kid(grammar, r);

    *result = p;
    return 1;
}
