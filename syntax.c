#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "syntax.h"

struct s_node *s_new(enum s_type t) {
    static int id = 100;
    struct s_node *n;
    n = malloc(sizeof *n);
    if (!n) nomem();
    n->id = id++; n->type = t;
    n->first = n->last = n->next = 0;
    n->text = 0;
    return n;
}

struct s_node *s_text(enum s_type t, char *n) {
    struct s_node *r = s_new(t);
    r->text = n;
    return r;
}

struct s_node *s_kid(enum s_type t, struct s_node *n) {
    struct s_node *r = s_new(t);
    r->first = n;
    return r;
}

struct s_node *s_both(enum s_type t, char *n, struct s_node *s) {
    struct s_node *r = s_new(t);
    if (t == bind) fprintf(stderr, "s_bind(%s, [%s])\n", n, s->text);
    r->text = n;
    r->first = s;
    return r;
}

/* s_retype(): destructively change the type of a node */
struct s_node *s_retype(enum s_type t, struct s_node *s) {
    s->type = t;
    return s;
}

struct s_node *s_grammar(char *preamble, struct s_node *defns) {
    struct s_node *r = s_new(grammar);
    r->text = preamble;
    r->first = defns;
    return r;
}

struct s_node *s_rule_cons(struct s_node *car, struct s_node *cdr) {
    car->next = cdr;
    return car;
}

#if 0
struct s_node *s_bind(char *n, struct s_node *c) {
    struct s_node *r = s_text(bind, n);
    r->first = c;
    return r;
}
#else
struct s_node *s_bind(char *n, struct s_node *c) {
    //fprintf(stderr, "s_bind(%s, [%s])\n", n, c->text);
    return s_both(bind, n, c);
}
#endif

struct s_node *s_alt(struct s_node *s, struct s_node *r) {
    if (0 && r->type == alt) {
	assert(!r->next);
	r->first = cons(s, r->first);
	return r;
    }
    return s_kid(alt, cons(s, r));
}

static char *t = "int";

char *s_stash_type(char *type) {
    char *s;

    /* XXX can we really not strip the string in the grammar? */
    while (*type && (*type == ':' || *type == ' '))
	++type;
    s = type + strlen(type) - 1;
    while (*s == ' ' || *s == '\t' || *s == '\n') {
	*s = '\0';
	--s;
    }

    /* XXX we need to handle void correctly */
    if (strcmp(type, "void") == 0) type = "int";

    t = type;
    return type;
}

char *s_stashed_type(void) {
    return t;
}

struct s_node *mkalt(struct s_node *l) {
    struct s_node *r;
    r = s_new(alt);
    if (!r) nomem();
    r->first = l;
    return r;
}

struct s_node *mkcall(char *name) {
    struct s_node *r;
    r = s_new(call);
    if (!r) nomem();
    r->text = name;
    return r;
}

struct s_node *mklit(char *t) {
    struct s_node *r;
    r = s_new(lit);
    if (!r) nomem();
    r->text = t;
    return r;
}

struct s_node *mkrule(char *name, struct s_node *what) {
    struct s_node *r;
    r = s_new(rule);
    if (!r) nomem();
    r->text = name;
    r->first = what;
    return r;
}

struct s_node *mkseq(struct s_node *l) {
    struct s_node *r;
    r = s_new(seq);
    if (!r) nomem();
    r->first = l;
    return r;
}

struct s_node *cons(struct s_node *a, struct s_node *d) {
    a->next = d;
    return a;
}

struct s_node *snoc(struct s_node *l, struct s_node *a) {
    struct s_node *p, *q;
    if (!l) return a;
    for (p = l; p; p = p->next)
	q = p;
    q->next = a;
    return l;
}

/* cons uniquely */
struct s_node *s_set_cons(struct s_node *i, struct s_node *l) {
    struct s_node *p;
    for (p = l; p; p = p->next)
	if (strcmp(p->text, i->text) == 0)
	    return l;
    return cons(i, l);
}

char *decode_type(enum s_type t) {
    switch(t) {
    case alt:		return "alt";
    case and:		return "and";
    case any:		return "any";
    case bind:		return "bind";
    case call:		return "call";
    case expr:		return "expr";
    case grammar:	return "grammar";
    case guard:		return "guard";
    case ident:		return "ident";
    case lit:		return "lit";
    case not:		return "not";
    case preamble:	return "preamble";
    case rep:		return "rep";
    case rule:		return "rule";
    case seq:		return "seq";
    case type:		return "type";
    default:		return "[unknown]";
    }
}

int s_has_children(enum s_type t) {
    return t == alt || t == and || t == bind || t == rule || t == grammar ||
	t == expr || t == guard || t == not || t == rep || t == seq;
}

int s_has_text(enum s_type t) {
    return t == bind || t == call || t == expr || t == grammar ||
	t == guard || t == ident || t == lit || t == preamble || t == rep ||
	t == rule || t == type;
}

static void dump(struct s_node *p, int indent) {
    int i;

    if (!p) return;
    for (i = 0; i < indent; ++i) fprintf(stderr, "  ");
    fprintf(stderr, "%s %d: ", decode_type(p->type), p->id);
    if (s_has_text(p->type))
	fprintf(stderr, "%s ", p->text);
    fprintf(stderr, "\n");

    if (s_has_children(p->type))
	for (p = p->first; p; p = p->next)
	    dump(p, indent + 1);

    return;
}

void s_dump(struct s_node *p) {
    dump(p, 0);
}
