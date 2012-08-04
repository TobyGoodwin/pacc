#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "error.h"
#include "preen.h"
#include "syntax.h"

static void resolve(struct s_node *g, struct s_node *n, struct s_node *from) {
    struct s_node *p;

    if (n->type == rule) from = n;
    if (n->type == call && !n->first) {
	struct s_node *i;
	for (i = g->first; i; i = i->next) {
	    if (i->type != rule) continue;
	    if (strcmp(i->text, n->text) == 0)
		n->first = i;
	}
	if (!n->first)
	    fatal5("rule not found: `", n->text,
		    "' (called from `", from->text, "')");
    }
    if (s_has_children(n->type))
	for (p = n->first; p; p = p->next)
	    resolve(g, p, from);
}

static int consumes(struct s_node *n) {
    struct s_node *p;
    int r;
    //fprintf(stderr, "considering %ld\n", n->id);
    switch (n->type) {
	case rule:
	    if (n->reached) {
		fatal3("left recursion in rule `", n->text, "'");
		//fprintf(stderr, "left recursion in rule `%s'\n", n->text);
		//return 0;
	    }
	    r = 0;
	    n->reached = 1;
	    for (p = n->first; p; p = p->next)
		r += consumes(p);
	    n->reached = 0;
	    return r;
	case lit:
	    return strlen(n->text) > 0;
	case any: case cclass:
	    return 1;
	case rep:
	    return n->text && n->text[0] == '1';
	case call:
	    return consumes(n->first);
	case alt:
	    r = 1;
	    for (p = n->first; p; p = p->next)
		if (!consumes(p)) r = 0;
	    return r;
	default:
	    if (s_has_children(n->type))
		for (p = n->first; p; p = p->next)
		    if (consumes(p)) return 1;
	    return 0;
    }
}

static void check_recursion(struct s_node *g) {
    struct s_node *p;

    for (p = g->first->next; p; p = p->next) {
	assert(p->type == rule);
//	fprintf(stderr, "%ld ", p->id);
	consumes(p);
    }
}

static void check_expression(struct s_node *g) {
    struct s_node *r;
    for (r = g->first->next; r; r = r->next) {
	int p;

	assert(r->type == rule);
	p = path_max(r, expr);
	//fprintf(stderr, "rule %ld %s of type %s: pathmax(expr) %d\n", r->id, r->text, r->first->text, p);
	if (strcmp(r->first->text, "void") == 0) {
	    if (p > 0)
		fatal3("expression in void rule `", r->text, "'");
	} else {
	    if (p < 1)
		fatal3("no expressions in non-void rule `", r->text, "'");
	    if (p > 1)
		fatal3("multiple expressions in rule `", r->text, "'");
	}

    }
}

void preen(struct s_node *g) {
    if (!g->text || g->text[0] == '\0') g->text = "pacc";
    resolve(g, g, 0);
    check_recursion(g);
    check_expression(g);
}
