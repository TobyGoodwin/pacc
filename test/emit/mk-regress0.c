/*
chars
parse fred fred
parse 'fred ' fred
noparse 'fred barney' End 5
*/

#include <sys/types.h>

#include "syntax.h"

int parse(char *ignore0, off_t ignore1, struct s_node **result) {
    struct s_node *p, *q, *r, *s;

    /*
	End ← !.
    */

    p = new_node(any); q = p;
    p = new_node(not); p->first = q; q = p;
    p = s_new(type); p->text = "int" /* XXX: "void" */; p->next = q; q = p;
    p = s_new(rule); p->text = "End"; p->first = q; r = p;

    /* _ ← " " * */
    p = s_new(lit); p->text = " "; q = p;
    //p = s_new(seq); p->first = q; q = p;
    p = s_new(rep); p->first = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "int" /* XXX: "void" */; p->next = q; q = p;
    p = s_new(rule); p->text = "_"; p->first = q; p->next = r; r = p;

    /*
	NameCont
	    ← c:Char &{ isalnum(*ref_ptr(c)) || ref_ptr(c) == '_' }
    */

    p = s_new(ident); p->text = "c"; s = p;
    p = s_text(guard, "isalnum(*ref_ptr(c)) || *ref_ptr(c) == '_'"); p->first = s; q = p;
    p = s_new(call); p->text = "Char"; s = p;
    p = s_new(bind); p->text = "c"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "int" /* XXX: "void" */; p->next = q; q = p;
    p = s_new(rule); p->text = "NameCont"; p->first = q; p->next = r; r = p;

    /*
	NameStart :: void
	    ← c:Char &{ isalpha(*ref_ptr(c)) || *ref_ptr(c) == '_' }
    */

    p = s_new(ident); p->text = "c"; s = p;
    p = s_text(guard, "isalpha(*ref_ptr(c)) || *ref_ptr(c) == '_'"); p->first = s; q = p;
    p = s_new(call); p->text = "Char"; s = p;
    p = s_new(bind); p->text = "c"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "int" /* XXX: "void" */; p->next = q; q = p;
    p = s_new(rule); p->text = "NameStart"; p->first = q; p->next = r; r = p;

    /* Char :: ref_t ← . { ref() } */

    p = s_new(expr); p->text = "ref()"; q = p;
    p = s_new(any); p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "ref_t"; p->next = q; q = p;
    p = s_new(rule); p->text = "Char"; p->first = q; p->next = r; r = p;

    /*
	Name :: char *
	    ← n:(NameStart NameCont*) _ { ref_str(n) }
    */

    p = s_both(expr, "ref_str(n)", s_text(ident, "n"));
    p = cons(s_text(call, "_"), p);
    q = s_both(rep, 0, s_kid(seq, s_text(call, "NameCont")));
    q = cons(s_text(call, "NameStart"), q);
    q = s_both(bind, "n", s_kid(seq, q));
    q = s_kid(seq, cons(q, p));
    p = s_new(type); p->text = "char *"; p->next = q; q = p;
    p = s_new(rule); p->text = "Name"; p->first = q; p->next = r; r = p;

    /* start anywhere... */
    p = s_new(ident); p->text = "r"; s = p;
    p = s_new(expr); p->text = "r"; p->first = s; q = p;
    p = s_new(call); p->text = "End"; p->next = q; q = p;
    p = s_new(call); p->text = "Name"; s = p;
    p = s_new(bind); p->text = "r"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "char *"; p->next = q; q = p;
    p = s_new(rule); p->text = "Start"; p->first = q; p->next = r; r = p;

    r = cons(s_text(preamble, "#include <ctype.h>\n#include \"syntax.h\"\n"), r);
    p = s_new(grammar); p->text = "yy"; p->first = r;

    *result = p;
    return 1;
}
