/*
int
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

int pacc_wrap(const char *ign0, char *ign1, off_t ign2, struct s_node **result) {
    struct s_node *p, *q, *r, *s, *t;

    /* Bryan Ford's trivial grammar with actions:
     *
     * Additive <- m:Multitive '+' a:Additive { m + a } / Multitive
     * Multitive <- p:Primary '*' m:Multitive { p * m } / Primary
     * Primary <- '(' a:Additive ')' -> a / Decimal
     * Decimal <- '0' { 0 } / '1' { 1 } / ... / '9'
     */

    /* Decimal <- '0' { 0 } / '1' { 1 } / ... / '9'{ 9 }  */
    p = s_new(expr); p->text = "9"; q = p;
    p = s_new(lit); p->text = "9"; p->next = q; q = p;
    p = s_new(seq); p->first = q; s = p;

    p = s_new(expr); p->text = "8"; q = p;
    p = s_new(lit); p->text = "8"; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = s; s = p;

    p = s_new(expr); p->text = "7"; q = p;
    p = s_new(lit); p->text = "7"; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = s; s = p;

    p = s_new(expr); p->text = "6"; q = p;
    p = s_new(lit); p->text = "6"; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = s; s = p;

    p = s_new(expr); p->text = "5"; q = p;
    p = s_new(lit); p->text = "5"; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = s; s = p;

    p = s_new(expr); p->text = "4"; q = p;
    p = s_new(lit); p->text = "4"; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = s; s = p;

    p = s_new(expr); p->text = "3"; q = p;
    p = s_new(lit); p->text = "3"; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = s; s = p;

    p = s_new(expr); p->text = "2"; q = p;
    p = s_new(lit); p->text = "2"; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = s; s = p;

    p = s_new(expr); p->text = "1"; q = p;
    p = s_new(lit); p->text = "1"; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = s; s = p;

    p = s_new(expr); p->text = "0"; q = p;
    p = s_new(lit); p->text = "0"; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = s; s = p;

    p = s_new(alt); p->first = s; q = p;
    p = s_new(type); p->text = "int"; p->next = q; q = p;
    p = s_new(rule); p->text = "Decimal"; p->first = q; p->next = 0; r = p;

    /* Primary <- '(' a:Additive ')' -> a / Decimal */
    p = s_new(ident); p->text = "d"; t = p;
    p = s_new(expr); p->text = "d"; p->first = t; q = p;
    p = s_new(call); p->text = "Decimal"; s = p;
    p = s_new(bind); p->text = "d"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; s = p;

    p = s_new(ident); p->text = "a"; t = p;
    p = s_new(expr); p->text = "a"; p->first = t; q = p;
    p = s_new(lit); p->text = ")"; p->next = q; q = p;
    p = s_new(call); p->text = "Additive"; t = p;
    p = s_new(bind); p->text = "a"; p->first = t; p->next = q; q = p;
    p = s_new(lit); p->text = "("; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = s; q = p;

    p = s_new(alt); p->first = q; q = p;
    p = s_new(type); p->text = "int"; p->next = q; q = p;
    p = s_new(rule); p->text = "Primary"; p->first = q; p->next = r; r = p;

    /* Multitive <- p:Primary '*' m:Multitive { p * m } / Primary */
    p = s_new(ident); p->text = "p"; t = p;
    p = s_new(expr); p->text = "p"; p->first = t; q = p;
    p = s_new(call); p->text = "Primary"; s = p;
    p = s_new(bind); p->text = "p"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; s = p;

    p = s_new(ident); p->text = "p"; t = p;
    p = s_new(ident); p->text = "m"; p->next = t; t = p;
    p = s_new(expr); p->text = "p * m"; p->first = t; q = p;
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
    p = s_new(ident); p->text = "m"; t = p;
    p = s_new(expr); p->text = "m"; p->first = t; q = p;
    p = s_new(call); p->text = "Multitive"; s = p;
    p = s_new(bind); p->text = "m"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; s = p;

    p = s_new(ident); p->text = "m"; t = p;
    p = s_new(ident); p->text = "a"; p->next = t; t = p;
    p = s_new(expr); p->text = "m + a"; p->first = t; q = p;
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
    p = s_kid(grammar, r);

    *result = p;
    return 1;
}
