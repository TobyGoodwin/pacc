/*
chars
parse a a
parse ac a
noparse ab A 1
*/

#include <sys/types.h>

#include "syntax.h"

int parse(char *ignore0, off_t ignore1, struct s_node **result) {
    struct s_node *p, *q, *r, *s;

    /* A single character with value:
     *
     * char *A ← 'a' !( 'b' ) { match() }
     *
     */

    p = new_node(expr); p->text = "match()"; q = p;
    p = new_node(lit); p->text = "b"; s = p;
    p = new_node(seq); p->first = s; s = p;
    p = new_node(not); p->first = s; p->next = q; q = p;
    p = new_node(lit); p->text = "a"; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "A"; p->first = q; r = p;

    r = cons(s_text(preamble, 0), r);
    p = new_node(grammar); p->text = "yy"; p->first = r;

    *result = p;
    return 1;
}
