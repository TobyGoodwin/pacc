struct s_node *create(void) {
    struct s_node *p, *q, *r, *s, *t;

    /* Bryan Ford's trivial grammar with actions:
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
