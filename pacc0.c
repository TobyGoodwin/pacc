#include "pacc.h"
#include "syntax.h"

char *prefix = 0; /* XXX */

int pacc_wrap(
	__attribute__((unused)) const char *ign0,
	__attribute__((unused)) char *ign1,
	__attribute__((unused)) off_t ign2,
	struct s_node **result) {
    struct s_node *i, *p, *q, *r, *s, *t;

    /*
	Star  ← "*" _
    */

    p = s_new(call); p->text = "_"; q = p;
    p = s_new(lit); p->text = "*"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "void"; p->next = q; q = p;
    p = s_new(rule); p->text = "Star"; p->first = q; r = p;

    /*
	Slash ← "/" _
    */

    p = s_new(call); p->text = "_"; q = p;
    p = s_new(lit); p->text = "/"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "void"; p->next = q; q = p;
    p = s_new(rule); p->text = "Slash"; p->first = q; p->next = r; r = p;

    /*
	Query ← "?" _
    */

    p = s_new(call); p->text = "_"; q = p;
    p = s_new(lit); p->text = "?"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "void"; p->next = q; q = p;
    p = s_new(rule); p->text = "Query"; p->first = q; p->next = r; r = p;

    /*
	Plus  ← "+" _
    */

    p = s_new(call); p->text = "_"; q = p;
    p = s_new(lit); p->text = "+"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "void"; p->next = q; q = p;
    p = s_new(rule); p->text = "Plus"; p->first = q; p->next = r; r = p;

    /*
	rParen  ← ")" _
    */

    p = s_new(call); p->text = "_"; q = p;
    p = s_new(lit); p->text = ")"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "void"; p->next = q; q = p;
    p = s_new(rule); p->text = "rParen"; p->first = q; p->next = r; r = p;

    /*
	lParen  ← "(" _
    */

    p = s_new(call); p->text = "_"; q = p;
    p = s_new(lit); p->text = "("; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "void"; p->next = q; q = p;
    p = s_new(rule); p->text = "lParen"; p->first = q; p->next = r; r = p;

    /*
	Not  ← "!" _
    */

    p = s_new(call); p->text = "_"; q = p;
    p = s_new(lit); p->text = "!"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "void"; p->next = q; q = p;
    p = s_new(rule); p->text = "Not"; p->first = q; p->next = r; r = p;

    /*
	Epsilon ← "ε" _
     */

    p = s_text(call, "_");
    p = cons(s_text(lit, "ε"), p);
    p = s_kid(seq, p);
    p = cons(s_text(type, "void"), p);
    r = cons(s_both(rule, "Epsilon", p), r);

    /*
	Dot ← "." _
     */

    p = s_text(call, "_");
    p = cons(s_text(lit, "."), p);
    p = s_kid(seq, p);
    p = cons(s_text(type, "void"), p);
    r = cons(s_both(rule, "Dot", p), r);

    /*
	ColCol ← "::" _
    */

    p = s_new(call); p->text = "_"; q = p;
    p = s_new(lit); p->text = "::"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "void"; p->next = q; q = p;
    p = s_new(rule); p->text = "ColCol"; p->first = q; p->next = r; r = p;

    /*
	Colon ← ":" _
    */

    p = s_new(call); p->text = "_"; q = p;
    p = s_new(lit); p->text = ":"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "void"; p->next = q; q = p;
    p = s_new(rule); p->text = "Colon"; p->first = q; p->next = r; r = p;

    /*
	lArrow  ← ("←" / "<-") _
    */

    p = s_new(call); p->text = "_"; q = p;
    p = s_new(lit); p->text = "<-"; t = p;
    p = s_new(lit); p->text = "←"; p->next = t; t = p;
    p = s_new(alt); p->first = t; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "void"; p->next = q; q = p;
    p = s_new(rule); p->text = "lArrow"; p->first = q; p->next = r; r = p;

    /*
	rArrow  ← ("→" / "->") _
    */

    p = s_new(call); p->text = "_"; q = p;
    p = s_new(lit); p->text = "->"; t = p;
    p = s_new(lit); p->text = "→"; p->next = t; t = p;
    p = s_new(alt); p->first = t; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "void"; p->next = q; q = p;
    p = s_new(rule); p->text = "rArrow"; p->first = q; p->next = r; r = p;

    /*
	And  ← "&" _
    */

    p = s_new(call); p->text = "_"; q = p;
    p = s_new(lit); p->text = "&"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "void"; p->next = q; q = p;
    p = s_new(rule); p->text = "And"; p->first = q; p->next = r; r = p;

    /*
	_
	    ← (" " / "\t" / "\n" / Comment) *
    */

    p = s_text(call, "Comment");
    p = cons(s_kid(seq, s_text(lit, "\\n")), p);
    p = cons(s_kid(seq, s_text(lit, "\\t")), p);
    p = cons(s_kid(seq, s_text(lit, " ")), p);
    p = s_kid(seq, s_both(rep, 0, s_kid(seq, s_kid(alt, p))));
    p = cons(s_text(type, "void"), p);
    p = s_both(rule, "_", p);
    r = cons(p, r);

    /*
	C_Comment
	    ← "//" (!"\n" .)* "\n"
	    / "/_*" (!"*_/" .)* "*_/"
    */

    p = s_kid(not, s_text(lit, "*/"));
    p = s_kid(seq, cons(p, s_new(any)));
    p = s_both(rep, 0, p);
    t = s_kid(seq, cons(s_text(lit, "/*"), cons(p, s_text(lit, "*/"))));
    p = s_kid(not, s_text(lit, "\\n"));
    p = s_kid(seq, cons(p, s_new(any)));
    p = s_both(rep, 0, p);
    p = s_kid(seq, cons(s_text(lit, "//"), cons(p, s_text(lit, "\\n"))));
    p = s_kid(alt, cons(p, t));
    p = cons(s_text(type, "void"), p);
    p = s_both(rule, "C_Comment", p);
    r = cons(p, r);

    /*
	Comment
	    ← "#" (!"\n" .)* "\n"
	    / C_Comment
    */
    t = s_kid(seq, s_text(call, "C_Comment"));
    p = s_kid(not, s_text(lit, "\\n"));
    p = s_kid(seq, cons(p, s_new(any)));
    p = s_both(rep, 0, p);
    p = s_kid(seq, cons(s_text(lit, "#"), cons(p, s_text(lit, "\\n"))));
    p = s_kid(alt, cons(p, t));
    p = cons(s_text(type, "void"), p);
    p = s_both(rule, "Comment", p);
    r = cons(p, r);

    /*
	TypeElement
	    ← Name _ / "*" _
    */

    p = s_new(call); p->text = "_"; q = p;
    p = s_new(lit); p->text = "*"; p->next = q; q = p;
    p = s_new(seq); p->first = q; t = p;

    p = s_new(call); p->text = "_"; q = p;
    p = s_new(call); p->text = "Name"; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = t; t = p;

    p = s_new(alt); p->first = t; q = p;
    p = s_new(type); p->text = "void"; p->next = q; q = p;
    p = s_new(rule); p->text = "TypeElement"; p->first = q; p->next = r; r = p;

    /*
	QuotedChar
	    ← c:SimpleCharEscape → c
	    / !"\\" x:. → { ref_dup(x) }
    */

    p = s_both(expr, "ref_dup(x)", s_text(ident, "x"));
    p = cons(s_both(bind, "x", s_new(any)), p);
    p = cons(s_kid(not, s_text(lit, "\\\\")), p);
    p = s_kid(seq, p);

    q = s_both(expr, "c", s_text(ident, "c"));
    q = cons(s_both(bind, "c", s_text(call, "SimpleCharEscape")), q);
    q = s_kid(seq, q);

    p = s_kid(alt, cons(q, p));
    p = cons(s_text(type, "char *"), p);
    p = s_both(rule, "QuotedChar", p);
    r = cons(p, r);

    /*
	SimpleCharEscape
	    ← "\\" a:("\'" / "\"" / "\?" / "\\"
	    / "a" / "b" / "f" / "n" / "r" / "t" / "v") { ref_dup(a) }
    */

    p = s_both(expr, "ref_dup(a)", s_text(ident, "a"));
    q = s_kid(seq, s_text(lit, "v"));
    q = cons(s_kid(seq, s_text(lit, "t")), q);
    q = cons(s_kid(seq, s_text(lit, "r")), q);
    q = cons(s_kid(seq, s_text(lit, "n")), q);
    q = cons(s_kid(seq, s_text(lit, "f")), q);
    q = cons(s_kid(seq, s_text(lit, "b")), q);
    q = cons(s_kid(seq, s_text(lit, "a")), q);
    q = cons(s_kid(seq, s_text(lit, "?")), q);
    q = cons(s_kid(seq, s_text(lit, "\\\\")), q);
    q = cons(s_kid(seq, s_text(lit, "\\\"")), q);
    q = cons(s_kid(seq, s_text(lit, "'")), q);
    q = s_both(bind, "a", s_kid(alt, q));
    p = cons(s_text(lit, "\\\\"), cons(q, p));
    p = s_kid(seq, p);
    p = cons(s_text(type, "ref_t"), p);
    p = s_both(rule, "SimpleCharEscape", p);
    r = cons(p, r);

    /*
	CharLit :: void
	    ← "'" QuotedChar "'" _
     */
    
    p = s_text(call, "_");
    p = cons(s_text(lit, "'"), p);
    p = cons(s_text(call, "QuotedChar"), p);
    p = cons(s_text(lit, "'"), p);
    p = s_kid(seq, p);
    p = cons(s_text(type, "void"), p);
    p = s_both(rule, "CharLit", p);
    r = cons(p, r);

    /*
	QuotedChars ← (!"\"" QuotedChar)* { ref_str() }
    */

    p = s_new(expr); p->text = "ref_str()"; t = p;
    p = s_new(call); p->text = "QuotedChar"; q = p;
    p = s_new(lit); p->text = "\\\""; s = p;
    p = s_new(not); p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(rep); p->first = q; p->next = t; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "char *"; p->next = q; q = p;
    p = s_new(rule); p->text = "QuotedChars"; p->first = q; p->next = r; r = p;

    /*
	StringLit ← "\"" q:QuotedChars "\"" _ → q
    */

    p = s_new(ident); p->text = "q"; i = p;
    p = s_new(expr); p->text = "q"; p->first = i; q = p;
    p = s_new(call); p->text = "_"; p->next = q; q = p;
    p = s_new(lit); p->text = "\\\""; p->next = q; q = p;
    p = s_new(call); p->text = "QuotedChars"; s = p;
    p = s_new(bind); p->text = "q"; p->first = s; p->next = q; q = p;
    p = s_new(lit); p->text = "\\\""; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "char *"; p->next = q; q = p;
    p = s_new(rule); p->text = "StringLit"; p->first = q; p->next = r; r = p;

    /*
        XXX probably not necessary any more
	Char :: ref_t
	    ← . { ref() }
    */

    p = s_new(expr); p->text = "ref()"; q = p;
    p = s_new(any); p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "ref_t"; p->next = q; q = p;
    p = s_new(rule); p->text = "Char"; p->first = q; p->next = r; r = p;

    /*
	TypeOptional
	    ← ColCol TypeElement+ { s_stash_type(ref_str()) }
	    / ε { s_stashed_type() }
    */
    p = s_new(expr); p->text = "s_stashed_type()"; q = p;
    p = s_new(seq); p->first = q; t = p;

    p = s_text(expr, "s_stash_type(ref_str())");
    q = s_text(call, "TypeElement");
    p = cons(s_both(rep, "1,", s_kid(seq, q)), p); q = p;
    p = s_new(call); p->text = "ColCol"; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = t; t = p;

    p = s_kid(seq, s_kid(alt, t)); q = p;
    p = s_new(type); p->text = "char *"; p->next = q; q = p;
    p = s_new(rule); p->text = "TypeOptional"; p->first = q; p->next = r; r = p;

    /*
	Name :: char *
	    ← n:([_A-Za-z] [_A-Za-z0-9]*) _ { ref_dup(n) }
    */
    p = s_both(expr, "ref_dup(n)", s_text(ident, "n"));
    p = cons(s_text(call, "_"), p);
    q = cons(s_num(ccge, '0'), s_num(ccle, '9'));
    q = cons(s_num(ccge, 'a'), cons(s_num(ccle, 'z'), q));
    q = cons(s_num(ccge, 'A'), cons(s_num(ccle, 'Z'), q));
    q = cons(s_num(cceq, '_'), q);
    q = s_both(cclass, "", q);
    q = s_both(rep, 0, q);
    s = cons(s_num(ccge, 'a'), s_num(ccle, 'z'));
    s = cons(s_num(ccge, 'A'), cons(s_num(ccle, 'Z'), s));
    s = cons(s_num(cceq, '_'), s);
    s = s_both(cclass, "", s);
    q = s_kid(seq, cons(s, q));
    q = s_both(bind, "n", q);
    p = s_kid(seq, cons(q, p));
    p = cons(s_text(type, "char *"), p);
    p = s_both(rule, "Name", p);
    r = cons(p, r);

    /*
	Preamble
	    ← c:Code { s_retype(preamble, c) } _
	    / ε { s_text(preamble, 0) }
    */

    p = s_text(expr, "s_text(preamble, 0)");
    t = s_kid(seq, p);

    p = s_both(expr, "s_retype(preamble, c)", s_text(ident, "c"));
    p = cons(s_both(bind, "c", s_text(call, "Code")), p);
    t = cons(s_kid(seq, p), t);

    p = s_new(alt); p->first = t; q = p;
    p = s_new(type); p->text = "struct s_node *"; p->next = q; q = p;
    p = s_new(rule); p->text = "Preamble"; p->first = q; p->next = r; r = p;

    /*
	CodeStart
	    ← "{" → { s_coords(pacc_coords) }
    */
    p = s_text(expr, "s_coords(pacc_coords)");
    p = cons(s_text(lit, "{"), p);
    p = s_kid(seq, p);
    p = cons(s_text(type, "struct s_node *"), p);
    p = s_both(rule, "CodeStart", p);
    r = cons(p, r);

    /*
	Code
	    ← l:CodeStart n:CodeAndNames "}" _ → { s_child_cons(n, l) }
     */

    /* XXX probably want lBrace and rBrace */

    p = s_text(ident, "l");
    p = cons(s_text(ident, "n"), p);
    p = s_both(expr, "s_child_cons(n, l)", p);
    p = cons(s_text(call, "_"), p);
    p = cons(s_text(lit, "}"), p);
    p = cons(s_both(bind, "n", s_text(call, "CodeAndNames")), p);
    p = cons(s_both(bind, "l", s_text(call, "CodeStart")), p);
    p = s_kid(seq, p);
    p = cons(s_text(type, "struct s_node *"), p);
    p = s_both(rule, "Code", p);
    r = cons(p, r);

    /*
	CodeAndNames
	    ← n:CodeNames { s_both(expr, ref_str(), n) }
     */

    p = s_both(expr, "s_both(expr, ref_str(), n)", s_text(ident, "n"));
    p = cons(s_both(bind, "n", s_text(call, "CodeNames")), p);
    p = s_kid(seq, p);
    p = cons(s_text(type, "struct s_node *"), p);
    r = cons(s_both(rule, "CodeAndNames", p), r);

    /*
	CodeNames
	    ← n:Name ns:CodeNames { s_set_cons(s_text(ident, n), ns) }
	    / StringLit ns:CodeNames → ns
	    / CharLit ns:CodeNames → ns
	    / C_Comment ns:CodeNames → ns 
	    / c:Char &{ ref_0(c) != '}' } ns:CodeNames → ns
	    / ε { 0 }
     */

    p = s_text(expr, "0");
    p = s_kid(seq, p);
    t = p;

    p = s_both(expr, "ns", s_text(ident, "ns"));
    p = cons(s_both(bind, "ns", s_text(call, "CodeNames")), p);
    p = cons(s_both(guard, "ref_0(c) != '}'", s_text(ident, "c")), p);
    p = cons(s_both(bind, "c", s_text(call, "Char")), p);
    p = s_kid(seq, p);
    t = cons(p, t);

    p = s_both(expr, "ns", s_text(ident, "ns"));
    p = cons(s_both(bind, "ns", s_text(call, "CodeNames")), p);
    p = cons(s_text(call, "C_Comment"), p);
    p = s_kid(seq, p);
    t = cons(p, t);

    p = s_both(expr, "ns", s_text(ident, "ns"));
    p = cons(s_both(bind, "ns", s_text(call, "CodeNames")), p);
    p = cons(s_text(call, "CharLit"), p);
    p = s_kid(seq, p);
    t = cons(p, t);

    p = s_both(expr, "ns", s_text(ident, "ns"));
    p = cons(s_both(bind, "ns", s_text(call, "CodeNames")), p);
    p = cons(s_text(call, "StringLit"), p);
    p = s_kid(seq, p);
    t = cons(p, t);

    p = cons(s_text(ident, "n"), s_text(ident, "ns"));
    p = s_both(expr, "s_set_cons(s_text(ident, n), ns)", p);
    p = cons(s_both(bind, "ns", s_text(call, "CodeNames")), p);
    p = cons(s_both(bind, "n", s_text(call, "Name")), p);
    p = s_kid(seq, p);
    t = cons(p, t);

    p = s_kid(alt, t);
    p = cons(s_text(type, "struct s_node *"), p);
    p = s_both(rule, "CodeNames", p);
    r = cons(p, r);

    /*
	Result
	    ← rArrow n:Name { s_both(expr, n, s_text(ident, n)) }
	    / rArrow? c:Code → c
    */

    p = s_both(expr, "c", s_text(ident, "c"));
    p = cons(s_both(bind, "c", s_text(call, "Code")), p);
    p = cons(s_both(rep, ",1", s_kid(seq, s_text(call, "rArrow"))), p);
    t = s_kid(seq, p);

    p = s_new(ident); p->text = "n"; i = p;
    p = s_new(expr); p->text = "s_both(expr, n, s_text(ident, n))"; p->first = i; q = p;
    p = s_new(call); p->text = "Name"; s = p;
    p = s_new(bind); p->text = "n"; p->first = s; p->next = q; q = p;
    p = s_text(call, "rArrow"); p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = t; t = p;

    p = s_new(alt); p->first = t; q = p;
    p = s_new(type); p->text = "struct s_node *"; p->next = q; q = p;
    p = s_new(rule); p->text = "Result"; p->first = q; p->next = r; r = p;

    /*
	CChar :: char *
	    ← "\\]" { "]" } / ! "]" y:QuotedChar → y
    */
    p = s_both(expr, "y", s_text(ident, "y"));
    p = cons(s_both(bind, "y", s_text(call, "QuotedChar")), p);
    p = cons(s_kid(not, s_text(lit, "]")), p);
    p = s_kid(seq, p);
    q = s_text(expr, "\"]\"");
    q = cons(s_text(lit, "\\\\]"), q);
    q = s_kid(seq, q);
    p = s_kid(alt, cons(q, p));
    p = cons(s_text(type, "char *"), p);
    p = s_both(rule, "CChar", p);
    r = cons(p, r);

    /*
	CRange
	    ← a:CChar "-" b:CChar { s_range2(a, b) }
	    / a:CChar { s_range1(a) }
    */
    p = s_both(expr, "s_range1(a)", s_text(ident, "a"));
    p = cons(s_both(bind, "a", s_text(call, "CChar")), p);
    p = s_kid(seq, p);
    q = cons(s_text(ident, "a"), s_text(ident, "b"));
    q = s_both(expr, "s_range2(a, b)", q);
    q = cons(s_both(bind, "b", s_text(call, "CChar")), q);
    q = cons(s_text(lit, "-"), q);
    q = cons(s_both(bind, "a", s_text(call, "CChar")), q);
    q = s_kid(seq, q);
    p = s_kid(alt, cons(q, p));
    p = cons(s_text(type, "struct s_node *"), p);
    p = s_both(rule, "CRange", p);
    r = cons(p, r);

    /*
	CRanges
	    ← c:CRange cs:CRanges { append(c, cs) }
	    / ε { 0 }
    */
    s = s_kid(seq, s_text(expr, "0"));
    p = cons(s_text(ident, "c"), s_text(ident, "cs"));
    p = s_both(expr, "append(c, cs)", p);
    p = cons(s_both(bind, "cs", s_text(call, "CRanges")), p);
    p = cons(s_both(bind, "c", s_text(call, "CRange")), p);
    p = s_kid(seq, p);
    p = s_kid(alt, cons(p, s));
    p = s_both(rule, "CRanges", cons(s_text(type, "struct s_node *"), p));
    r = cons(p, r);

    /*
	CClass1
	    ← "^" c:CRanges → { s_both(cclass, ref_str(), c) }
    */
    p = s_text(ident, "c");
    p = s_both(expr, "s_both(cclass, ref_str(), c)", p);
    p = cons(s_both(bind, "c", s_text(call, "CRanges")), p);
    p = cons(s_both(rep, ",1", s_text(lit, "^")), p);
    p = s_kid(seq, p);
    p = s_both(rule, "CClass1", cons(s_text(type, "struct s_node *"), p));
    r = cons(p, r);

    /*
	CClass0
	    ← "[" c:CClass1 "]" _ → c
    */
    p = s_both(expr, "c", s_text(ident, "c"));
    p = cons(s_text(call, "_"), p);
    p = cons(s_text(lit, "]"), p);
    p = cons(s_both(bind, "c", s_text(call, "CClass1")), p);
    p = cons(s_text(lit, "["), p);
    p = s_kid(seq, p);
    p = s_both(rule, "CClass0", cons(s_text(type, "struct s_node *"), p));
    r = cons(p, r);

    /*
	Rule6
	    ← lParen r:Rule0 rParen → r
     */

    p = s_both(expr, "r", s_text(ident, "r"));
    p = cons(s_text(call, "rParen"), p);
    p = cons(s_both(bind, "r", s_text(call, "Rule0")), p);
    p = cons(s_text(call, "lParen"), p);

    p = s_kid(seq, p);
    p = s_both(rule, "Rule6", cons(s_text(type, "struct s_node *"), p));
    r = cons(p, r);

    /*
	Rule5
	    ← n:Name !lArrow !ColCol { s_text(call, n) }
	    / Dot { s_new(any) }
	    / l:StringLit { s_text(lit, l) }
	    / c:CClass0 → c
	    / r:Rule6 → r
     */

    p = s_both(expr, "r", s_text(ident, "r"));
    p = cons(s_both(bind, "r", s_text(call, "Rule6")), p);
    s = s_kid(seq, p);

    p = s_both(expr, "c", s_text(ident, "c"));
    p = cons(s_both(bind, "c", s_text(call, "CClass0")), p);
    s = cons(s_kid(seq, p), s);

    p = s_both(expr, "s_text(lit, l)", s_text(ident, "l"));
    p = cons(s_both(bind, "l", s_text(call, "StringLit")), p);
    s = cons(s_kid(seq, p), s);

    p = s_text(expr, "s_new(any)");
    p = cons(s_text(call, "Dot"), p);
    s = cons(s_kid(seq, p), s);

    p = s_both(expr, "s_text(call, n)", s_text(ident, "n"));
    p = cons(s_kid(not, s_text(call, "ColCol")), p);
    p = cons(s_kid(not, s_text(call, "lArrow")), p);
    p = cons(s_both(bind, "n", s_text(call, "Name")), p);
    s = cons(s_kid(seq, p), s);

    p = s_kid(alt, s);
    p = s_both(rule, "Rule5", cons(s_text(type, "struct s_node *"), p));
    r = cons(p, r);

    /*
	Rule4
	    ← r:Rule5 Query { s_both(rep, ",1", r) }
	    / r:Rule5 Star { s_both(rep, 0, r) }
	    / r:Rule5 Plus { s_both(rep, "1,", r) }
	    / r:Rule5 → r
     */

    p = s_both(expr, "r", s_text(ident, "r"));
    p = cons(s_both(bind, "r", s_text(call, "Rule5")), p);
    s = s_kid(seq, p);

    p = s_both(expr, "s_both(rep, \"1,\", r)", s_text(ident, "r"));
    p = cons(s_text(call, "Plus"), p);
    p = cons(s_both(bind, "r", s_text(call, "Rule5")), p);
    s = cons(s_kid(seq, p), s);

    p = s_text(ident, "r");
    p = s_both(expr, "s_both(rep, 0, r)", p);
    p = cons(s_text(call, "Star"), p);
    p = cons(s_both(bind, "r", s_text(call, "Rule5")), p);
    s = cons(s_kid(seq, p), s);

    p = s_text(ident, "r");
    p = s_both(expr, "s_both(rep, \",1\", r)", p);
    p = cons(s_text(call, "Query"), p);
    p = cons(s_both(bind, "r", s_text(call, "Rule5")), p);
    s = cons(s_kid(seq, p), s);

    p = s_kid(alt, s);
    p = s_both(rule, "Rule4", cons(s_text(type, "struct s_node *"), p));
    r = cons(p, r);

    /*
	Rule3
	    ← And u:Rule4 { s_kid(and, u) }
	    / Not u:Rule4 { s_kid(not, u) }
	    / And c:Code { s_retype(guard, c) }
	    / n:Name Colon u:Rule4 { s_both(bind, n, u) }
	    / l:StringLit Colon u:Rule4 { s_both(lit, l, u) }
	    / u:Rule4 → u
     */

    p = s_both(expr, "u", s_text(ident, "u"));
    p = cons(s_both(bind, "u", s_text(call, "Rule4")), p);
    s = s_kid(seq, p);

    p = cons(s_text(ident, "l"), s_text(ident, "u"));
    p = s_both(expr, "s_both(lit, l, u)", p);
    p = cons(s_both(bind, "u", s_text(call, "Rule4")), p);
    p = cons(s_text(call, "Colon"), p);
    p = cons(s_both(bind, "l", s_text(call, "StringLit")), p);
    s = cons(s_kid(seq, p), s);

    p = cons(s_text(ident, "n"), s_text(ident, "u"));
    p = s_both(expr, "s_both(bind, n, u)", p);
    p = cons(s_both(bind, "u", s_text(call, "Rule4")), p);
    p = cons(s_text(call, "Colon"), p);
    p = cons(s_both(bind, "n", s_text(call, "Name")), p);
    s = cons(s_kid(seq, p), s);

    p = s_text(ident, "c");
    p = s_both(expr, "s_retype(guard, c)", p);
    p = cons(s_both(bind, "c", s_text(call, "Code")), p);
    p = cons(s_text(call, "And"), p);
    s = cons(s_kid(seq, p), s);

    p = s_text(ident, "u");
    p = s_both(expr, "s_kid(not, u)", p);
    p = cons(s_both(bind, "u", s_text(call, "Rule4")), p);
    p = cons(s_text(call, "Not"), p);
    s = cons(s_kid(seq, p), s);

    p = s_text(ident, "u");
    p = s_both(expr, "s_kid(and, u)", p);
    p = cons(s_both(bind, "u", s_text(call, "Rule4")), p);
    p = cons(s_text(call, "And"), p);
    s = cons(s_kid(seq, p), s);

    p = s_kid(alt, s);
    p = s_both(rule, "Rule3", cons(s_text(type, "struct s_node *"), p));
    r = cons(p, r);

    /*
	Rule2
	    ← r:Rule3 s:Rule2 { cons(r, s) }
	    / ε { 0 }
     */

    s = s_kid(seq, s_text(expr, "0"));

    p = cons(s_text(ident, "s"), s_text(ident, "r"));
    p = s_both(expr, "cons(r, s)", p);
    p = cons(s_both(bind, "s", s_text(call, "Rule2")), p);
    p = cons(s_both(bind, "r", s_text(call, "Rule3")), p);
    p = s_kid(seq, p);

    p = s_kid(alt, cons(p, s));
    p = s_both(rule, "Rule2", cons(s_text(type, "struct s_node *"), p));
    r = cons(p, r);

    /*
	Rule1 
	    ← Epsilon r:Result? → { s_kid(seq, r) }
	    / s:Rule2 r:Result? → { s_kid(seq, append(s, r)) }
     */

    p = cons(s_text(ident, "s"), s_text(ident, "r"));
    p = s_both(expr, "s_kid(seq, append(s, r))", p);
    //q = s_kid(seq, s_text(call, "Result"));
    q = s_text(call, "Result");
    q = s_both(bind, "r", s_both(rep, ",1", q));
    p = cons(q, p);
    p = cons(s_both(bind, "s", s_text(call, "Rule2")), p);
    s = s_kid(seq, p);

    p = s_text(ident, "r");
    p = s_both(expr, "s_kid(seq, r)", p);
    //q = s_kid(seq, s_text(call, "Result"));
    q = s_text(call, "Result");
    q = s_both(bind, "r", s_both(rep, ",1", q));
    p = cons(q, p);
    p = cons(s_text(call, "Epsilon"), p);
    q = cons(s_kid(seq, p), s);

    p = s_new(alt); p->first = q; q = p;
    p = s_new(type); p->text = "struct s_node *"; p->next = q; q = p;
    p = s_new(rule); p->text = "Rule1"; p->first = q; p->next = r; r = p;
  
    /*
	Rule0
	    ← s:Rule1 Slash r:Rule0 { s_alt(s, r) }
	    / s:Rule1 → s
    */

    p = s_new(ident); p->text = "s"; i = p;
    p = s_new(expr); p->text="s"; p->first = i; q = p;
    p = s_new(call); p->text = "Rule1"; s = p;
    p = s_new(bind); p->text = "s"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; t = p;

    p = s_new(ident); p->text = "r"; i = p;
    p = s_new(ident); p->text = "s"; p->next = i; i = p;
    p = s_new(expr); p->text="s_alt(s, r)"; p->first = i; q = p;
    p = s_new(call); p->text = "Rule0"; s = p;
    p = s_new(bind); p->text = "r"; p->first = s; p->next = q; q = p;
    p = s_new(call); p->text = "Slash"; p->next = q; q = p;
    p = s_new(call); p->text = "Rule1"; s = p;
    p = s_new(bind); p->text = "s"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = t; q = p;

    p = s_new(alt); p->first = q; q = p;
    p = s_new(type); p->text = "struct s_node *"; p->next = q; q = p;
    p = s_new(rule); p->text = "Rule0"; p->first = q; p->next = r; r = p;

    /*
	Defn
	    ← n:Name t:TypeOptional lArrow r:Rule _ →
		{ s_both(rule, n, cons(s_text(type, t), r)) }
    */
    p = s_new(ident); p->text = "r"; i = p;
    p = s_new(ident); p->text = "t"; p->next = i; i = p;
    p = s_new(ident); p->text = "n"; p->next = i; i = p;
    p = s_text(expr, "s_both(rule, n, cons(s_text(type, t), r))"); p->first = i; q = p;
    p = s_new(call); p->text = "_"; p->next = q; q = p;
    p = s_new(call); p->text = "Rule0"; s = p;
    p = s_new(bind); p->text = "r"; p->first = s; p->next = q; q = p;
    p = s_new(call); p->text = "lArrow"; p->next = q; q = p;
    p = s_new(call); p->text = "TypeOptional"; s = p;
    p = s_new(bind); p->text = "t"; p->first = s; p->next = q; q = p;
    p = s_new(call); p->text = "Name"; s = p;
    p = s_new(bind); p->text = "n"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "struct s_node *"; p->next = q; q = p;
    p = s_new(rule); p->text = "Defn"; p->first = q; p->next = r; r = p;

    /* Defns ← d:Defn ds:Defns { cons(d, ds) } / d:Defn → d */

    p = s_both(expr, "d", s_text(ident, "d"));
    p = cons(s_both(bind, "d", s_text(call, "Defn")), p);
    t = s_kid(seq, p);

    p = s_new(ident); p->text = "ds"; i = p;
    p = s_new(ident); p->text = "d"; p->next = i; i = p;
    p = s_new(expr); p->text = "cons(d, ds)"; p->first = i; q = p;
    p = s_new(call); p->text = "Defns"; s = p;
    p = s_new(bind); p->text = "ds"; p->first = s; p->next = q; q = p;
    p = s_new(call); p->text = "Defn"; s = p;
    p = s_new(bind); p->text = "d"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = t; t = p;

    p = s_new(alt); p->first = t; q = p;
    p = s_new(type); p->text = "struct s_node *"; p->next = q; q = p;
    p = s_new(rule); p->text = "Defns"; p->first = q; p->next = r; r = p;

    /*
	Grammar :: struct s_node *
	    ← _ p:Preamble ds:Defns
	    → { s_both(grammar, "yy", cons(p, ds)) }
    */
    p = s_new(ident); p->text = "ds"; i = p;
    p = s_new(ident); p->text = "p"; p->next = i; i = p;
    p = s_both(expr, "s_both(grammar, \"\", cons(p, ds))", i); q = p;
    p = s_new(call); p->text = "Defns"; s = p;
    p = s_new(bind); p->text = "ds"; p->first = s; p->next = q; q = p;
    p = s_new(call); p->text = "Preamble"; s = p;
    p = s_new(bind); p->text = "p"; p->first = s; p->next = q; q = p;
    p = s_new(call); p->text = "_"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "struct s_node *"; p->next = q; q = p;
    p = s_new(rule); p->text = "Grammar"; p->first = q; p->next = r; r = p;

    r = cons(s_text(preamble, "#include <ctype.h>\n#include \"syntax.h\"\n"), r);
    p = s_both(grammar, "", r);

    *result = p;
    return 1;
}
