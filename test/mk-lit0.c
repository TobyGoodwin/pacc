#include "syntax.h"

char *prefix = 0;

struct s_node *create(void) {
    struct s_node *p, *q, *r, *s;

    /* A multi-character literal:
     *
     * A :: char * ← "foo" { match() }
     *
     */

    /* A :: char * ← "foo" */
    p = new_node(expr); p->text = "match()"; q = p;
    p = new_node(lit); p->text = "foo"; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "A"; p->first = q; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    return p;
}
