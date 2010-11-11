/*
type int
check 5 'hello, world!parsed with value 5'
noparse x A 1
*/

#include <sys/types.h>

#include "syntax.h"

int parse(const char *ign0, char *ign1, off_t ign2, struct s_node **result) {
    struct s_node *p, *q, *r, *s;

    /* A semantic predicate, with no binding
     *
     * A <- '5' &{ printf("hello, world!") } { 5 }
     *
     */

    /* A <- 'a' */
    p = new_node(expr); p->text = "5"; q = p;
    p = new_node(guard); p->text = "printf(\"hello, world!\")"; p->next = q; q = p;
    p = new_node(lit); p->text = "5"; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "int"; p->next = q; q = p;
    p = new_node(rule); p->text = "A"; p->first = q; r = p;

    r = cons(s_text(preamble, 0), r);
    p = new_node(grammar); p->text = "yy"; p->first = r;

    *result = p;
    return 1;
}
