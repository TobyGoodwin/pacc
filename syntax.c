#include <stdio.h>

#include "syntax.h"

char *decode_type(enum s_type t) {
    switch(t) {
    case grammar:	return "grammar";
    case rule:		return "rule";
    case alt:		return "alt";
    case lit:		return "lit";
    default:		return "[unknown]";
    }
}

int s_has_children(enum s_type t) {
    return t == grammar || t == rule;
}

int s_has_text(enum s_type t) {
    return t == grammar || t == rule || t == lit;
}

static void dump(struct s_node *p, int indent) {
    int i;

    if (!p) return;
    for (i = 0; i < indent; ++i) fprintf(stderr, "  ");
    fprintf(stderr, "%s: ", decode_type(p->type));
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
