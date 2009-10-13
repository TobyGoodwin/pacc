#include "syntax.h"

char *prefix = "#include <ctype.h>\n#include \"syntax.h\"\n";

struct s_node *create(void) {
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
    p = s_new(rep); p->number = 0; p->first = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "int" /* XXX: "void" */; p->next = q; q = p;
    p = s_new(rule); p->text = "_"; p->first = q; p->next = r; r = p;

    /*
	NameCont
	    ← c:Char &{ isalnum(*c) || c == '_' }
    */

    p = s_new(ident); p->text = "c"; s = p;
    p = s_new(guard); p->text = "isalnum(*c) || *c == '_'"; p->first = s; q = p;
    p = s_new(call); p->text = "Char"; s = p;
    p = s_new(bind); p->text = "c"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "int" /* XXX: "void" */; p->next = q; q = p;
    p = s_new(rule); p->text = "NameCont"; p->first = q; p->next = r; r = p;

    /*
	NameStart :: void
	    ← c:Char &{ isalpha(*c) || *c == '_' }
    */

    p = s_new(ident); p->text = "c"; s = p;
    p = s_new(guard); p->text = "isalpha(*c) || *c == '_'"; p->first = s; q = p;
    p = s_new(call); p->text = "Char"; s = p;
    p = s_new(bind); p->text = "c"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "int" /* XXX: "void" */; p->next = q; q = p;
    p = s_new(rule); p->text = "NameStart"; p->first = q; p->next = r; r = p;

    /* Char ← . { match() } */

    p = s_new(expr); p->text = "match()"; q = p;
    p = s_new(any); p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "char *"; p->next = q; q = p;
    p = s_new(rule); p->text = "Char"; p->first = q; p->next = r; r = p;

    /*
	Name :: char *
	    ← NameStart NameCont* { match() } _
    */

    p = s_new(call); p->text = "_"; q = p;
    p = s_new(expr); p->text = "match()"; p->next = q; q = p;
    p = s_new(call); p->text = "NameCont"; s = p;
    p = s_new(seq); p->first = s; s = p;
    p = s_new(rep); p->number = 0; p->first = s; p->next = q; q = p;
    p = s_new(call); p->text = "NameStart"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
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

    p = s_new(grammar); p->text = "yy"; p->first = r;

    return p;
}
