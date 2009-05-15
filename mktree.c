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

    /* A single character:
     *
     * A <- 'a'
     *
     */

    /* A <- 'a' */
    p = new_node(lit); p->text = "a"; p->next = 0; q = p;
    p = new_node(rule); p->text = "A"; p->first = q; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    resolve(p, p);

    return p;
}
#endif

#if 0
struct s_node *create(void) {
    struct s_node *p, *q, *r, *s;

    /* A trivial sequence:
     *
     * A <- 'a' 'b'
     *
     */

    /* A <- 'a' 'b' */
    p = new_node(lit); p->text = "b"; q = p;
    p = new_node(lit); p->text = "a"; p->next = q; q = p;
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

    /* A trivial alternation:
     *
     * A <- 'a' / 'b'
     *
     */

    /* A <- 'a' 'b' */
    p = new_node(lit); p->text = "b"; q = p;
    p = new_node(lit); p->text = "a"; p->next = q; q = p;
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

    /* A trivial call:
     *
     * P <- A
     * A <- 'a'
     *
     */

    /* A <- 'a' */
    p = new_node(lit); p->text = "a"; q = p;
    p = new_node(rule); p->text = "A"; p->first = q; r = p;

    /* P <- A */
    p = new_node(call); p->text = "A"; q = p;
    p = new_node(rule); p->text = "P"; p->first = q; p->next = r; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    resolve(p, p);

    return p;
}
#endif

#if 0
struct s_node *create(void) {
    struct s_node *p, *q, *r, *s;

    /* A trivial action:
     *
     * P <- 'a' { printf("Hello, world\n"); }
     *
     */

    /* A <- 'a' { printf("Hello, world\n"); } */
    p = new_node(act); p->text = "printf(\"Hello, world\\n\");"; q = p;
    p = new_node(lit); p->text = "a"; p->next = q; q = p;
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

    /* Sequence / call combination:
     *
     * P <- A B
     * A <- 'a'
     * B <- 'b'
     *
     */

    /* B <- 'b' */
    p = new_node(lit); p->text = "b"; q = p;
    p = new_node(rule); p->text = "B"; p->first = q; r = p;

    /* A <- 'a' */
    p = new_node(lit); p->text = "a"; q = p;
    p = new_node(rule); p->text = "A"; p->first = q; p->next = r; r = p;

    /* P <- A B */
    p = new_node(call); p->text = "B"; q = p;
    p = new_node(call); p->text = "A"; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(rule); p->text = "P"; p->first = q; p->next = r; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    resolve(p, p);

    return p;
}
#endif

#if 1
struct s_node *create(void) {
    struct s_node *p, *q, *r, *s;

    /* Sequence / action combination:
     *
     * P <- A B
     * A <- 'a' { printf("matched: a\n"); }
     * B <- 'b' { printf("matched: b\n"); }
     *
     */

    /* B <- 'b' { ... } */
    p = new_node(act); p->text = "{ printf(\"matched: b\\n\"); }"; q = p;
    p = new_node(lit); p->text = "b"; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(rule); p->text = "B"; p->first = q; r = p;

    /* A <- 'a' { ... } */
    p = new_node(act); p->text = "{ printf(\"matched: a\\n\"); }"; q = p;
    p = new_node(lit); p->text = "a"; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(rule); p->text = "A"; p->first = q; p->next = r; r = p;

    /* P <- A B */
    p = new_node(call); p->text = "B"; q = p;
    p = new_node(call); p->text = "A"; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(rule); p->text = "P"; p->first = q; p->next = r; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    resolve(p, p);

    return p;
}
#endif


#if 0
struct s_node *create(void) {
    struct s_node *a, *p, *q, *r, *s;

    /* Sequence / alternative / call combination:
     *
     * P <- A B / B
     * A <- 'a'
     * B <- 'b'
     *
     */

    /* B <- 'b' */
    p = new_node(lit); p->text = "b"; q = p;
    p = new_node(rule); p->text = "B"; p->first = q; r = p;

    /* A <- 'a' */
    p = new_node(lit); p->text = "a"; q = p;
    p = new_node(rule); p->text = "A"; p->first = q; p->next = r; r = p;

    /* P <- A B / B */
    p = new_node(call); p->text = "B"; a = p;
    p = new_node(call); p->text = "B"; q = p;
    p = new_node(call); p->text = "A"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = a; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(rule); p->text = "P"; p->first = q; p->next = r; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    resolve(p, p);

    return p;
}
#endif

#if 0
struct s_node *create(void) {
    struct s_node *p, *q, *r, *s;

    /* A trivial recursion:
     *
     * P <- 'a' P / 'a'
     *
     */

    /* P <- 'a' P / 'a' */
    p = new_node(lit); p->text = "a"; r = p;
    p = new_node(call); p->text = "P"; q = p;
    p = new_node(lit); p->text = "a"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = r; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(rule); p->text = "P"; p->first = q; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    resolve(p, p);

    return p;
}
#endif

#if 0
struct s_node *create(void) {
    struct s_node *p, *q, *r, *s;

    /* Bryan Ford's trivial grammar:
     *
     * Additive <- Multitive '+' Additive / Multitive
     * Multitive <- Primary '*' Multitive / Primary
     * Primary <- '(' Additive ')' / Decimal
     * Decimal <- '0' / '1' / ... / '9'
     *
     */

    /* Decimal = '0' / '1' / ... / '9' */
    p = new_node(lit); p->text = "9"; p->next = 0; q = p;
    p = new_node(lit); p->text = "8"; p->next = q; q = p;
    p = new_node(lit); p->text = "7"; p->next = q; q = p;
    p = new_node(lit); p->text = "6"; p->next = q; q = p;
    p = new_node(lit); p->text = "5"; p->next = q; q = p;
    p = new_node(lit); p->text = "4"; p->next = q; q = p;
    p = new_node(lit); p->text = "3"; p->next = q; q = p;
    p = new_node(lit); p->text = "2"; p->next = q; q = p;
    p = new_node(lit); p->text = "1"; p->next = q; q = p;
    p = new_node(lit); p->text = "0"; p->next = q; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(rule); p->text = "Decimal"; p->first = q; p->next = 0; r = p;

    /* Primary = '(' Additive ')' / Decimal */
    p = new_node(call); p->text = "Decimal"; s = p;
    p = new_node(lit); p->text = ")"; p->next = 0; q = p;
    p = new_node(call); p->text = "Additive"; p->next = q; q = p;
    p = new_node(lit); p->text = "("; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(rule); p->text = "Primary"; p->first = q; p->next = r; r = p;

    /* Multitive = Primary '*' Multitive / Primary */
    p = new_node(call); p->text = "Primary"; s = p;
    p = new_node(call); p->text = "Multitive"; q = p;
    p = new_node(lit); p->text = "*"; p->next = q; q = p;
    p = new_node(call); p->text = "Primary"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(rule); p->text = "Multitive"; p->first = q; p->next = r; r = p;

    /* Additive = Multitive '+' Additive / Multitive */
    p = new_node(call); p->text = "Multitive"; s = p;
    p = new_node(call); p->text = "Additive"; q = p;
    p = new_node(lit); p->text = "+"; p->next = q; q = p;
    p = new_node(call); p->text = "Multitive"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(rule); p->text = "Additive"; p->first = q; p->next = r; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    resolve(p, p);

    return p;
}
#endif

#if 0
struct s_node *create(void) {
    struct s_node *p, *q, *r, *s;

    /* An even trivialer grammar:
     *
     * P <- A B / A P
     * A <- 'a'
     * B <- 'b'
     *
     */

    /* B <- 'b' */
    p = new_node(lit); p->text = "b"; p->next = 0; q = p;
    p = new_node(rule); p->text = "B"; p->first = q; p->next = 0; r = p;

    /* A <- 'a' */
    p = new_node(lit); p->text = "a"; p->next = 0; q = p;
    p = new_node(rule); p->text = "A"; p->first = q; p->next = r; r = p;

    /* P <- A B / A P */
    p = new_node(call); p->text = "P"; q = p;
    p = new_node(call); p->text = "A"; p->next = q; q = p;
    p = new_node(seq); p->first = q; s = p;
    p = new_node(call); p->text = "B"; q = p;
    p = new_node(call); p->text = "A"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(rule); p->text = "P"; p->first = q; p->next = r; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    resolve(p, p);

    return p;
}
#endif
