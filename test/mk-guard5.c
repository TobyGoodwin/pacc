#include "syntax.h"

char *prefix = 0;

struct s_node *create(void) {
    struct s_node *p, *q, *r, *s;

    /* Semantic predicates, calls, and any matchers:
     *
     * int P <- a:A &{ *a>='a' && *a<='z' } b:A &{ *b>='0' && *b<='9' } { *b - '0' }
     * char *A <- . { match() }
     *
     */

    p = new_node(expr); p->text = "match()"; q = p;
    p = new_node(any); p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "A"; p->first = q; r = p;

    p = new_node(ident); p->text = "b"; s = p;
    p = new_node(expr); p->text = "*b - '0'"; p->first = s; q = p;
    p = new_node(ident); p->text = "b"; s = p;
    p = new_node(guard); p->text = "*b>='0' && *b<='9'"; p->first = s; p->next = q; q = p;
    p = new_node(call); p->text = "A"; s = p;
    p = new_node(bind); p->text = "b"; p->first = s; p->next = q; q = p;
    p = new_node(ident); p->text = "a"; s = p;
    p = new_node(guard); p->text = "*a>='a' && *a<='z'"; p->first = s; p->next = q; q = p;
    p = new_node(call); p->text = "A"; s = p;
    p = new_node(bind); p->text = "a"; p->first = s; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "int"; p->next = q; q = p;
    p = new_node(rule); p->text = "P"; p->first = q; p->next = r; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    resolve(p, p);

    return p;
}
