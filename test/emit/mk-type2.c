#include "syntax.h"

char *prefix = 0;

struct s_node *create(void) {
    struct s_node *p, *q, *r, *s;

    /* Type char * with call and match macro:
     *
     * char *P <- A P { match() } / ε
     * char *A <- 'a'
     *
     */

    /* A <- 'a' */
    p = new_node(lit); p->text = "a"; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "A"; p->first = q; r = p;

    p = new_node(seq); s = p;
    p = new_node(expr); p->text = "match()"; q = p;
    p = new_node(call); p->text = "P"; p->next = q; q = p;
    p = new_node(call); p->text = "A"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "P"; p->first = q; p->next = r; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    return p;
}
