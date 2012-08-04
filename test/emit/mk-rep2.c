/*
type chars
noparse xx Y 2
parse xyx xyx
parse xyyx xyyx
parse xyyyx xyyyx
parse xyyyyyyyyyyx xyyyyyyyyyyx
*/

#include <sys/types.h>

#include "syntax.h"

int pacc_wrap(const char *ign0, char *ign1, off_t ign2, struct s_node **result) {
    struct s_node *p, *q, *r, *s;

    /* The + operator:
     *
     * char *S <- 'x' Y 'x' → { ref_str() }
     * Y :: void ← 'y' +
     *
     */

    p = new_node(lit); p->text = "y"; s = p;
    p = s_text(rep, "1,"); p->first = s; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "void"; p->next = q; q = p;
    p = new_node(rule); p->text = "Y"; p->first = q; r = p;
    
    p = new_node(expr); p->text = "ref_str()"; q = p;
    p = new_node(lit); p->text = "x"; p->next = q; q = p;
    p = new_node(call); p->text = "Y"; p->next = q; q = p;
    p = new_node(lit); p->text = "x"; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "S"; p->first = q; p->next = r; r = p;

    r = cons(s_text(preamble, 0), r);
    p = s_both(grammar, "pacc", r);

    *result = p;
    return 1;
}
