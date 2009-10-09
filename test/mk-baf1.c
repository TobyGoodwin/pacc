#include "syntax.h"

char *prefix = 0;

struct s_node *create(void) {
    struct s_node *i, *p, *q, *r, *s, *t;

    /* Bryan Ford's trivial grammar with match():
     *
     * Additive <- m:Multitive '+' a:Additive { m + a } / Multitive
     * Multitive <- p:Primary '*' m:Multitive { p * m } / Primary
     * Primary <- '(' a:Additive ')' -> a / Decimal
     * int Decimal <- Digit { match()[0] - '0' }
     * int Digit <- '0' / '1' / ... / '9'
     */

    /* int Digit <- '0' / '1' / ... / '9' */
    p = s_new(lit); p->text = "9"; q = p;
    p = s_new(seq); p->first = q; s = p;
    p = s_new(lit); p->text = "8"; q = p;
    p = s_new(seq); p->first = q; p->next = s; s = p;
    p = s_new(lit); p->text = "7"; q = p;
    p = s_new(seq); p->first = q; p->next = s; s = p;
    p = s_new(lit); p->text = "6"; q = p;
    p = s_new(seq); p->first = q; p->next = s; s = p;
    p = s_new(lit); p->text = "5"; q = p;
    p = s_new(seq); p->first = q; p->next = s; s = p;
    p = s_new(lit); p->text = "4"; q = p;
    p = s_new(seq); p->first = q; p->next = s; s = p;
    p = s_new(lit); p->text = "3"; q = p;
    p = s_new(seq); p->first = q; p->next = s; s = p;
    p = s_new(lit); p->text = "2"; q = p;
    p = s_new(seq); p->first = q; p->next = s; s = p;
    p = s_new(lit); p->text = "1"; q = p;
    p = s_new(seq); p->first = q; p->next = s; s = p;
    p = s_new(lit); p->text = "0"; q = p;
    p = s_new(seq); p->first = q; p->next = s; s = p;

    p = s_new(alt); p->first = s; q = p;
    p = s_new(type); p->text = "int"; p->next = q; q = p;
    p = s_new(rule); p->text = "Digit"; p->first = q; r = p;

    /* int Decimal ← Digit { match()[0] - '0' } */
    p = s_new(expr); p->text = "match()[0] - '0'"; q = p;
    p = s_new(call); p->text = "Digit"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "int"; p->next = q; q = p;
    p = s_new(rule); p->text = "Decimal"; p->first = q; p->next = r; r = p;

    /* Primary <- '(' a:Additive ')' -> a / Decimal */
    p = s_new(ident); p->text = "d"; i = p;
    p = s_new(expr); p->text = "d"; p->first = i; q = p;
    p = s_new(call); p->text = "Decimal"; s = p;
    p = s_new(bind); p->text = "d"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; s = p;

    p = s_new(ident); p->text = "a"; i = p;
    p = s_new(expr); p->text = "a"; p->first = i; q = p;
    p = s_new(lit); p->text = ")"; p->next = q; q = p;
    p = s_new(call); p->text = "Additive"; t = p;
    p = s_new(bind); p->text = "a"; p->first = t; p->next = q; q = p;
    p = s_new(lit); p->text = "("; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = s; q = p;

    p = s_new(alt); p->first = q; q = p;
    p = s_new(type); p->text = "int"; p->next = q; q = p;
    p = s_new(rule); p->text = "Primary"; p->first = q; p->next = r; r = p;

    /* Multitive <- p:Primary '*' m:Multitive { p * m } / Primary */
    p = s_new(ident); p->text = "p"; i = p;
    p = s_new(expr); p->text = "p"; p->first = i; q = p;
    p = s_new(call); p->text = "Primary"; s = p;
    p = s_new(bind); p->text = "p"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; s = p;

    p = s_new(ident); p->text = "m"; i = p;
    p = s_new(ident); p->text = "p"; p->next = i; i = p;
    p = s_new(expr); p->text = "p * m"; p->first = i; q = p;
    p = s_new(call); p->text = "Multitive"; t = p;
    p = s_new(bind); p->text = "m"; p->first = t; p->next = q; q = p;
    p = s_new(lit); p->text = "*"; p->next = q; q = p;
    p = s_new(call); p->text = "Primary"; t = p;
    p = s_new(bind); p->text = "p"; p->first = t; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = s; q = p;

    p = s_new(alt); p->first = q; q = p;
    p = s_new(type); p->text = "int"; p->next = q; q = p;
    p = s_new(rule); p->text = "Multitive"; p->first = q; p->next = r; r = p;

    /* Additive <- m:Multitive '+' a:Additive { m + a } / Multitive */
    p = s_new(ident); p->text = "m"; i = p;
    p = s_new(expr); p->text = "m"; p->first = i; q = p;
    p = s_new(call); p->text = "Multitive"; s = p;
    p = s_new(bind); p->text = "m"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; s = p;

    p = s_new(ident); p->text = "a"; i = p;
    p = s_new(ident); p->text = "m"; p->next = i; i = p;
    p = s_new(expr); p->text = "m + a"; p->first = i; q = p;
    p = s_new(call); p->text = "Additive"; t = p;
    p = s_new(bind); p->text = "a"; p->first = t; p->next = q; q = p;
    p = s_new(lit); p->text = "+"; p->next = q; q = p;
    p = s_new(call); p->text = "Multitive"; t = p;
    p = s_new(bind); p->text = "m"; p->first = t; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = s; q = p;

    p = s_new(alt); p->first = q; q = p;
    p = s_new(type); p->text = "int"; p->next = q; q = p;
    p = s_new(rule); p->text = "Additive"; p->first = q; p->next = r; r = p;

    p = s_new(grammar); p->text = "yy"; p->first = r;

    resolve(p, p);

    return p;
}
