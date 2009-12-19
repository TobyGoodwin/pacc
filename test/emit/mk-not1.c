/*
chars
noparse f "o" 1
noparse fo "o" 2
parse foo yes
noparse food S 3
*/

#include <sys/types.h>

#include "syntax.h"

int parse(char *ignore0, off_t ignore1, struct s_node **result) {
    struct s_node *p, *q, *r, *s;

    /* End of input:
     *
     * S ← "foo" !. { "yes" }
     *
     */

    p = new_node(expr); p->text = "\"yes\""; q = p;
    p = new_node(any); s = p;
    p = new_node(not); p->first = s; p->next = q; q = p;
    p = new_node(lit); p->text = "o"; p->next = q; q = p;
    p = new_node(lit); p->text = "o"; p->next = q; q = p;
    p = new_node(lit); p->text = "f"; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "S"; p->first = q; r = p;

    r = cons(s_text(preamble, 0), r);
    p = new_node(grammar); p->text = "yy"; p->first = r;

    *result = p;
    return 1;
}
