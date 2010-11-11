/*
type chars
parse a a
parse 'a b' 'a b'
parse 'a  b' 'a  b'
*/

#include <sys/types.h>

#include "syntax.h"

int parse(const char *ign0, char *ign1, off_t ign2, struct s_node **result) {
    struct s_node *p, *q, *r, *s;

    /* Nested repetition:
     *
     * char *Atoms ← ( Atom Space ) * → { ref_str() }
     * Atom ← Letter +
     * Letter ← 'a' / 'b' / 'c'
     * Space ← ' ' *
     *
     */

    p = new_node(lit); p->text = " "; s = p;
    p = new_node(rep); p->first = s; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "Space"; p->first = q; r = p;

    p = new_node(lit); p->text = "c"; q = p;
    p = new_node(seq); p->first = q; s = p;
    p = new_node(lit); p->text = "b"; q = p;
    p = new_node(seq); p->first = q; p->next = s; s = p;
    p = new_node(lit); p->text = "a"; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "Letter"; p->first = q; p->next = r; r = p;

    p = new_node(expr); p->text = "ref_str()"; q = p;
    p = new_node(call); p->text = "Letter"; s = p;
    p = s_text(rep, "1,"); p->first = s; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "Atom"; p->first = q; p->next = r; r = p;

    p = new_node(expr); p->text = "ref_str()"; q = p;
    p = new_node(call); p->text = "Space"; s = p;
    p = new_node(call); p->text = "Atom"; p->next = s; s = p;
    p = new_node(seq); p->first = s; s = p;
    p = s_new(rep); p->first = s; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "Atoms"; p->first = q; p->next = r; r = p;

    r = cons(s_text(preamble, 0), r);
    p = s_both(grammar, "yy", r);

    *result = p;
    return 1;
}
