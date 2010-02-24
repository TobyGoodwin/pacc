/*
chars
parse foo foo
noparse bar A 0
*/

#include <sys/types.h>

#include "syntax.h"

int parse(char *ignore0, off_t ignore1, struct s_node **result) {
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
    p = new_node(grammar); p->text = "yy"; p->first = r;

    *result = p;
    return 1;
}
