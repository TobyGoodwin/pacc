#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "syntax.h"

/* Hmm... it would probably be worth having a generic "walk" function
 * that could handle desugaring, dumping, resolving, etc. One nice thing
 * we could then do is implement walk with an explicit stack, rather
 * than recursion.
 */

static void derep1(struct s_node *g, struct s_node *n) {
    char *x0, *x1, *xx;
    int l;
    struct s_node *p, *q, *r, *s;

    /* generate a rule that matches exactly one of the rep */
    l = snprintf(0, 0, "..%d", n->id);
    x0 = malloc(l + 1);
    if (!x0) nomem();
    snprintf(x0, l, "..%d", n->id);
    //fprintf(stderr, "!!! got a rep - nm is %s !!!\n", x0);
    *x0 = '1'; x1 = strdup(x0); if (!x1) nomem();
    *x0 = 'x'; xx = strdup(x0); if (!xx) nomem();
    free(x0);

    p = s_text(type, "int" /* XXX: void */); p->next = n->first; q = p;
    p = s_text(rule, x1); p->first = q; r = p;

    if (!n->text || *n->text == '\0') {
	/* generate a rule that matches zero or more of the rep */
	p = s_new(seq); s = p;
	p = s_text(call, xx); q = p;
	p = s_text(call, x1); p->next = q; q = p;
	p = s_new(seq); p->first = q; p->next = s; q = p;
	p = s_new(alt); p->first = q; q = p;
	p = s_text(type, "int" /* XXX: void */); p->next = q; q = p;
	p = s_text(rule, xx); p->first = q; p->next = r; r = p;
    } else if (strcmp(n->text, "1,") == 0) {
	/* generate a rule that matches one or more of the rep */
	p = s_text(call, x1); q = p;
	p = s_new(seq); p->first = q; s = p;
	p = s_text(call, xx); q = p;
	p = s_text(call, x1); p->next = q; q = p;
	p = s_new(seq); p->first = q; p->next = s; q = p;
	p = s_new(alt); p->first = q; q = p;
	p = s_text(type, "int" /* XXX: void */); p->next = q; q = p;
	p = s_text(rule, xx); p->first = q; p->next = r; r = p;
    } else if (strcmp(n->text, ",1") == 0) {
	/* generate a rule that matches zero or one of the rep */
	p = s_new(seq); s = p;
	p = s_text(call, x1); q = p;
	p = s_new(seq); p->first = q; p->next = s; q = p;
	p = s_new(alt); p->first = q; q = p;
	p = s_text(type, "int" /* XXX: void */); p->next = q; q = p;
	p = s_text(rule, xx); p->first = q; p->next = r; r = p;
    }

    n->text = xx;
    n->type = call;
    n->first = 0;

    /* can't just link onto the front of the rules, since the first rule
     * is the start rule :-( XXX what about having struct node
     * *new_rules; - dump any newly created rules (which presumably do
     * not require further desugaring) into new_rules, then link them in
     * all at one go at the end?
     */
    for (p = g->first; p; p = p->next)
	q = p;

    q->next = r;
}

static void derep0(struct s_node *g, struct s_node *n) {
    struct s_node *p;

    if (s_has_children(n->type))
	for (p = n->first; p; p = p->next)
	    if (p->type == rep)
		derep1(g, p);
	    else
		derep0(g, p);
}

static void debind1(struct s_node *g, struct s_node *n) {
    assert(0);
}

static void debind0(struct s_node *g, struct s_node *n) {
    struct s_node *p;

    if (s_has_children(n->type))
	for (p = n->first; p; p = p->next)
	    if (p->type == call && p->first->type != bind)
		debind1(g, p);
	    else
		debind0(g, p);
}

void desugar(struct s_node *g) {
    derep0(g, g);
}
