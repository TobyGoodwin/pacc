/*
type: chars
source: arg
parse ab a
noparse ac '"b"' 2
*/

#include <sys/types.h>

#include "syntax.h"

char *prefix = 0;

int parse(const char *ign0, char *ign1, off_t ign2, struct s_node **result) {
    struct s_node *p, *q, *r, *s;

    /* A single character with value:
     *
     * char *A ← 'a' &( 'b' ) { ref_str() }
     *
     */

    p = new_node(expr); p->text = "ref_str()"; q = p;
    p = new_node(lit); p->text = "b"; s = p;
    p = new_node(seq); p->first = s; s = p;
    p = new_node(and); p->first = s; p->next = q; q = p;
    p = new_node(lit); p->text = "a"; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "A"; p->first = q; r = p;

    r = cons(s_text(preamble, 0), r);
    p = new_node(grammar); p->text = "yy"; p->first = r;

    *result = p;
    return 1;
}
