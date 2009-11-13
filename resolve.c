#include <stdio.h>
#include <string.h>

#include "error.h"
#include "syntax.h"

static void resolve0(struct s_node *g, struct s_node *n) {
    struct s_node *p;
    if (n->type == call && !n->first) {
	struct s_node *i;
	for (i = g->first; i; i = i->next) {
	    if (i->type != rule) continue;
	    if (strcmp(i->text, n->text) == 0)
		n->first = i;
	}
	if (!n->first)
	    fatal3("rule not found: `", n->text, "'");
    }
    if (s_has_children(n->type))
	for (p = n->first; p; p = p->next)
	    resolve0(g, p);
}

void resolve(struct s_node *g) {
    resolve0(g, g);
}
