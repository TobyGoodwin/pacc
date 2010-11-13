#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "syntax.h"

/* We now have a generic "walk" function that handles all current
 * transforms. I used to think that at this point I would implement walk
 * with an explicit stack, rather than recursion. Now I wonder why? This
 * is the compiler; not the emitted code. It's not worth sacrificing
 * clarity to imagined performance. 
 */

static struct s_node *rules = 0;

/* XXX only find_type needs this; must be a better way? */
static struct s_node *top;

/*
The deblit (desugar bound literal) transform converts this tree:

        lit 115: five 
          call 114: Word 

    into this tree:

        seq 115: 
          bind 147: _pacc_bl115 
            call 114: Word 
          guard 146: ref_streq(_pacc_bl115,"five") 
            ident 145: _pacc_bl115 

 */
static void deblit(struct s_node *n, const char *ign0, int ign1) {
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

static int isblit(struct s_node *n) {
    return n->type == lit && n->first;
}

/* XXX this is pretty grody, and kindof in the wrong place */
char *find_type(char *name) {
    struct s_node *p;

    for (p = top->first->next; p; p = p->next)
	if (strcmp(p->text, name) == 0)
	    return p->first->text;

    fatal3("rule not found: `", name, "'");
    return 0; /* not reached */
}

/*
The dequery (desugar query operator) transform changes
    R <- x: Thing? { ... }
    Thing :: t <- ...
into
    R <- x: ThingOpt { ... }
    ThingOpt :: t <- a:Thing -> a / epsilon -> 0
except with different names for the virtual rules. It (mostly) works
because "0" is a member of both numeric and pointer types in C.
*/

static void dequery(struct s_node *n, const char *name, int c) {
    char *xo, *thing, *typ;
    int l;
    struct s_node *p, *q, *s;

    thing = n->first->first->text;

    /* XXX to find the type of the called rule, we need to find the
     * rule, and we haven't called resolve() yet. At this point, we
     * don't even have a pointer that's of any use to find it! Darn,
     * this is going to get messy.
     */
    typ = find_type(thing);
    //fprintf(stderr, "thing is %s :: %s\n", thing, typ);

    /* Create the name for ThingOpt. */
    l = snprintf(0, 0, "%s:?:%d", name, c) + 1;
    xo = malloc(l);
    if (!xo) nomem();
    snprintf(xo, l, "%s:?:%d", name, c);

    /* generate the new rule */
    p = s_kid(seq, s_text(expr, "0")); s = p;
    p = s_both(expr, "a", s_text(ident, "a"));
    p = cons(s_both(bind, "a", s_text(call, thing)), p); q = p;
    p = s_new(seq); p->first = q; p->next = s; q = p;
    p = s_new(alt); p->first = q; q = p;
    p = s_text(type, typ); p->next = q; q = p;
    p = s_text(rule, xo); p->first = q;
    p->next = rules; rules = p;

    /* replace "bind / rep / call" with "bind / call" */
    n->first->text = xo;
    n->first->type = call;
    free(n->first->first);
    n->first->first = 0;
}

/*
The debind (desugar binding) transform creates a new rule for the right
hand side of a non-rule binding.
    S <- r:("y" "z") { use r }
becomes, effectively
    S <- r:S_r { use r }
    S_r <- "y" "z"
except that the name of the invented rule contains a prohibited
character instead of the underscore.
*/
static void debind(struct s_node *n, const char *name, int c) {
    char *x;
    int l;
    struct s_node *p, *r;

    if (n->first && n->first->type == rep &&
	n->first->text && strcmp(n->first->text, ",1") == 0 &&
	n->first->first && n->first->first->type == call) {
	dequery(n, name, c);
	return;
    }

    l = snprintf(0, 0, "%s:%s:%d", name, n->text, c) + 1;
    x = malloc(l);
    if (!x) nomem();
    snprintf(x, l, "%s:%s:%d", name, n->text, c);

    p = s_text(expr, "ref()");
    p = s_kid(seq, cons(n->first, p));
    p = cons(s_text(type, "ref_t"), p);
    r = s_both(rule, x, p);
    
    n->first = s_text(call, x);

    r->next = rules;
    rules = r;
}

static int isbind(struct s_node *n) {
    return n->type == bind && n->first->type != call;
}

/*
The derep (desugar repetitions) transform changes
    Y <- "y" *
into
    Y <- Y_star
    Y_star <- Y_one Y_star / epsilon
    Y_one <- "y"
except with different names for the virtual rules.
*/

static void derep(struct s_node *n, const char *name, int c) {
    char *x0, *x1, *xx;
    int l;
    struct s_node *p, *q, *s;

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
    p = s_text(rule, x1); p->first = q;
    p->next = rules; rules = p;

    if (!n->text || *n->text == '\0') {
	/* generate a rule that matches zero or more of the rep */
	p = s_new(seq); s = p;
	p = s_text(call, xx); q = p;
	p = s_text(call, x1); p->next = q; q = p;
	p = s_new(seq); p->first = q; p->next = s; q = p;
	p = s_new(alt); p->first = q; q = p;
	p = s_text(type, "int" /* XXX: void */); p->next = q; q = p;
	p = s_text(rule, xx); p->first = q;
	p->next = rules; rules = p;
    } else if (strcmp(n->text, "1,") == 0) {
	/* generate a rule that matches one or more of the rep */
	p = s_text(call, x1); q = p;
	p = s_new(seq); p->first = q; s = p;
	p = s_text(call, xx); q = p;
	p = s_text(call, x1); p->next = q; q = p;
	p = s_new(seq); p->first = q; p->next = s; q = p;
	p = s_new(alt); p->first = q; q = p;
	p = s_text(type, "int" /* XXX: void */); p->next = q; q = p;
	p = s_text(rule, xx); p->first = q;
	p->next = rules; rules = p;
    } else if (strcmp(n->text, ",1") == 0) {
	/* generate a rule that matches zero or one of the rep */
	p = s_new(seq); s = p;
	p = s_text(call, x1); q = p;
	p = s_new(seq); p->first = q; p->next = s; q = p;
	p = s_new(alt); p->first = q; q = p;
	p = s_text(type, "int" /* XXX: void */); p->next = q; q = p;
	p = s_text(rule, xx); p->first = q;
	p->next = rules; rules = p;
    }

    n->text = xx;
    n->type = call;
    n->first = 0;
}

static int isrep(struct s_node *n) {
    return n->type == rep;
}

typedef int pred_fn_t(struct s_node *);
typedef void trans_fn_t(struct s_node *, const char *, int);

static void walk(struct s_node *n, pred_fn_t pred, trans_fn_t transform) {
    static char *name;
    static int c;
    struct s_node *p;

    if (n->type == rule) {
	name = n->text;
	c = 0;
    }
    if (s_has_children(n->type))
	for (p = n->first; p; p = p->next)
	    if (pred(n))
		transform(n, name, c++);
	    else
		walk(p, pred, transform);
}

void desugar(struct s_node *g) {
    struct s_node *p;

    top = g; /* XXX for find_type */

    walk(g, &isblit, &deblit);

    walk(g, &isbind, &debind);
    for (p = g->first; p->next; p = p->next)
	;
    p->next = rules;
    rules = 0;

    walk(g, &isrep, &derep);

    for (p = g->first; p->next; p = p->next)
	;
    p->next = rules;
}
