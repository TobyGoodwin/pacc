#include "syntax.h"

char *prefix = 0;

struct s_node *create(void) {
    struct s_node *p, *q, *r, *s;

    /* End of input:
     *
     * S ← "foo" !. { "yes" }
     *
     */

    p = new_node(expr); p->text = "\"yes\""; q = p;
    p = new_node(any); s = p;
    p = new_node(not); p->first = s; p->next = q; q = p;
    p = new_node(lit); p->text = "o"; p->next = q; q = p;
    p = new_node(lit); p->text = "o"; p->next = q; q = p;
    p = new_node(lit); p->text = "f"; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "S"; p->first = q; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    return p;
}
