#include "syntax.h"

char *prefix = 0;

struct s_node *create(void) {
    struct s_node *p, *q, *r, *s;

    /* Semantic predicate with two bindings:
     *
     * int P ← a:A b:A &{ a == b } { a }
     * int A ← '5' { 5 } / '6' { 6 }
     *
     */

    p = new_node(expr); p->text = "6"; q = p;
    p = new_node(lit); p->text = "6"; p->next = q; q = p;
    p = new_node(seq); p->first = q; s = p;
    p = new_node(expr); p->text = "5"; q = p;
    p = new_node(lit); p->text = "5"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(type); p->text = "int"; p->next = q; q = p;
    p = new_node(rule); p->text = "A"; p->first = q; r = p;

    p = s_new(ident); p->text = "a"; q = p;
    p = new_node(expr); p->text = "a"; p->first = q; q = p;
    p = new_node(ident); p->text = "b"; s = p;
    p = new_node(ident); p->text = "a"; p->next = s; s = p;
    p = new_node(guard); p->text = "a == b"; p->first = s; p->next = q; q = p;
    p = new_node(call); p->text = "A"; s = p;
    p = new_node(bind); p->text = "b"; p->first = s; p->next = q; q = p;
    p = new_node(call); p->text = "A"; s = p;
    p = new_node(bind); p->text = "a"; p->first = s; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "int"; p->next = q; q = p;
    p = new_node(rule); p->text = "P"; p->first = q; p->next = r; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    resolve(p, p);

    return p;
}
