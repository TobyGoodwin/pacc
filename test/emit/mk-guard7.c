/*
type chars
parse {} {}
parse {_any_chars_here_} {_any_chars_here_}
noparse {_no '"}"' 5
*/

#include <sys/types.h>

#include "syntax.h"

int pacc_wrap(const char *ign0, char *ign1, off_t ign2, struct s_node **result) {
    struct s_node *p, *q, *r, *s, *t;

    /* Char ← . { ref_str() } */

    p = s_new(expr); p->text = "ref_str()"; q = p;
    p = s_new(any); p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "char *"; p->next = q; q = p;
    p = s_new(rule); p->text = "Char"; p->first = q; p->next = 0; r = p;

    /* _ ← (" " / "\n" ) * */
    p = s_new(lit); p->text = "\\n"; q = p;
    p = s_new(lit); p->text = " "; p->next = q; q = p;
    p = s_new(alt); p->first = q; q = p;
    p = s_new(rep); p->first = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "void"; p->next = q; q = p;
    p = s_new(rule); p->text = "_"; p->first = q; p->next = r; r = p;

    /*
	RawCode
	    ← "{" ( c:Char &{ *c != 0x7d } )* "}" { ref_str() } _
    */

    p = s_new(call); p->text = "_"; q = p;
    p = s_new(expr); p->text = "ref_str()"; p->next = q; q = p;
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
	    ← RawCode { ref_str() }
    */
    p = s_new(expr); p->text = "ref_str()"; q = p;
    p = s_new(call); p->text = "RawCode"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "char *"; p->next = q; q = p;
    p = s_new(rule); p->text = "Start"; p->first = q; p->next = r; r = p;

    r = cons(s_text(preamble, 0), r);
    p = s_kid(grammar, r);

    *result = p;
    return 1;
}
