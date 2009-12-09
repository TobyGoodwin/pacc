#include "syntax.h"

char *prefix = 0;

struct s_node *create(void) {
    struct s_node *i, *p, *q, *r, *s, *t;

    /* Bryan Ford's trivial grammar with integers and spacing:
     *
     * int Sum ← Space a:Additive End → a
     * Additive <- m:Multitive Plus a:Additive { m + a } / Multitive
     * Multitive <- p:Primary Times m:Multitive { p * m } / Primary
     * Primary <- Left a:Additive Right -> a / Decimal
     * Decimal ← Digits1 { atoi(match()) } Space
     * Digits1 ← Digit Digits1 / Digit
     * Digit <- '0' / '1' / ... / '9'
     * void Left ← '(' Space
     * Right ← ')' Space
     * Plus ← '+' Space
     * Times ← '*' Space
     * Space ← ' ' Space / ε
     * End ← ! .
     */

    /* void End ← ! . */
    p = s_new(any); q = p;
    p = s_new(not); p->first = q; q = p;
    p = s_new(type); p->text = "int"; p->next = q; q = p;
    p = s_new(rule); p->text = "End"; p->first = q; r = p;

    /* void Space ← ' ' Space / ε */
    p = s_new(seq); s = p;
    p = s_new(call); p->text = "Space"; q = p;
    p = s_new(lit); p->text = " "; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = s; q = p;
    p = s_new(alt); p->first = q;
    p = s_new(type); p->text = "int"; p->next = q; q = p;
    p = s_new(rule); p->text = "Space"; p->first = q; p->next = r; r = p;

    /* void Times ← '*' Space */
    p = s_new(call); p->text = "Space"; q = p;
    p = s_new(lit); p->text = "*"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "int"; p->next = q; q = p;
    p = s_new(rule); p->text = "Times"; p->first = q; p->next = r; r = p;

    /* void Plus ← '+' Space */
    p = s_new(call); p->text = "Space"; q = p;
    p = s_new(lit); p->text = "+"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "int"; p->next = q; q = p;
    p = s_new(rule); p->text = "Plus"; p->first = q; p->next = r; r = p;

    /* void Left ← '(' Space */
    p = s_new(call); p->text = "Space"; q = p;
    p = s_new(lit); p->text = "("; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "int"; p->next = q; q = p;
    p = s_new(rule); p->text = "Left"; p->first = q; p->next = r; r = p;

    /* void Right ← ')' Space */
    p = s_new(call); p->text = "Space"; q = p;
    p = s_new(lit); p->text = ")"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "int"; p->next = q; q = p;
    p = s_new(rule); p->text = "Right"; p->first = q; p->next = r; r = p;

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
    p = s_new(rule); p->text = "Digit"; p->first = q; p->next = r; r = p;

    /* int Digits1 ← Digit Digits1 / Digit */
    p = s_new(call); p->text = "Digit"; s = p;
    p = s_new(call); p->text = "Digits1"; q = p;
    p = s_new(call); p->text = "Digit"; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = s; q = p;
    p = s_new(alt); p->first = q; q = p;
    p = s_new(type); p->text = "int"; p->next = q; q = p;
    p = s_new(rule); p->text = "Digits1"; p->first = q; p->next = r; r = p;

    /* int Decimal ← Digits1 { atoi(match()) } Space */
    p = s_new(call); p->text = "Space"; q = p;
    p = s_new(expr); p->text = "atoi(match())"; p->next = q; q = p;
    p = s_new(call); p->text = "Digits1"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "int"; p->next = q; q = p;
    p = s_new(rule); p->text = "Decimal"; p->first = q; p->next = r; r = p;

    /* int Primary <- Left a:Additive Right -> a / Decimal */
    p = s_new(ident); p->text = "d"; i = p;
    p = s_new(expr); p->text = "d"; p->first = i; q = p;
    p = s_new(call); p->text = "Decimal"; s = p;
    p = s_new(bind); p->text = "d"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; s = p;

    p = s_new(ident); p->text = "a"; i = p;
    p = s_new(expr); p->text = "a"; p->first = i; q = p;
    p = s_new(call); p->text = "Right"; p->next = q; q = p;
    p = s_new(call); p->text = "Additive"; t = p;
    p = s_new(bind); p->text = "a"; p->first = t; p->next = q; q = p;
    p = s_new(call); p->text = "Left"; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = s; q = p;

    p = s_new(alt); p->first = q; q = p;
    p = s_new(type); p->text = "int"; p->next = q; q = p;
    p = s_new(rule); p->text = "Primary"; p->first = q; p->next = r; r = p;

    /* Multitive <- p:Primary Times m:Multitive { p * m } / Primary */
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
    p = s_new(call); p->text = "Times"; p->next = q; q = p;
    p = s_new(call); p->text = "Primary"; t = p;
    p = s_new(bind); p->text = "p"; p->first = t; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = s; q = p;

    p = s_new(alt); p->first = q; q = p;
    p = s_new(type); p->text = "int"; p->next = q; q = p;
    p = s_new(rule); p->text = "Multitive"; p->first = q; p->next = r; r = p;

    /* Additive <- m:Multitive Plus a:Additive { m + a } / Multitive */
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
    p = s_new(call); p->text = "Plus"; p->next = q; q = p;
    p = s_new(call); p->text = "Multitive"; t = p;
    p = s_new(bind); p->text = "m"; p->first = t; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = s; q = p;

    p = s_new(alt); p->first = q; q = p;
    p = s_new(type); p->text = "int"; p->next = q; q = p;
    p = s_new(rule); p->text = "Additive"; p->first = q; p->next = r; r = p;

    /* int Sum ← Space a:Additive → a */
    p = s_new(ident); p->text = "a"; i = p;
    p = s_new(expr); p->text = "a"; p->first =i; q = p;
    p = s_new(call); p->text = "End"; p->next = q; q = p;
    p = s_new(call); p->text = "Additive"; t = p;
    p = s_new(bind); p->text = "a"; p->first = t; p->next = q; q = p;
    p = s_new(call); p->text = "Space"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "int"; p->next = q; q = p;
    p = s_new(rule); p->text = "Sum"; p->first = q; p->next = r; r = p;

    p = s_new(grammar); p->text = "yy"; p->first = r;

    return p;
}