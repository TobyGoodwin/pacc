#include <stdlib.h>

#include "syntax.h"

struct s_node *create(void) {
    int id = 0;
    struct s_node *p, *q, *r;

    q = malloc(sizeof *q);
    q->type = lit;
    q->id = ++id;
    q->text = "y";
    q->next = 0;

    p = malloc(sizeof *p);
    p->type = lit;
    p->id = ++id;
    p->text = "x";
    p->next = q;

    q = malloc(sizeof *q);
    q->type = seq;
    q->id = ++id;
    q->first = p;
    q->next = 0;

    p = malloc(sizeof *p);
    p->type = lit;
    p->id = ++id;
    p->text = "z";
    p->next = q;

    r = malloc(sizeof *r);
    r->type = alt;
    r->id = ++id;
    r->first = p;

    p = malloc(sizeof *p);
    p->type = rule;
    p->id = ++id;
    p->text = "Rule";
    p->first = p->last = r;
    p->next = 0;

    r = malloc(sizeof *r);
    r->type = grammar;
    r->id = ++id;
    r->text = "yy";
    r->first = r->last = p;

    return r;
}
