/*
type int
parse 5.5 25
parse 5.6 30
parse 6.5 30
parse 6.6 36
noparse x P 1
noparse 5.x A 3
noparse 6.x A 3
parse 5+5 10
parse 5+6 11
parse 6+5 11
parse 6+6 12
noparse 5+x A 3
noparse 6+x A 3
noparse 5.+ A 3
*/

#include <sys/types.h>

#include "syntax.h"

int parse(const char *ign0, char *ign1, off_t ign2, struct s_node **result) {
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

    p = s_new(ident); p->text = "v"; q = p;
    p = new_node(expr); p->text = "v"; p->first = q; s = p;
    p = new_node(call); p->text = "B"; q = p;
    p = new_node(bind); p->text = "v"; p->first = q; p->next = s; q = p;
    p = new_node(seq); p->first = q; s = p;
    p = new_node(expr); p->text = "5"; q = p;
    p = new_node(lit); p->text = "5"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(type); p->text = "int"; p->next = q; q = p;
    p = new_node(rule); p->text = "A"; p->first = q; p->next = r; r = p;

    p = s_new(ident); p->text = "f"; q = p;
    p = s_new(ident); p->text = "e"; p->next = q; q = p;
    p = new_node(expr); p->text = "e + f"; p->first = q; s = p;
    p = new_node(call); p->text = "A"; q = p;
    p = new_node(bind); p->text = "f"; p->first = q; p->next = s; s = p;
    p = new_node(lit); p->text = "+"; p->next = s; s = p;
    p = new_node(call); p->text = "A"; q = p;
    p = new_node(bind); p->text = "e"; p->first = q; p->next = s; q = p;
    p = new_node(seq); p->first = q; t = p;

    p = s_new(ident); p->text = "b"; q = p;
    p = s_new(ident); p->text = "a"; p->next = q; q = p;
    p = new_node(expr); p->text = "a * b"; p->first = q; s = p;
    p = new_node(call); p->text = "A"; q = p;
    p = new_node(bind); p->text = "b"; p->first = q; p->next = s; s = p;
    p = new_node(lit); p->text = "."; p->next = s; s = p;
    p = new_node(call); p->text = "A"; q = p;
    p = new_node(bind); p->text = "a"; p->first = q; p->next = s; q = p;
    p = new_node(seq); p->first = q; p->next = t; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(type); p->text = "int"; p->next = q; q = p;
    p = new_node(rule); p->text = "P"; p->first = q; p->next = r; r = p;

    r = cons(s_text(preamble, 0), r);
    p = s_both(grammar, "yy", r);

    *result = p;
    return 1;
}
