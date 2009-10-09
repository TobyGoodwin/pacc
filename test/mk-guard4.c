#include "syntax.h"

char *prefix = 0;

struct s_node *create(void) {
    struct s_node *p, *q, *r, *s;

    /* Semantic predicate and any matcher:
     *
     * char *P ← b:A &{ *b >= 'a' && *b <= 'z' } { b }
     * char *A <- . { match() }
     *
     * This is not right, of course. We need better types!
     *
     * 2009-10-09: XXX The above comment was present in the first version of
     * this file, dated 2009-07-04. Unfortunately, I'm not sure what
     * it's getting at. Given that we are assuming utf8, only a "char *"
     * can hold what "." matches.
     *
     * Perhaps the (yet to be written) tree munger could automatically
     * supply the "match()" expression, but beyond that I don't see
     * what's wrong with the example. I wish this didn't happen. :-(
     *
     */

    p = new_node(expr); p->text = "match()"; q = p;
    p = new_node(any); p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "A"; p->first = q; r = p;

    p = s_new(ident); p->text = "b"; q = p;
    p = new_node(expr); p->text = "b"; p->first = q; q = p;
    p = new_node(ident); p->text = "b"; s = p;
    p = new_node(guard); p->text = "*b>='a' && *b<='z'"; p->first = s; p->next = q; q = p;
    p = new_node(call); p->text = "A"; s = p;
    p = new_node(bind); p->text = "b"; p->first = s; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "P"; p->first = q; p->next = r; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    resolve(p, p);

    return p;
}
