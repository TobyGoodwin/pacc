/*
type: int
parse .5 5
parse .6 6
noparse .x A 2
*/

#include <sys/types.h>

#include "syntax.h"

int parse(const char *ign0, char *ign1, off_t ign2, struct s_node **result) {
    struct s_node *p, *q, *r, *s, *t;

    /* Nested calls with values:
     *
     * P <- '.' a:A { a }
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

    p = s_new(ident); p->text = "a"; q = p;
    p = new_node(expr); p->text = "a"; p->first = q; s = p;
    p = new_node(call); p->text = "A"; q = p;
    p = new_node(bind); p->text = "a"; p->first = q; p->next = s; q = p;
    p = new_node(lit); p->text = "."; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "int"; p->next = q; q = p;
    p = new_node(rule); p->text = "P"; p->first = q; p->next = r; r = p;

    r = cons(s_text(preamble, 0), r);
    p = s_both(grammar, "yy", r);

    *result = p;
    return 1;
}
