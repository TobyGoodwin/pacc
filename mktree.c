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

#include "mk-target.c"

#if 0
#endif

#if 0
#endif

#if 0
#endif

#if 0
#endif

#if 0
struct s_node *create(void) {
    struct s_node *p, *q, *r, *s, *t;

    /* Non-linear, from Bryan Ford (personal communication):
     *
     * S <- A S / a:A -> a
     * A <- a S b -> 2 / a S c -> 3 / a -> 1
     *
     */

    p = new_node(expr); p->text = "1"; q = p;
    p = new_node(lit); p->text = "a"; p->next = q; q = p;
    p = new_node(seq); p->first = q; s = p;
    p = new_node(expr); p->text = "3"; q = p;
    p = new_node(lit); p->text = "c"; p->next = q; q = p;
    p = new_node(call); p->text = "A"; p->next = q; q = p;
    p = new_node(lit); p->text = "a"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; s = p;
    p = new_node(expr); p->text = "2"; q = p;
    p = new_node(lit); p->text = "b"; p->next = q; q = p;
    p = new_node(call); p->text = "A"; p->next = q; q = p;
    p = new_node(lit); p->text = "a"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(rule); p->text = "A"; p->first = q; r = p;

    p = new_node(expr); p->text = "a"; q = p;
    p = new_node(call); p->text = "A"; t = p;
    p = new_node(bind); p->text = "a"; p->first = t; p->next = q; q = p;
    p = new_node(seq); p->first = q; s = p;
    p = new_node(call); p->text = "S"; q = p;
    p = new_node(call); p->text = "A"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; s = p;
    p = new_node(alt); p->first = s; q = p;
    p = new_node(rule); p->text = "S"; p->first = q; p->next = r; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    resolve(p, p);

    return p;
}
#endif
