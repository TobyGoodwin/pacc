#include "syntax.h"

char *prefix = 0;

struct s_node *create(void) {
    struct s_node *p, *q, *r, *s;

    /* This grammar is, of course, linear for a properly working packrat
     * parser. But it is exponential if packrat memoization is turned
     * off, or if * is implemented without memoization. Thanks to Bryan
     * Ford (communication on PEG mailing list 2009-03-20).
     *
     * S ← A*
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

    p = new_node(call); p->text = "A"; q = p;
    p = new_node(rep); p->number = 0; p->first = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "int"; p->next = q; q = p;
    p = new_node(rule); p->text = "S"; p->first = q; p->next = r; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    resolve(p, p);

    return p;
}
