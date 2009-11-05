#include "syntax.h"

char *prefix = 0;

struct s_node *create(void) {
    struct s_node *p, *q, *r, *s;

    /* The ? operator:
     *
     * char *S <- 'x' Y 'x' → { match() }
     * Y ← 'y' ?
     *
     */

    p = new_node(lit); p->text = "y"; s = p;
    p = s_text(rep, ",1"); p->first = s; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "Y"; p->first = q; r = p;
    
    p = new_node(expr); p->text = "match()"; q = p;
    p = new_node(lit); p->text = "x"; p->next = q; q = p;
    p = new_node(call); p->text = "Y"; p->next = q; q = p;
    p = new_node(lit); p->text = "x"; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "S"; p->first = q; p->next = r; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    return p;
}
