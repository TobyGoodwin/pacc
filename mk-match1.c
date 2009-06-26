struct s_node *create(void) {
    struct s_node *p, *q, *r, *s;

    /* Calls to rmatch() can occur anywhere:
     *
     * char *A <- '5' '6' { rmatch() } '7'
     *
     */

    /* A <- 'a' */
    p = new_node(lit); p->text = "7"; q = p;
    p = new_node(expr); p->text = "rmatch()"; p->next = q; q = p;
    p = new_node(lit); p->text = "6"; p->next = q; q = p;
    p = new_node(lit); p->text = "5"; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "A"; p->first = q; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    resolve(p, p);

    return p;
}
