#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "syntax.h"

struct s_node *new_node(enum s_type t) {
    static int id = 0;
    struct s_node *n;
    n = malloc(sizeof *n);
    n->id = id++;
    n->type = t;
    return n;
}

void resolve(struct s_node *g, struct s_node *n) {
    struct s_node *p;
    if (n->type == rule && !n->first) {
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
        
struct s_node *create(void) {
    struct s_node *p, *q, *r, *s;

    /* Decimal = '0' / '1' / ... / '9' */
    p = new_node(lit); p->text = "9"; p->next = 0; q = p;
    p = new_node(lit); p->text = "8"; p->next = q; q = p;
    p = new_node(lit); p->text = "7"; p->next = q; q = p;
    p = new_node(lit); p->text = "6"; p->next = q; q = p;
    p = new_node(lit); p->text = "5"; p->next = q; q = p;
    p = new_node(lit); p->text = "4"; p->next = q; q = p;
    p = new_node(lit); p->text = "3"; p->next = q; q = p;
    p = new_node(lit); p->text = "2"; p->next = q; q = p;
    p = new_node(lit); p->text = "1"; p->next = q; q = p;
    p = new_node(lit); p->text = "0"; p->next = q; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(defn); p->text = "Decimal"; p->first = q; p->next = 0; r = p;

    /* Primary = '(' Additive ')' / Decimal */
    p = new_node(rule); p->text = "Decimal"; s = p;
    p = new_node(lit); p->text = ")"; p->next = 0; q = p;
    p = new_node(rule); p->text = "Additive"; p->next = q; q = p;
    p = new_node(lit); p->text = "("; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(defn); p->text = "Primary"; p->first = q; p->next = r; r = p;

    /* Multitive = Primary '*' Multitive / Primary */
    p = new_node(rule); p->text = "Primary"; s = p;
    p = new_node(rule); p->text = "Multitive"; q = p;
    p = new_node(lit); p->text = "*"; p->next = q; q = p;
    p = new_node(rule); p->text = "Primary"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(defn); p->text = "Multitive"; p->first = q; p->next = r; r = p;
    /* Additive = Multitive '+' Additive / Multitive */
    p = new_node(rule); p->text = "Multitive"; s = p;
    p = new_node(rule); p->text = "Additive"; q = p;
    p = new_node(lit); p->text = "+"; p->next = q; q = p;
    p = new_node(rule); p->text = "Multitive"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(defn); p->text = "Additive"; p->first = q; p->next = r; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    resolve(p, p);

    return p;
}
