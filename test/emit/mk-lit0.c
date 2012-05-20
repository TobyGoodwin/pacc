/*
type chars
parse foo foo
noparse bar A 1
*/

#include <sys/types.h>

#include "syntax.h"

int pacc_parse(const char *ign0, char *ign1, off_t ign2, struct s_node **result) {
    struct s_node *p, *q, *r, *s;

    /* A multi-character literal:
     *
     * A :: char * ← "foo" { ref_str() }
     *
     */

    /* A :: char * ← "foo" */
    p = new_node(expr); p->text = "ref_str()"; q = p;
    p = new_node(lit); p->text = "foo"; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "A"; p->first = q; r = p;

    r = cons(s_text(preamble, 0), r);
    p = new_node(grammar); p->text = "pacc"; p->first = r;

    *result = p;
    return 1;
}
