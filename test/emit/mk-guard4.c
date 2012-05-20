/*
type chars
parse a a
parse q q
parse z z
noparse A A 2
noparse % A 2
*/

#include <sys/types.h>

#include "syntax.h"

int pacc_parse(const char *ign0, char *ign1, off_t ign2, struct s_node **result) {
    struct s_node *p, *q, *r, *s;

    /* Semantic predicate and any matcher:
     *
     * char *P ← b:A &{ *b >= 'a' && *b <= 'z' } { b }
     * char *A <- . { ref_str() }
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
     * 2010-02-24: match() has now become ref_str(). Perhaps the "better
     * types" comment was a call for ref_t, which now exists, but I
     * haven't yet rewritten this test to use it.
     *
     */

    p = new_node(expr); p->text = "ref_str()"; q = p;
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

    r = cons(s_text(preamble, 0), r);
    p = new_node(grammar); p->text = "pacc"; p->first = r;

    *result = p;
    return 1;
}
