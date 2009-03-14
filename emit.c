#include <stdio.h>

#include "syntax.h"

static void grammar_pre(struct s_node *n) {
    int r = 0;
    struct s_node *p;

    for (p = n->first; p; p = p->next)
	++r;
    printf("#define n_rules %d\n", r);
    printf("st = %d;\ntop:\nswitch(st) {\n", n->first->id);
}

static void grammar_post(struct s_node *n) {
    printf("case -1: break;\n");
    printf("}\n");
}

static void literal(char *t) {
    printf("if (! (string[col] == '%c')) goto contin;\n", t[0]);
    printf("cur->remainder = ++col;\n");
    printf("cur->status = parsed;\n");
}

static void rule_pre(struct s_node *n) {
    static int count = 0;
    printf("case %d:\n", n->id); 
    printf("cur = matrix + col * n_rules + %d;\n", count++);
    printf("if (cur->status == uncomputed) {\n");
    printf("cur->status = no_parse;\n");
}

static void rule_post(struct s_node *n) {
    printf("}\n");
    printf("goto contin;\n");
}

static void node(struct s_node *n) {
    struct s_node *p;
    switch (n->type) {
    case grammar:
	grammar_pre(n);
	break;
    case lit:
	literal(n->text);
	break;
    case rule:
	rule_pre(n);
	break;
    default: break;
    }

    if (s_has_children(n->type))
	for (p = n->first; p; p = p->next)
	    node(p);

    switch(n->type) {
    case grammar:
	grammar_post(n);
	break;
    case rule:
	rule_post(n);
	break;
    default: break;
    }
}

void emit(struct s_node *g) {
    node(g);
}
