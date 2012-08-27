/*
chars
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

    p = s_text(ccle, "z");
    p = cons(s_text(ccge, "v"), p);
    p = cons(s_text(ccle, "t"), p);
    p = cons(s_text(ccge, "p"), p);
    p = cons(s_text(ccle, "n"), p);
    p = cons(s_text(ccge, "j"), p);
    p = cons(s_text(ccle, "h"), p);
    p = cons(s_text(ccge, "f"), p);
    p = cons(s_text(ccle, "d"), p);
    p = cons(s_text(ccge, "b"), p);
    p = s_both(cclass, "[b-df-hj-np-tv-z]", p);

    p = s_kid(seq, p);
    p = s_both(rule, "Consonant", cons(s_text(type, "void"), p));
    r = p;

    p = s_text(cceq, "u");
    p = cons(s_text(cceq, "o"), p);
    p = cons(s_text(cceq, "i"), p);
    p = cons(s_text(cceq, "e"), p);
    p = cons(s_text(cceq, "a"), p);
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
    r = s_both(grammar, "pacc", r);

    *result = r;
    return 1;
}
