#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "preen.h"
#include "syntax.h"

/* XXX check_redef is O(n²) */
static void check_redef(struct s_node *g) {
    struct s_node *p, *q;

    for (p = g->first->next; p; p = p->next)
	for (q = g->first->next; q != p; q = q->next)
	    if (strcmp(p->text, q->text) == 0)
		fatal3("rule `", p->text, "' redefined");
}

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

/* Starting from the start rule, we should sooner or later call every rule in
 * the grammar. */
static void reached(struct s_node *n) {
    struct s_node *p;

    if (n->type == rule)
	n->reached = 1;
    if (n->type == call && !n->first->reached)
	reached(n->first);
    if (s_has_children(n->type))
	for (p = n->first; p; p = p->next)
	    reached(p);
}

static void check_reached(struct s_node *g) {
    struct s_node *p;

    for (p = g->first->next; p; p = p->next) {
	assert(p->type == rule);
	p->reached = 0;
    }

    reached(g->first->next);

    for (p = g->first->next; p; p = p->next) {
	assert(p->type == rule);
	if (!p->reached)
	    warn3("rule `", p->text, "' not reached");
	p->reached = 0;
    }
}

/* Every path through a rule must consume some input before that rule is called
 * again, otherwise we have a left recursion. */
static int consumes(struct s_node *n) {
    struct s_node *p;
    int r;

    switch (n->type) {
	case rule:
	    if (n->reached)
		fatal3("left recursion in rule `", n->text, "'");
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
	case and: case not: case guard:
	    return 0;
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

void check_recursion(struct s_node *g) {
    struct s_node *p;

    for (p = g->first->next; p; p = p->next) {
	assert(p->type == rule);
	consumes(p);
    }
}

/* Every path through a non-void rule must include exactly 1 expression. No
 * path through a void rule can include any expressions. */
static void check_expression(struct s_node *g) {
    struct s_node *r;
    for (r = g->first->next; r; r = r->next) {
	int p;

	assert(r->type == rule);
	p = path_max(r, expr);
	if (strcmp(r->first->text, "void") == 0) {
	    if (p > 0)
		fatal3("expression in void rule `", r->text, "'");
	} else {
	    int q = path_min(r, expr);
	    if (p < 1 || q < 1)
		fatal3("no expressions in non-void rule `", r->text, "'");
	    if (p > 1 || q > 1)
		fatal3("multiple expressions in rule `", r->text, "'");
	}

    }
}

/* A name cannot be bound more than once in any scope; each seq represents a
 * single scope. This is O(n^2), but n is the number of bindings in a sequence,
 * which is rarely more than 2. */
static void check_rebound_seq(struct s_node *s) {
    char **names = 0;
    int namea = 0, namep = 0;
    struct s_node *p;

    for (p = s->first; p; p = p->next)
	if (p->type == bind) {
	    int i;
	    for (i = 0; i < namep; ++i)
		if (strcmp(names[i], p->text) == 0) 
		    fatal3("name `", p->text, "' rebound");
	    if (namep == namea) {
		int l = 2 * namea + 1;
		char **n = realloc(names, l * sizeof *n);
		if (!n) nomem();
		names = n;
		namea = l;
	    }
	    names[namep++] = p->text;
	}
}

static void check_rebound(struct s_node *n) {
    struct s_node *p;
    if (n->type == seq)
	    check_rebound_seq(n);
    if (s_has_children(n->type))
	for (p = n->first; p; p = p->next)
	    check_rebound(p);
}

void preen(struct s_node *g, char *name) {
    assert(g && !g->text);
    g->text = name;
    resolve(g, g, 0);
    check_redef(g);
    check_reached(g);
    check_recursion(g);
    check_expression(g);
    check_rebound(g);
}
