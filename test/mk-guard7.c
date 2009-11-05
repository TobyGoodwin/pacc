#include "syntax.h"

char *prefix = 0;

struct s_node *create(void) {
    struct s_node *p, *q, *r, *s, *t;

    /*
	End ← !.
    */

    p = new_node(any); q = p;
    p = new_node(not); p->first = q; q = p;
    p = s_new(type); p->text = "int" /* XXX: "void" */; p->next = q; q = p;
    p = s_new(rule); p->text = "End"; p->first = q; r = p;

    /* Char ← . { match() } */

    p = s_new(expr); p->text = "match()"; q = p;
    p = s_new(any); p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "char *"; p->next = q; q = p;
    p = s_new(rule); p->text = "Char"; p->first = q; p->next = r; r = p;

    /* _ ← (" " / "\n" ) * */
    p = s_new(lit); p->text = "\n"; q = p;
    p = s_new(lit); p->text = " "; p->next = q; q = p;
    p = s_new(alt); p->first = q; q = p;
    p = s_new(rep); p->first = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "int" /* XXX: "void" */; p->next = q; q = p;
    p = s_new(rule); p->text = "_"; p->first = q; p->next = r; r = p;

    /*
	RawCode
	    ← "{" ( c:Char &{ *c != 0x7d } )* "}" { match() } _
    */

    p = s_new(call); p->text = "_"; q = p;
    p = s_new(expr); p->text = "match()"; p->next = q; q = p;
    p = s_new(lit); p->text = "}"; p->next = q; t = p;
    p = s_new(ident); p->text = "c"; s = p;
    p = s_new(guard); p->text = "*c != 0x7d"; p->first = s; q = p;
    p = s_new(call); p->text = "Char"; s = p;
    p = s_new(bind); p->text = "c"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(rep); p->first = q; p->next = t; q = p;
    p = s_new(lit); p->text = "{"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "char *"; p->next = q; q = p;
    p = s_new(rule); p->text = "RawCode"; p->first = q; p->next = r; r = p;

    /*
	Start
	    ← RawCode End { match() }
    */
    p = s_new(expr); p->text = "match()"; q = p;
    p = s_new(call); p->text = "End"; p->next = q; q = p;
    p = s_new(call); p->text = "RawCode"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "char *"; p->next = q; q = p;
    p = s_new(rule); p->text = "Start"; p->first = q; p->next = r; r = p;

    p = s_new(grammar); p->text = "yy"; p->first = r;

    return p;
}
