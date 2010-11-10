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

/* The deblit (remove bound literal) sugaring converts this tree:

        lit 115: five 
          call 114: Word 

    into this tree:

        seq 115: 
          bind 147: _pacc_bl115 
            call 114: Word 
          guard 146: ref_streq(_pacc_bl115,"five") 
            ident 145: _pacc_bl115 

 */
static void deblit1(struct s_node *g, struct s_node *n, const char *name, int c) {
    char *gd, *id;
    int l;
    struct s_node *p, *q;

    /* We need a unique identifier name for this bound literal... */
    l = snprintf(0, 0, "_pacc_bl%ld", n->id) + 1;
    id = malloc(l);
    if (!id) nomem();
    snprintf(id, l, "_pacc_bl%ld", n->id);

    /* ... and a guard string. */
    l = snprintf(0, 0, "ref_streq(%s,\"%s\")", id, n->text) + 1;
    gd = malloc(l);
    if (!gd) nomem();
    snprintf(gd, l, "ref_streq(%s,\"%s\")", id, n->text);
    fprintf(stderr, "in deblit1, id is %s\n", id);
    fprintf(stderr, "in deblit1, gd is %s\n", gd);

    p = s_text(ident, id); q = p;
    p = s_both(guard, gd, q); q = p;
    p = s_both(bind, id, n->first); p->next = q; q = p;

    free(n->text);
    n->first = p;
    s_retype(seq, n);
}

static void deblit0(struct s_node *g, struct s_node *n) {
    static char *name;
    static int c;
    struct s_node *p;

    if (n->type == rule) {
	name = n->text;
	c = 0;
    }
    if (s_has_children(n->type))
	for (p = n->first; p; p = p->next)
	    if (p->type == lit && p->first)
		deblit1(g, p, name, c++);
	    else
		deblit0(g, p);
}

/*
The debind sugaring creates a new rule for the right hand side of a
non-rule binding:

S <- r:("y" "z") { use r }

becomes, effectively

S <- r:S_r { use r }
S_r <- "y" "z"

except that the name of the invented rule contains a prohibited
character instead of the underscore.
*/
static void debind1(struct s_node *g, struct s_node *n, const char *name, int c) {
    char *x;
    int l;
    struct s_node *p, *q, *r;

    l = snprintf(0, 0, "%s:%s:%d", name, n->text, c) + 1;
    x = malloc(l);
    if (!x) nomem();
    snprintf(x, l, "%s:%s:%d", name, n->text, c);

    p = s_text(expr, "ref()");
    p = s_kid(seq, cons(n->first, p));
    p = cons(s_text(type, "ref_t"), p);
    r = s_both(rule, x, p);
    
    n->first = s_text(call, x);

    for (p = g->first; p; p = p->next)
	q = p;

    q->next = r;
}

static void debind0(struct s_node *g, struct s_node *n) {
    static char *name;
    static int c;
    struct s_node *p;

    if (n->type == rule) {
	name = n->text;
	c = 0;
    }
    if (s_has_children(n->type))
	for (p = n->first; p; p = p->next)
	    if (p->type == bind && p->first->type != call)
		debind1(g, p, name, c);
	    else
		debind0(g, p);
}

static void derep1(struct s_node *g, struct s_node *n, char *name, int c) {
    char *x0, *x1, *xx;
    int l;
    struct s_node *p, *q, *r, *s;

    /* Create names for two new rules. User-named rules can never
     * contain a colon, so we will use that in rules generated by
     * desugaring to ensure uniqueness. */
    l = snprintf(0, 0, "%s:.:%d", name, c) + 1;
    x0 = malloc(l);
    if (!x0) nomem();
    snprintf(x0, l, "%s:.:%d", name, c);
    //fprintf(stderr, "!!! got a rep - nm is %s !!!\n", x0);
    l = strlen(name) + 1;
    x0[l] = '1'; x1 = strdup(x0); if (!x1) nomem();
    x0[l] = '*'; xx = strdup(x0); if (!xx) nomem();
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
    static char *name;
    static int c;
    struct s_node *p;

    if (n->type == rule) {
	name = n->text;
	c = 0;
    }
    if (s_has_children(n->type))
	for (p = n->first; p; p = p->next)
	    if (p->type == rep)
		derep1(g, p, name, c++);
	    else
		derep0(g, p);
}

void desugar(struct s_node *g) {
    deblit0(g, g);
    debind0(g, g);
    derep0(g, g);
}
