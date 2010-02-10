/*
chars
parse a a
parse aa aa
parse aaaaa aaaaa
parse aabaa aa
*/

#include <sys/types.h>

#include "syntax.h"

int parse(char *ignore0, off_t ignore1, struct s_node **result) {
    struct s_node *p, *q, *r, *s;

    /* Type char * with call and ref() macro:
     *
     * char *P <- A P { ref_str(ref()) } / ε
     * char *A <- 'a'
     *
     */

    /* A <- 'a' */
    p = new_node(lit); p->text = "a"; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "A"; p->first = q; r = p;

    p = new_node(seq); s = p;
    p = new_node(expr); p->text = "ref_str(ref())"; q = p;
    p = new_node(call); p->text = "P"; p->next = q; q = p;
    p = new_node(call); p->text = "A"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "P"; p->first = q; p->next = r; r = p;

    r = cons(s_text(preamble, 0), r);
    p = s_both(grammar, "yy", r);

    *result = p;
    return 1;
}
