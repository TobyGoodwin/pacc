#include "syntax.h"

char *prefix = "char r[9]; int i = 0;";

struct s_node *create(void) {
    struct s_node *p, *r;

    /*
	S ← R* E { r[i++] = '\0', r }
	R ← a:C "." D* { r[i++] = *a }
	C ← . { match() }
	D ← . { match() } ! "."
	E ← ! .
     */

    p = s_new(any);
    p = s_kid(not, p);
    p = s_kid(seq, p);
    p = cons(s_text(type, "int"), p);
    p = s_both(rule, "E", p);
    r = p;

    p = s_text(lit, ".");
    p = s_kid(not, p);
    p = cons(s_text(expr, "match()"), p);
    p = cons(s_new(any), p);
    p = s_kid(seq, p);
    p = cons(s_text(type, "char *"), p);
    p = s_both(rule, "D", p);
    r = cons(p, r);

    p = s_text(expr, "match()");
    p = cons(s_new(any), p);
    p = s_kid(seq, p);
    p = cons(s_text(type, "char *"), p);
    p = s_both(rule, "C", p);
    r = cons(p, r);

    p = s_both(expr, "r[i++] = *a", s_text(ident, "a"));
    p = cons(s_both(rep, "", s_text(call, "D")), p);
    p = cons(s_text(lit, "."), p);
    p = cons(s_both(bind, "a", s_text(call, "C")), p);
    p = s_kid(seq, p);
    p = cons(s_text(type, "int"), p);
    p = s_both(rule, "R", p); r = cons(p, r);

    p = s_text(expr, "r[i++] = '\\0', r");
    p = cons(s_text(call, "E"), p);
    p = cons(s_both(rep, "", s_kid(seq, s_text(call, "R"))), p);
    p = s_kid(seq, p);
    p = cons(s_text(type, "char *"), p);
    p = s_both(rule, "S", p);
    r = cons(p, r);

    p = s_both(grammar, "yy", r);

    return p;
}
