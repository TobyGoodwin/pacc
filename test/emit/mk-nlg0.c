/*
chars
parse a a
parse aac aac
parse aaaac aaaac
parse aaaaaac aaaaaac
parse aaaaaaaac aaaaaaaac
parse aaaaaaaaaac aaaaaaaaaac
parse aaaaaaaaaaaac aaaaaaaaaaaac
*/

#include <sys/types.h>

#include "syntax.h"

int parse(char *ignore0, off_t ignore1, struct s_node **result) {
    struct s_node *p, *q, *r, *s;

    /* This grammar is, of course, linear for a properly working packrat
     * parser. But it is exponential if packrat memoization is turned
     * off, or if * is implemented without memoization. Thanks to Bryan
     * Ford (communication on PEG mailing list 2009-03-20).
     *
     * S ← A* { ref_str() }
     * A ← a S b / a S c / a
     *
     */

    p = new_node(lit); p->text = "a"; q = p;
    p = new_node(seq); p->first = q; s = p;

    p = new_node(lit); p->text = "c"; q = p;
    p = new_node(call); p->text = "S"; p->next = q; q = p;
    p = new_node(lit); p->text = "a"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; s = p;

    p = new_node(lit); p->text = "b"; q = p;
    p = new_node(call); p->text = "S"; p->next = q; q = p;
    p = new_node(lit); p->text = "a"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; s = p;

    p = new_node(alt); p->first = s; q = p;
    p = new_node(type); p->text = "int"; p->next = q; q = p;
    p = new_node(rule); p->text = "A"; p->first = q; r = p;

    p = s_text(call, "A"), p; q = p;
    p = s_text(rep, 0); p->first = q; q = p;
    p = s_kid(seq, cons(p, s_text(expr, "ref_str()"))); q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "S"; p->first = q; p->next = r; r = p;

    r = cons(s_text(preamble, 0), r);
    p = new_node(grammar); p->text = "yy"; p->first = r;

    *result = p;
    return 1;
}
