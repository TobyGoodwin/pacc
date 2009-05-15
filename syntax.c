#include <stdio.h>

#include "syntax.h"

char *decode_type(enum s_type t) {
    switch(t) {
    case act:		return "act";
    case alt:		return "alt";
    case call:		return "call";
    case grammar:	return "grammar";
    case lit:		return "lit";
    case rule:		return "rule";
    case seq:		return "seq";
    default:		return "[unknown]";
    }
}

int s_has_children(enum s_type t) {
    return t == alt || t == rule || t == grammar || t == seq;
}

int s_has_text(enum s_type t) {
    return t == act || t == call || t == grammar || t == rule || t == lit;
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
