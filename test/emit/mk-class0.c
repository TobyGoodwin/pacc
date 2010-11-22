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

int parse(const char *ign0, char *ign1, off_t ign2, struct s_node **result) {
    struct s_node *p, *r;

    /*
	Word :: char * ← Consonant Vowel Consonant { ref_str() }
	Vowel ← [aeiou]
	Consonant ← [b-df-hj-np-tv-z]
     */

    p = s_text(crange, "<z");
    p = cons(s_text(crange, ">v"), p);
    p = cons(s_text(crange, "<t"), p);
    p = cons(s_text(crange, ">p"), p);
    p = cons(s_text(crange, "<n"), p);
    p = cons(s_text(crange, ">j"), p);
    p = cons(s_text(crange, "<h"), p);
    p = cons(s_text(crange, ">f"), p);
    p = cons(s_text(crange, "<d"), p);
    p = cons(s_text(crange, ">b"), p);
    p = s_kid(cclass, p);

    p = s_kid(seq, p);
    p = s_both(rule, "Consonant", cons(s_text(type, "char *"), p));
    r = p;

    p = s_text(crange, "=u");
    p = cons(s_text(crange, "=o"), p);
    p = cons(s_text(crange, "=i"), p);
    p = cons(s_text(crange, "=e"), p);
    p = cons(s_text(crange, "=a"), p);
    p = s_kid(cclass, p);

    p = s_kid(seq, p);
    p = s_both(rule, "Vowel", cons(s_text(type, "char *"), p));
    r = cons(p, r);

    p = s_text(expr, "ref_str()");
    p = cons(s_text(call, "Consonant"), p);
    p = cons(s_text(call, "Vowel"), p);
    p = cons(s_text(call, "Consonant"), p);

    p = s_kid(seq, p);
    p = s_both(rule, "Word", cons(s_text(type, "char *"), p));
    r = cons(p, r);

    r = cons(s_text(preamble, 0), r);
    r = s_both(grammar, "yy", r);

    *result = r;
    return 1;
}
