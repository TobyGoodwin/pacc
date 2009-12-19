/*
chars
parse 567 7
*/

#include <sys/types.h>

#include "syntax.h"

int parse(char *ignore0, off_t ignore1, struct s_node **result) {
     struct s_node *p, *q, *r, *s;

    /* Calls to rmatch() can occur anywhere:
     *
     * char *A <- '5' '6' { rmatch() } '7'
     *
     */

    /* A <- 'a' */
    p = new_node(lit); p->text = "7"; q = p;
    p = new_node(expr); p->text = "rmatch()"; p->next = q; q = p;
    p = new_node(lit); p->text = "6"; p->next = q; q = p;
    p = new_node(lit); p->text = "5"; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "A"; p->first = q; r = p;

    r = cons(s_text(preamble, 0), r);
    p = new_node(grammar); p->text = "yy"; p->first = r;

    *result = p;
    return 1;
}
