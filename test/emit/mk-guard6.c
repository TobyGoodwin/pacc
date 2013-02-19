/*
type chars
parse q q
parse qux qux
parse q39a q39a
parse q. q
parse q3. q3
noparse 37 Identifier 1
*/

#include <sys/types.h>

#include "syntax.h"

/*
 * Identifier :: char * ← i:(IdentStart IdentCont*) .* { ref_dup(i) } 
 * void IdentStart ← c:Char &{ *c >= 'a' && *c <= 'z' }
 * void IdentCont ← IdentStart / d:Char &{ *d >= '0' && *d <= '9' }
 * *Char :: ref_t ← . { ref_str() }
 */

int pacc_wrap(const char *ign0, char *ign1, off_t ign2, struct s_node **result) {
    struct s_node *p, *q, *r, *s, *t;

    /* char *Char ← . { ref_str() } */
    p = new_node(expr); p->text = "ref_str()"; q = p;
    p = new_node(any); p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "Char"; p->first = q; r = p;

    /* void IdentCont ← IdentStart / d:Char &{ *d >= '0' && *d <= '9' } */
    p = new_node(ident); p->text = "d"; s = p;
    p = new_node(guard); p->text = "*d >= '0' && *d <= '9'"; p->first = s; q = p;
    p = new_node(call); p->text = "Char"; s = p;
    p = new_node(bind); p->text = "d"; p->first = s; p->next = q; q = p;
    p = new_node(seq); p->first = q; s = p;
    p = new_node(call); p->text = "IdentStart"; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(type); p->text = "void"; p->next = q; q = p;
    p = new_node(rule); p->text = "IdentCont"; p->first = q; p->next = r; r = p;

    /* void IdentStart ← c:Char &{ *c >= 'a' && *c <= 'z' } */
    p = new_node(ident); p->text = "c"; s = p;
    p = new_node(guard); p->text = "*c >= 'a' && *c <= 'z'"; p->first = s; q = p;
    p = new_node(call); p->text = "Char"; s = p;
    p = new_node(bind); p->text = "c"; p->first = s; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "void"; p->next = q; q = p;
    p = new_node(rule); p->text = "IdentStart"; p->first = q; p->next = r; r = p;

    /* Identifier :: char * ← i:(IdentStart IdentCont*) .* { ref_dup(i) } */
    p = s_both(expr, "ref_dup(i)", s_text(ident, "i"));
    p = cons(s_both(rep, 0, s_new(any)), p);
    q = s_both(rep, 0, s_text(call, "IdentCont"));
    q = cons(s_text(call, "IdentStart"), q);
    q = s_both(bind, "i", s_kid(seq, q));
    p = cons(q, p);
    p = cons(s_text(type, "char *"), s_kid(seq, p));
    p = s_both(rule, "Identifier", p);
    r = cons(p, r);

    r = cons(s_text(preamble, 0), r);
    p = s_kid(grammar, r);

    *result = p;
    return 1;
}
