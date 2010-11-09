/*
type: int
parse 5 5
parse 2+3 5
parse 2*3 6
parse 2+3+4 9
parse 2*3+4 10
parse 2+3*4 14
parse 2*3*4 24
noparse x Additive 1
*/

#include <sys/types.h>

#include "syntax.h"

int parse(const char *ign0, char *ign1, off_t ign2, struct s_node **result) {
    struct s_node *i, *p, *q, *r, *s, *t;

    /* Bryan Ford's trivial grammar with ref_ptr():
     *
     * Additive <- m:Multitive '+' a:Additive { m + a } / Multitive
     * Multitive <- p:Primary '*' m:Multitive { p * m } / Primary
     * Primary <- '(' a:Additive ')' -> a / Decimal
     * int Decimal <- d:Digit { *ref_ptr(d) - '0' }
     * ref_t Digit <- ('0' / '1' / ... / '9') { ref() }
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

    p = cons(s_kid(alt, p), s_text(expr, "ref()"));
    p = s_kid(seq, p);
    p = cons(s_text(type, "ref_t"), p);
    p = s_both(rule, "Digit", p); r = p;

    /* int Decimal <- d:Digit { *ref_ptr(d) - '0' } */
    p = s_both(expr, "*ref_ptr(d) - '0'", s_text(ident, "d")); q = p;
    p = s_both(bind, "d", s_text(call, "Digit")); p->next = q; q = p;
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

    r = cons(s_text(preamble, 0), r);
    p = s_new(grammar); p->text = "yy"; p->first = r;

    *result = p;
    return 1;
}
