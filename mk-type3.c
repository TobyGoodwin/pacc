struct s_node *create(void) {
    struct s_node *p, *q, *r, *s;

    /* Mixed types:
     *
     * int Decimal ← d:Digits1 { atoi(d) }
     * char *Digits1 ← Digit Digits1 { match() } / Digit { match() }
     * char *Digit ← '0' / '1' / ... / '9'
     *
     */

    /* char *Digit <- '0' / '1' / ... / '9' */
    p = new_node(lit); p->text = "9"; q = p;
    p = new_node(seq); p->first = q; s = p;
    p = new_node(lit); p->text = "8"; q = p;
    p = new_node(seq); p->first = q; p->next = s; s = p;
    p = new_node(lit); p->text = "7"; q = p;
    p = new_node(seq); p->first = q; p->next = s; s = p;
    p = new_node(lit); p->text = "6"; q = p;
    p = new_node(seq); p->first = q; p->next = s; s = p;
    p = new_node(lit); p->text = "5"; q = p;
    p = new_node(seq); p->first = q; p->next = s; s = p;
    p = new_node(lit); p->text = "4"; q = p;
    p = new_node(seq); p->first = q; p->next = s; s = p;
    p = new_node(lit); p->text = "3"; q = p;
    p = new_node(seq); p->first = q; p->next = s; s = p;
    p = new_node(lit); p->text = "2"; q = p;
    p = new_node(seq); p->first = q; p->next = s; s = p;
    p = new_node(lit); p->text = "1"; q = p;
    p = new_node(seq); p->first = q; p->next = s; s = p;
    p = new_node(lit); p->text = "0"; q = p;
    p = new_node(seq); p->first = q; p->next = s; s = p;

    p = new_node(alt); p->first = s; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "Digit"; p->first = q; r = p;

    /* char *Digits1 ← Digit Digits1 { match() } / Digit { match() } */
    p = new_node(expr); p->text = "match()"; q = p;
    p = new_node(call); p->text = "Digit"; p->next = q; q = p;
    p = new_node(seq); p->first = q; s = p;
    p = new_node(expr); p->text = "match()"; q = p;
    p = new_node(call); p->text = "Digits1"; p->next = q; q = p;
    p = new_node(call); p->text = "Digit"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "Digits1"; p->first = q; p->next = r; r = p;

    /* int Decimal ← Digits1 { atoi(match()) } */
    p = new_node(expr); p->text = "atoi(d)"; q = p;
    p = new_node(call); p->text = "Digits1"; s = p;
    p = new_node(bind); p->text = "d"; p->first = s; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "int"; p->next = q; q = p;
    p = new_node(rule); p->text = "Decimal"; p->first = q; p->next = r; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    resolve(p, p);

    return p;
}
