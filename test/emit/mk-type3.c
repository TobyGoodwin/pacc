/*
type int
parse 78 78
parse 321 321
*/

#include <sys/types.h>

#include "syntax.h"

int pacc_wrap(const char *ign0, char *ign1, off_t ign2, struct s_node **result) {
    struct s_node *p, *q, *r, *s;

    /* Mixed types:
     *
     * Decimal :: int ← d:Digits1 { atoi(ref_ptr(d)) }
     * Digits1 :: ref_t ← Digit Digits1 { ref() } / Digit { ref() }
     * Digit :: void ← '0' / '1' / ... / '9'
     *
     */

    /* void Digit <- '0' / '1' / ... / '9' */
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
    p = new_node(type); p->text = "void"; p->next = q; q = p;
    p = new_node(rule); p->text = "Digit"; p->first = q; r = p;

    /* char *Digits1 ← Digit Digits1 { ref() } / Digit { ref() } */
    p = new_node(expr); p->text = "ref()"; q = p;
    p = new_node(call); p->text = "Digit"; p->next = q; q = p;
    p = new_node(seq); p->first = q; s = p;
    p = new_node(expr); p->text = "ref()"; q = p;
    p = new_node(call); p->text = "Digits1"; p->next = q; q = p;
    p = new_node(call); p->text = "Digit"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(type); p->text = "ref_t"; p->next = q; q = p;
    p = new_node(rule); p->text = "Digits1"; p->first = q; p->next = r; r = p;

    /* int Decimal ← Digits1 { atoi(ref_ptr(d)) } */
    q = s_both(expr, "atoi(ref_ptr(d))", s_text(ident, "d"));
    p = new_node(call); p->text = "Digits1"; s = p;
    p = new_node(bind); p->text = "d"; p->first = s; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "int"; p->next = q; q = p;
    p = new_node(rule); p->text = "Decimal"; p->first = q; p->next = r; r = p;

    r = cons(s_text(preamble, 0), r);
    p = s_kid(grammar, r);

    *result = p;
    return 1;
}
