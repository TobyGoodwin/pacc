#include "syntax.h"

char *prefix = 0;

struct s_node *create(void) {
    struct s_node *p, *q, *r, *s;

    /* A semantic predicate, with no binding
     *
     * A <- '5' &{ printf("hello, world!\n") } { 5 }
     *
     */

    /* A <- 'a' */
    p = new_node(expr); p->text = "5"; q = p;
    p = new_node(guard); p->text = "printf(\"hello, world!\\n\")"; p->next = q; q = p;
    p = new_node(lit); p->text = "5"; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "int"; p->next = q; q = p;
    p = new_node(rule); p->text = "A"; p->first = q; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    resolve(p, p);

    return p;
}
