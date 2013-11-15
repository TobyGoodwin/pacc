/*
type chars
parse bab bab
parse pen pen
parse git git
parse box box
parse tug tug
noparse sky Vowel 2
noparse end Word 1
*/

#include <sys/types.h>

#include "syntax.h"

int pacc_wrap(const char *ign0, char *ign1, off_t ign2, struct s_node **result) {
    struct s_node *p, *r;

    /*
	Word :: char * ← Consonant Vowel Consonant { ref_str() }
	Vowel :: void ← [aeiou]
	Consonant ← [b-df-hj-np-tv-z]
     */

    p = s_ccrange(s_ccnode("v"), s_ccnode("z"));
    p = append(s_ccrange(s_ccnode("p"), s_ccnode("t")), p);
    p = append(s_ccrange(s_ccnode("j"), s_ccnode("n")), p);
    p = append(s_ccrange(s_ccnode("f"), s_ccnode("h")), p);
    p = append(s_ccrange(s_ccnode("b"), s_ccnode("d")), p);
    p = s_both(cclass, "[b-df-hj-np-tv-z]", p);

    p = s_kid(seq, p);
    p = s_both(rule, "Consonant", cons(s_text(type, "void"), p));
    r = p;

    p = s_ccnode("u");
    p = cons(s_ccnode("o"), p);
    p = cons(s_ccnode("i"), p);
    p = cons(s_ccnode("e"), p);
    p = cons(s_ccnode("a"), p);
    p = s_both(cclass, "[aeiou]", p);

    p = s_kid(seq, p);
    p = s_both(rule, "Vowel", cons(s_text(type, "void"), p));
    r = cons(p, r);

    p = s_text(expr, "ref_str()");
    p = cons(s_text(call, "Consonant"), p);
    p = cons(s_text(call, "Vowel"), p);
    p = cons(s_text(call, "Consonant"), p);

    p = s_kid(seq, p);
    p = s_both(rule, "Word", cons(s_text(type, "char *"), p));
    r = cons(p, r);

    r = cons(s_text(preamble, 0), r);
    r = s_kid(grammar, r);

    *result = r;
    return 1;
}
