#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "syntax.h"

struct s_node *new_node(enum s_type t) {
    static int id = 100;
    struct s_node *n;
    n = malloc(sizeof *n);
    n->id = id++;
    n->type = t;
    return n;
}

void resolve(struct s_node *g, struct s_node *n) {
    struct s_node *p;
    if (n->type == call && !n->first) {
	struct s_node *i;
	for (i = g->first; i; i = i->next)
	    if (strcmp(i->text, n->text) == 0)
		n->first = i;
	if (!n->first)
	    fprintf(stderr, "rule not found: %s\n", n->text);
    }
    if (s_has_children(n->type))
	for (p = n->first; p; p = p->next)
	    resolve(g, p);
}

#if 0
struct s_node *create(void) {
    struct s_node *p, *q, *r, *s;

    /* A single character with value:
     *
     * A <- '5' { 5 }
     *
     */

    /* A <- 'a' */
    p = new_node(expr); p->text = "5"; q = p;
    p = new_node(lit); p->text = "5"; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(rule); p->text = "A"; p->first = q; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    resolve(p, p);

    return p;
}
#endif

#if 0
struct s_node *create(void) {
    struct s_node *p, *q, *r, *s;

    /* Alternation with value:
     *
     * A <- '5' { 5 } / '6' { 6 }
     *
     */

    p = new_node(expr); p->text = "6"; q = p;
    p = new_node(lit); p->text = "6"; p->next = q; q = p;
    p = new_node(seq); p->first = q; s = p;
    p = new_node(expr); p->text = "5"; q = p;
    p = new_node(lit); p->text = "5"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(rule); p->text = "A"; p->first = q; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    resolve(p, p);

    return p;
}
#endif

#if 1
struct s_node *create(void) {
    struct s_node *p, *q, *r, *s;

    /* Call with value:
     *
     * P <- a:A { a }
     * A <- '5' { 5 } / '6' { 6 }
     *
     */

    p = new_node(expr); p->text = "6"; q = p;
    p = new_node(lit); p->text = "6"; p->next = q; q = p;
    p = new_node(seq); p->first = q; s = p;
    p = new_node(expr); p->text = "5"; q = p;
    p = new_node(lit); p->text = "5"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(rule); p->text = "A"; p->first = q; r = p;

    p = new_node(expr); p->text = "a"; s = p;
    p = new_node(call); p->text = "A"; q = p;
    p = new_node(bind); p->text = "a"; p->first = q; p->next = s; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(rule); p->text = "P"; p->first = q; p->next = r; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    resolve(p, p);

    return p;
}
#endif
