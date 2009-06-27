#include "syntax.h"

char *prefix = 0;

struct s_node *create(void) {
    struct s_node *p, *q, *r, *s, *t;

    /* Nested calls with alternatives & values:
     *
     * P <- a:A '.' b:A { a * b } / e:A '+' f:A { e + f }
     * A <- '5' { 5 } / B
     * B <- '6' { 6 }
     *
     */

    p = new_node(expr); p->text = "6"; q = p;
    p = new_node(lit); p->text = "6"; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "int"; p->next = q; q = p;
    p = new_node(rule); p->text = "B"; p->first = q; r = p;

    p = new_node(expr); p->text = "v"; s = p;
    p = new_node(call); p->text = "B"; q = p;
    p = new_node(bind); p->text = "v"; p->first = q; p->next = s; q = p;
    p = new_node(seq); p->first = q; s = p;
    p = new_node(expr); p->text = "5"; q = p;
    p = new_node(lit); p->text = "5"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(type); p->text = "int"; p->next = q; q = p;
    p = new_node(rule); p->text = "A"; p->first = q; p->next = r; r = p;

    p = new_node(expr); p->text = "e + f"; s = p;
    p = new_node(call); p->text = "A"; q = p;
    p = new_node(bind); p->text = "f"; p->first = q; p->next = s; s = p;
    p = new_node(lit); p->text = "+"; p->next = s; s = p;
    p = new_node(call); p->text = "A"; q = p;
    p = new_node(bind); p->text = "e"; p->first = q; p->next = s; q = p;
    p = new_node(seq); p->first = q; t = p;

    p = new_node(expr); p->text = "a * b"; s = p;
    p = new_node(call); p->text = "A"; q = p;
    p = new_node(bind); p->text = "b"; p->first = q; p->next = s; s = p;
    p = new_node(lit); p->text = "."; p->next = s; s = p;
    p = new_node(call); p->text = "A"; q = p;
    p = new_node(bind); p->text = "a"; p->first = q; p->next = s; q = p;
    p = new_node(seq); p->first = q; p->next = t; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(type); p->text = "int"; p->next = q; q = p;
    p = new_node(rule); p->text = "P"; p->first = q; p->next = r; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    resolve(p, p);

    return p;
}
