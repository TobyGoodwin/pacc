#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "syntax.h"

void nomem(void) {
    fprintf(stderr, "fatal: out of memory\n");
    exit(1);
}

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

struct s_node *s_rule(void) {
    return 0;
}

struct s_node *s_rule_cons(struct s_node *car, struct s_node *cdr) {
    car->next = cdr;
    return car;
}

struct s_node *s_query(void) {
    return 0;
}

struct s_node *s_star(void) {
    return 0;
}

struct s_node *s_plus(void) {
    return 0;
}

struct s_node *s_call(char *n) {
    struct s_node *r = s_new(call);
    r->text = n;
    printf("s_call(\"%s\")\n", n);
    return r;
}

struct s_node *s_lit(void) {
    return 0;
}

struct s_node *s_bind(char *n, struct s_node *c) {
    struct s_node *r = s_text(bind, n);
    r->first = c;
    return r;
}

struct s_node *s_and(void) {
    return 0;
}

struct s_node *s_not(void) {
    return 0;
}

struct s_node *s_guard(void) {
    return 0;
}

static char *t = "int";

char *s_stash_type(char *type) {
    while (*type && (*type == ':' || *type == ' '))
	++type;
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
	t == guard || t == ident || t == lit || t == rep ||
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
