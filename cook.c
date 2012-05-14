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
	fprintf(stderr, "match at node %ld\n", n->id);
    }

    /* remove all expr nodes */
    if (n->type == expr) {
	assert(pre);
	pre->next = n->next;
	free(n); /* XXX and its children */
    }

    if (s_has_children(n->type))
	for (p = 0, q = n->first; q; p = q, q = q->next)
	    cook0(q, p);

    return n;
}

struct s_node *cook(struct s_node *g) {
    feed_rule = arg_feed_rule();
    return cook0(g, 0);
}
