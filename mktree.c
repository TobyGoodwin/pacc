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
