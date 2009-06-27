#include "syntax.h"

char *prefix = 0;

struct s_node *create(void) {
    struct s_node *p, *q, *r, *s, *t;

    /* Bryan Ford's trivial grammar with match():
     *
     * Additive <- m:Multitive '+' a:Additive { m + a } / Multitive
     * Multitive <- p:Primary '*' m:Multitive { p * m } / Primary
     * Primary <- '(' a:Additive ')' -> a / Decimal
     * int Decimal <- Digit { match()[0] - '0' }
     * int Digit <- '0' / '1' / ... / '9'
     */

    /* int Digit <- '0' / '1' / ... / '9' */
    p = new_node(lit); p->text = "9"; q = p;
    p = new_node(seq); p->first = q; s = p;
    p = new_node(lit); p->text = "8"; q = p;
    p = new_node(seq); p->first = q; p->next = s; s = p;
    p = new_node(lit); p->text = "7"; q = p;
    p = new_node(seq); p->first = q; p->next = s; s = p;
    p = new_node(lit); p->text = "6"; q = p;
    p = new_node(seq); p->first = q; p->next = s; s = p;
    p = new_node(lit); p->text = "5"; q = p;
    p = new_node(seq); p->first = q; p->next = s; s = p;
    p = new_node(lit); p->text = "4"; q = p;
    p = new_node(seq); p->first = q; p->next = s; s = p;
    p = new_node(lit); p->text = "3"; q = p;
    p = new_node(seq); p->first = q; p->next = s; s = p;
    p = new_node(lit); p->text = "2"; q = p;
    p = new_node(seq); p->first = q; p->next = s; s = p;
    p = new_node(lit); p->text = "1"; q = p;
    p = new_node(seq); p->first = q; p->next = s; s = p;
    p = new_node(lit); p->text = "0"; q = p;
    p = new_node(seq); p->first = q; p->next = s; s = p;

    p = new_node(alt); p->first = s; q = p;
    p = new_node(type); p->text = "int"; p->next = q; q = p;
    p = new_node(rule); p->text = "Digit"; p->first = q; r = p;

    /* int Decimal ← Digit { match()[0] - '0' } */
    p = new_node(expr); p->text = "match()[0] - '0'"; q = p;
    p = new_node(call); p->text = "Digit"; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "int"; p->next = q; q = p;
    p = new_node(rule); p->text = "Decimal"; p->first = q; p->next = r; r = p;

    /* Primary <- '(' a:Additive ')' -> a / Decimal */
    p = new_node(expr); p->text = "d"; q = p;
    p = new_node(call); p->text = "Decimal"; s = p;
    p = new_node(bind); p->text = "d"; p->first = s; p->next = q; q = p;
    p = new_node(seq); p->first = q; s = p;

    p = new_node(expr); p->text = "a"; q = p;
    p = new_node(lit); p->text = ")"; p->next = q; q = p;
    p = new_node(call); p->text = "Additive"; t = p;
    p = new_node(bind); p->text = "a"; p->first = t; p->next = q; q = p;
    p = new_node(lit); p->text = "("; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;

    p = new_node(alt); p->first = q; q = p;
    p = new_node(type); p->text = "int"; p->next = q; q = p;
    p = new_node(rule); p->text = "Primary"; p->first = q; p->next = r; r = p;

    /* Multitive <- p:Primary '*' m:Multitive { p * m } / Primary */
    p = new_node(expr); p->text = "p"; q = p;
    p = new_node(call); p->text = "Primary"; s = p;
    p = new_node(bind); p->text = "p"; p->first = s; p->next = q; q = p;
    p = new_node(seq); p->first = q; s = p;

    p = new_node(expr); p->text = "p * m"; q = p;
    p = new_node(call); p->text = "Multitive"; t = p;
    p = new_node(bind); p->text = "m"; p->first = t; p->next = q; q = p;
    p = new_node(lit); p->text = "*"; p->next = q; q = p;
    p = new_node(call); p->text = "Primary"; t = p;
    p = new_node(bind); p->text = "p"; p->first = t; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;

    p = new_node(alt); p->first = q; q = p;
    p = new_node(type); p->text = "int"; p->next = q; q = p;
    p = new_node(rule); p->text = "Multitive"; p->first = q; p->next = r; r = p;

    /* Additive <- m:Multitive '+' a:Additive { m + a } / Multitive */
    p = new_node(expr); p->text = "m"; q = p;
    p = new_node(call); p->text = "Multitive"; s = p;
    p = new_node(bind); p->text = "m"; p->first = s; p->next = q; q = p;
    p = new_node(seq); p->first = q; s = p;

    p = new_node(expr); p->text = "m + a"; q = p;
    p = new_node(call); p->text = "Additive"; t = p;
    p = new_node(bind); p->text = "a"; p->first = t; p->next = q; q = p;
    p = new_node(lit); p->text = "+"; p->next = q; q = p;
    p = new_node(call); p->text = "Multitive"; t = p;
    p = new_node(bind); p->text = "m"; p->first = t; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;

    p = new_node(alt); p->first = q; q = p;
    p = new_node(type); p->text = "int"; p->next = q; q = p;
    p = new_node(rule); p->text = "Additive"; p->first = q; p->next = r; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    resolve(p, p);

    return p;
}
