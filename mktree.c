#include <stdlib.h>

#include "syntax.h"

struct s_node *create(void) {
    int id = 0;
    struct s_node *p, *q, *r;

    p = malloc(sizeof *p);
    p->type = lit;
    p->id = ++id;
    p->text = "x";
    
    q = malloc(sizeof *q);
    q->type = rule;
    q->id = ++id;
    q->text = "Rule";
    q->first = q->last = p;
    q->next = 0;

    r = malloc(sizeof *r);
    r->type = grammar;
    r->id = ++id;
    r->text = "yy";
    r->first = r->last = q;

    return r;
}
