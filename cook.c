#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arg.h"
#include "cook.h"
#include "error.h"
#include "preen.h"
#include "syntax.h"

/* cook() prepares a grammar for feeding */

/* perhaps we should use the walk() function from sugar.c, but that doesn't
 * support the "pre" variable that we use. */

static void cook0(struct s_node *n) {
    static char *name;
    struct s_node *p;

    if (n->type == rule)
	name = n->text;

    /* Replace
     *     ... _ foo
     * with
     *     ... _ (foo / !.)
     * for any non-empty foo. 
     */
    if (n->type == cafe) {
	struct s_node *t;
	//fprintf(stderr, "match at node %ld\n", n->id);
	if (!n->next)
	    fatal3("invalid `$' at the end of rule `", name, "'");
	t = s_kid(seq, s_kid(not, s_new(any)));
	t = cons(s_kid(seq, n->next), t);
	t = s_kid(alt, t);
	//	pre->next = t;
	//	free(n);
	n->next = t;
    }

    if (s_has_children(n->type))
	for (p = n->first; p; p = p->next)
	    cook0(p);
}

/* make all rules void, and remove all expr nodes */
static void dexpr(struct s_node *n, struct s_node *pre) {
    struct s_node *p, *q;
    if (n->type == expr) {
	assert(pre);
	pre->next = n->next;
	free(n); /* XXX and its children */
    }

    /* XXX in principle, we could remove all bindings *except* those
     * used in guards. */

    if (s_has_children(n->type))
	for (p = 0, q = n->first; q; p = q, q = q->next)
	    dexpr(q, p);
}

void cook(struct s_node *g) {
    char *newname;
    int l;

    dexpr(g, 0);
    cook0(g);
    l = strlen(g->text) + strlen("_feed") + 1;
    newname = realloc(0, l);
    if (!newname) nomem();
    strcpy(newname, g->text);
    strcat(newname, "_feed");
    g->text = newname;
    check_recursion(g);
}
