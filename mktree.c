#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "syntax.h"

struct s_node *new_node(enum s_type t) {
    static int id = 100;
    struct s_node *n;
    n = malloc(sizeof *n);
    n->id = id++;
    n->type = t;
    return n;
}

void resolve(struct s_node *g, struct s_node *n) {
    struct s_node *p;
    if (n->type == call && !n->first) {
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

#if 0
struct s_node *create(void) {
    struct s_node *p, *q, *r, *s;

    /* A single character with value:
     *
     * A <- '5' { 5 }
     *
     */

    /* A <- 'a' */
    p = new_node(expr); p->text = "5"; q = p;
    p = new_node(lit); p->text = "5"; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(rule); p->text = "A"; p->first = q; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    resolve(p, p);

    return p;
}
#endif

#if 0
struct s_node *create(void) {
    struct s_node *p, *q, *r, *s;

    /* Alternation with value:
     *
     * A <- '5' { 5 } / '6' { 6 }
     *
     */

    p = new_node(expr); p->text = "6"; q = p;
    p = new_node(lit); p->text = "6"; p->next = q; q = p;
    p = new_node(seq); p->first = q; s = p;
    p = new_node(expr); p->text = "5"; q = p;
    p = new_node(lit); p->text = "5"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(rule); p->text = "A"; p->first = q; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    resolve(p, p);

    return p;
}
#endif

#if 0
struct s_node *create(void) {
    struct s_node *p, *q, *r, *s;

    /* Call with value:
     *
     * P <- a:A { a }
     * A <- '5' { 5 } / '6' { 6 }
     *
     */

    p = new_node(expr); p->text = "6"; q = p;
    p = new_node(lit); p->text = "6"; p->next = q; q = p;
    p = new_node(seq); p->first = q; s = p;
    p = new_node(expr); p->text = "5"; q = p;
    p = new_node(lit); p->text = "5"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(rule); p->text = "A"; p->first = q; r = p;

    p = new_node(expr); p->text = "a"; s = p;
    p = new_node(call); p->text = "A"; q = p;
    p = new_node(bind); p->text = "a"; p->first = q; p->next = s; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(rule); p->text = "P"; p->first = q; p->next = r; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    resolve(p, p);

    return p;
}
#endif

#if 0
struct s_node *create(void) {
    struct s_node *p, *q, *r, *s;

    /* Two calls with values:
     *
     * P <- a:A '.' b:A { a * b }
     * A <- '5' { 5 } / '6' { 6 }
     *
     */

    p = new_node(expr); p->text = "6"; q = p;
    p = new_node(lit); p->text = "6"; p->next = q; q = p;
    p = new_node(seq); p->first = q; s = p;
    p = new_node(expr); p->text = "5"; q = p;
    p = new_node(lit); p->text = "5"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(rule); p->text = "A"; p->first = q; r = p;

    p = new_node(expr); p->text = "a * b"; s = p;
    p = new_node(call); p->text = "A"; q = p;
    p = new_node(bind); p->text = "b"; p->first = q; p->next = s; s = p;
    p = new_node(lit); p->text = "."; p->next = s; s = p;
    p = new_node(call); p->text = "A"; q = p;
    p = new_node(bind); p->text = "a"; p->first = q; p->next = s; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(rule); p->text = "P"; p->first = q; p->next = r; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    resolve(p, p);

    return p;
}
#endif

#if 1
struct s_node *create(void) {
    struct s_node *p, *q, *r, *s, *t;

    /* Bryan Ford's trivial grammar with actions:
     *
     * Additive <- Multitive '+' Additive / Multitive
     * Multitive <- Primary '*' Multitive / Primary
     * Primary <- '(' Additive ')' / Decimal
     * Decimal <- '0' / '1' / ... / '9'
     *
     * Additive <- m:Multitive '+' a:Additive { m + a } / Multitive
     * Multitive <- p:Primary '*' m:Multitive { p * m } / Primary
     * Primary <- '(' a:Additive ')' -> a / Decimal
     * Decimal <- '0' { 0 } / '1' { 1 } / ... / '9'
     */

    /* Decimal <- '0' { 0 } / '1' { 1 } / ... / '9'{ 9 }  */
    p = new_node(expr); p->text = "9"; q = p;
    p = new_node(lit); p->text = "9"; p->next = q; q = p;
    p = new_node(seq); p->first = q; s = p;

    p = new_node(expr); p->text = "8"; q = p;
    p = new_node(lit); p->text = "8"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; s = p;

    p = new_node(expr); p->text = "7"; q = p;
    p = new_node(lit); p->text = "7"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; s = p;

    p = new_node(expr); p->text = "6"; q = p;
    p = new_node(lit); p->text = "6"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; s = p;

    p = new_node(expr); p->text = "5"; q = p;
    p = new_node(lit); p->text = "5"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; s = p;

    p = new_node(expr); p->text = "4"; q = p;
    p = new_node(lit); p->text = "4"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; s = p;

    p = new_node(expr); p->text = "3"; q = p;
    p = new_node(lit); p->text = "3"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; s = p;

    p = new_node(expr); p->text = "2"; q = p;
    p = new_node(lit); p->text = "2"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; s = p;

    p = new_node(expr); p->text = "1"; q = p;
    p = new_node(lit); p->text = "1"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; s = p;

    p = new_node(expr); p->text = "0"; q = p;
    p = new_node(lit); p->text = "0"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; s = p;

    p = new_node(alt); p->first = s; q = p;
    p = new_node(rule); p->text = "Decimal"; p->first = q; p->next = 0; r = p;

    /* Primary <- '(' a:Additive ')' -> a / Decimal */
    p = new_node(expr); p->text = "d"; q = p;
    p = new_node(call); p->text = "Decimal"; s = p;
    p = new_node(bind); p->text = "d"; p->first = s; p->next = q; q = p;
    p = new_node(seq); p->first = q; s = p;

    p = new_node(expr); p->text = "a"; q = p;
    p = new_node(lit); p->text = ")"; p->next = q; q = p;
    p = new_node(call); p->text = "Additive"; t = p;
    p = new_node(bind); p->text = "a"; p->first = t; p->next = q; q = p;
    p = new_node(lit); p->text = "("; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;

    p = new_node(alt); p->first = q; q = p;
    p = new_node(rule); p->text = "Primary"; p->first = q; p->next = r; r = p;

    /* Multitive <- p:Primary '*' m:Multitive { p * m } / Primary */
    p = new_node(expr); p->text = "p"; q = p;
    p = new_node(call); p->text = "Primary"; s = p;
    p = new_node(bind); p->text = "p"; p->first = s; p->next = q; q = p;
    p = new_node(seq); p->first = q; s = p;

    p = new_node(expr); p->text = "p * m"; q = p;
    p = new_node(call); p->text = "Multitive"; t = p;
    p = new_node(bind); p->text = "m"; p->first = t; p->next = q; q = p;
    p = new_node(lit); p->text = "*"; p->next = q; q = p;
    p = new_node(call); p->text = "Primary"; t = p;
    p = new_node(bind); p->text = "p"; p->first = t; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;

    p = new_node(alt); p->first = q; q = p;
    p = new_node(rule); p->text = "Multitive"; p->first = q; p->next = r; r = p;

    /* Additive <- m:Multitive '+' a:Additive { m + a } / Multitive */
    p = new_node(expr); p->text = "m"; q = p;
    p = new_node(call); p->text = "Multitive"; s = p;
    p = new_node(bind); p->text = "m"; p->first = s; p->next = q; q = p;
    p = new_node(seq); p->first = q; s = p;

    p = new_node(expr); p->text = "m + a"; q = p;
    p = new_node(call); p->text = "Additive"; t = p;
    p = new_node(bind); p->text = "a"; p->first = t; p->next = q; q = p;
    p = new_node(lit); p->text = "+"; p->next = q; q = p;
    p = new_node(call); p->text = "Multitive"; t = p;
    p = new_node(bind); p->text = "m"; p->first = t; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;

    p = new_node(alt); p->first = q; q = p;
    p = new_node(rule); p->text = "Additive"; p->first = q; p->next = r; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    resolve(p, p);

    return p;
}
#endif
