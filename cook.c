#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arg.h"
#include "cook.h"
#include "syntax.h"

/* cook() prepares a grammar for feeding */

/* perhaps we should use the walk() function from sugar.c, but i'm not
 * sure it's worth it. */

static char *feed_rule;

static struct s_node *cook0(struct s_node *n, struct s_node *pre) {
    struct s_node *p, *q;

    if (n->type == call && strcmp(feed_rule, n->text) == 0) {
	struct s_node *t;
	fprintf(stderr, "match at node %ld\n", n->id);
	t = s_kid(seq, s_kid(not, s_new(any)));
	t = cons(s_kid(seq, n->next), t);
	t = s_kid(alt, t);
//	pre->next = t;
//	free(n);
	n->next = t;
    }

    /* remove all expr nodes */
    if (n->type == expr) {
	assert(pre);
	pre->next = n->next;
	free(n); /* XXX and its children */
    }

    /* XXX in principle, we could remove all bindings *except* those
     * used in guards. */

    if (s_has_children(n->type))
	for (p = 0, q = n->first; q; p = q, q = q->next)
	    cook0(q, p);

    return n;
}

struct s_node *cook(struct s_node *g) {
    char *newname;
    int l;

    l = strlen(g->text) + strlen("_feed") + 1;
    newname = realloc(0, l);
    if (!newname) nomem();
    strcpy(newname, g->text);
    strcat(newname, "_feed");
    g->text = newname;
    feed_rule = arg_feed();
    return cook0(g, 0);
}
