#include "syntax.h"

char *prefix = 0;

struct s_node *create(void) {
    struct s_node *p, *q, *r, *s;

    /* Nested repetition:
     *
     * char *Atoms ← ( Atom Space ) * → { match() }
     * Atom ← Letter +
     * Letter ← 'a' / 'b' / 'c'
     * Space ← ' ' *
     *
     */

    p = new_node(lit); p->text = " "; s = p;
    p = new_node(rep); p->number = 0; p->first = s; q = p;
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

    p = new_node(expr); p->text = "match()"; q = p;
    p = new_node(call); p->text = "Letter"; s = p;
    p = new_node(rep); p->number = 0x10000; p->first = s; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "Atom"; p->first = q; p->next = r; r = p;

#if 0
    p = new_node(expr); p->text = "match()"; q = p;
    p = new_node(call); p->text = "Space"; s = p;
    p = new_node(call); p->text = "Atom"; p->next = s; s = p;
    p = new_node(seq); p->first = s; p->next = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "Atoms"; p->first = q; p->next = r; r = p;
#endif

    p = new_node(expr); p->text = "match()"; q = p;
    p = new_node(call); p->text = "Space"; s = p;
    p = new_node(call); p->text = "Atom"; p->next = s; s = p;
    p = new_node(seq); p->first = s; s = p;
    p = new_node(rep); p->number = 0; p->first = s; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "Atoms"; p->first = q; p->next = r; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    resolve(p, p);

    return p;
}
