struct s_node *create(void) {
    struct s_node *p, *q, *r, *s;

    /* Semantic predicate and any matcher:
     *
     * P <- a:A &{ a >= 'a' && a <= 'z' } { a }
     * A <- q:Any { q }
     * Any <- .
     *
     * This is not right, of course. We need better types!
     */

    p = new_node(any); q = p;
    p = new_node(seq); p->first = q; s = p;
    p = new_node(rule); p->text = "Any"; p->first = q; r = p;

    p = new_node(expr); p->text = "a"; q = p;
    p = new_node(call); p->text = "Any"; s = p;
    p = new_node(bind); p->text = "a"; p->first = s; p->next = q; q = p;
    p = new_node(seq); p->first = q; s = p;
    p = new_node(rule); p->text = "A"; p->first = q; p->next = r; r = p;

    p = new_node(expr); p->text = "b"; q = p;
    p = new_node(ident); p->text = "b"; s = p;
    p = new_node(guard); p->text = "b>='0' && b<='9'"; p->first = s; p->next = q; q = p;
    p = new_node(call); p->text = "A"; s = p;
    p = new_node(bind); p->text = "b"; p->first = s; p->next = q; q = p;
    p = new_node(ident); p->text = "a"; s = p;
    p = new_node(guard); p->text = "a>='a' && a<='z'"; p->first = s; p->next = q; q = p;
    p = new_node(call); p->text = "A"; s = p;
    p = new_node(bind); p->text = "a"; p->first = s; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(rule); p->text = "P"; p->first = q; p->next = r; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    resolve(p, p);

    return p;
}
