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
	End ← !.
    */

    p = new_node(any); q = p;
    p = new_node(not); p->first = q; q = p;
    p = s_new(type); p->text = "int" /* XXX: "void" */; p->next = q; q = p;
    p = s_new(rule); p->text = "End"; p->first = q; r = p;

    /*
	Star  ← "*" _
    */

    p = s_new(call); p->text = "_"; q = p;
    p = s_new(lit); p->text = "*"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "int" /* XXX: "void" */; p->next = q; q = p;
    p = s_new(rule); p->text = "Star"; p->first = q; p->next = r; r = p;

    /*
	Slash ← "/" _
    */

    p = s_new(call); p->text = "_"; q = p;
    p = s_new(lit); p->text = "/"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "int" /* XXX: "void" */; p->next = q; q = p;
    p = s_new(rule); p->text = "Slash"; p->first = q; p->next = r; r = p;

    /*
	Query ← "?" _
    */

    p = s_new(call); p->text = "_"; q = p;
    p = s_new(lit); p->text = "?"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "int" /* XXX: "void" */; p->next = q; q = p;
    p = s_new(rule); p->text = "Query"; p->first = q; p->next = r; r = p;

    /*
	Plus  ← "+" _
    */

    p = s_new(call); p->text = "_"; q = p;
    p = s_new(lit); p->text = "+"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "int" /* XXX: "void" */; p->next = q; q = p;
    p = s_new(rule); p->text = "Plus"; p->first = q; p->next = r; r = p;

    /*
	rParen  ← ")" _
    */

    p = s_new(call); p->text = "_"; q = p;
    p = s_new(lit); p->text = ")"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "int" /* XXX: "void" */; p->next = q; q = p;
    p = s_new(rule); p->text = "rParen"; p->first = q; p->next = r; r = p;

    /*
	lParen  ← "(" _
    */

    p = s_new(call); p->text = "_"; q = p;
    p = s_new(lit); p->text = "("; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "int" /* XXX: "void" */; p->next = q; q = p;
    p = s_new(rule); p->text = "lParen"; p->first = q; p->next = r; r = p;

    /*
	Not  ← "!" _
    */

    p = s_new(call); p->text = "_"; q = p;
    p = s_new(lit); p->text = "!"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "int" /* XXX: "void" */; p->next = q; q = p;
    p = s_new(rule); p->text = "Not"; p->first = q; p->next = r; r = p;

    /*
	Epsilon ← "ε" _
     */

    p = s_text(call, "_");
    p = cons(s_text(lit, "ε"), p);
    p = s_kid(seq, p);
    p = cons(s_text(type, "int" /* XXX: void */), p);
    r = cons(s_both(rule, "Epsilon", p), r);

    /*
	Dot ← "." _
     */

    p = s_text(call, "_");
    p = cons(s_text(lit, "."), p);
    p = s_kid(seq, p);
    p = cons(s_text(type, "int" /* XXX: void */), p);
    r = cons(s_both(rule, "Dot", p), r);

    /*
	ColCol ← "::" _
    */

    p = s_new(call); p->text = "_"; q = p;
    p = s_new(lit); p->text = "::"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "int" /* XXX: "void" */; p->next = q; q = p;
    p = s_new(rule); p->text = "ColCol"; p->first = q; p->next = r; r = p;

    /*
	Colon ← ":" _
    */

    p = s_new(call); p->text = "_"; q = p;
    p = s_new(lit); p->text = ":"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "int" /* XXX: "void" */; p->next = q; q = p;
    p = s_new(rule); p->text = "Colon"; p->first = q; p->next = r; r = p;

    /*
	lArrow  ← ("←" / "<-") _
    */

    p = s_new(call); p->text = "_"; q = p;
    p = s_new(lit); p->text = "<-"; t = p;
    p = s_new(lit); p->text = "←"; p->next = t; t = p;
    p = s_new(alt); p->first = t; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "int" /* XXX: "void" */; p->next = q; q = p;
    p = s_new(rule); p->text = "lArrow"; p->first = q; p->next = r; r = p;

    /*
	rArrow  ← ("→" / "->") _
    */

    p = s_new(call); p->text = "_"; q = p;
    p = s_new(lit); p->text = "->"; t = p;
    p = s_new(lit); p->text = "→"; p->next = t; t = p;
    p = s_new(alt); p->first = t; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "int" /* XXX: "void" */; p->next = q; q = p;
    p = s_new(rule); p->text = "rArrow"; p->first = q; p->next = r; r = p;

    /*
	And  ← "&" _
    */

    p = s_new(call); p->text = "_"; q = p;
    p = s_new(lit); p->text = "&"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "int" /* XXX: "void" */; p->next = q; q = p;
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
    p = cons(s_text(type, "int" /* XXX: void */), p);
    p = s_both(rule, "_", p);
    r = cons(p, r);

#if 0
    p = s_new(call); p->text = "Comment"; q = p;
    p = s_text(lit,p->text = "\\n"; p->next = q; q = p;
    p = s_new(lit); p->text = "\\t"; p->next = q; q = p;
    p = s_new(lit); p->text = " "; p->next = q; q = p;
    p = s_new(alt); p->first = q; q = p;
    p = s_new(rep); p->first = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "int" /* XXX: "void" */; p->next = q; q = p;
    p = s_new(rule); p->text = "_"; p->first = q; p->next = r; r = p;
#endif

#if 0
    /* Comment ← "#" (c:Char &{ *c != '\n' })* "\n" */
    p = s_new(lit); p->text = "\\n"; s = p;
    p = s_new(ident); p->text = "c"; i = p;
    p = s_new(guard); p->text = "*c != '\\n'" /* XXX: '\n' */; p->first = i; q = p;
    p = s_new(call); p->text = "Char"; i = p;
    p = s_new(bind); p->text = "c"; p->first = i; p->next = q; q = p;
    p = s_kid(seq, p);
    p = s_both(rep, "", p); p->next = s; q = p;
    p = s_new(lit); p->text = "#"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "int" /* XXX: "void" */; p->next = q; q = p;
    p = s_new(rule); p->text = "Comment"; p->first = q; p->next = r; r = p;
#endif

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
    p = cons(s_text(type, "int" /* XXX void */), p);
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
    p = cons(s_text(type, "int" /* XXX void */), p);
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
    p = s_new(type); p->text = "int" /* XXX: "void" */; p->next = q; q = p;
    p = s_new(rule); p->text = "TypeElement"; p->first = q; p->next = r; r = p;

    /*
	NameCont
	    ← c:Char &{ isalnum(ref_0(c)) || ref_0(c) == '_' }
    */

    p = s_new(ident); p->text = "c"; s = p;
    p = s_text(guard, "isalnum(ref_0(c)) || ref_0(c) == '_'"); p->first = s; q = p;
    p = s_new(call); p->text = "Char"; s = p;
    p = s_new(bind); p->text = "c"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "int" /* XXX: "void" */; p->next = q; q = p;
    p = s_new(rule); p->text = "NameCont"; p->first = q; p->next = r; r = p;

    /*
	NameStart :: void
	    ← c:Char &{ isalpha(ref_0(c)) || ref_0(c) == '_' }
    */

    p = s_new(ident); p->text = "c"; s = p;
    p = s_text(guard, "isalpha(ref_0(c)) || ref_0(c) == '_'"); p->first = s; q = p;
    p = s_new(call); p->text = "Char"; s = p;
    p = s_new(bind); p->text = "c"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "int" /* XXX: "void" */; p->next = q; q = p;
    p = s_new(rule); p->text = "NameStart"; p->first = q; p->next = r; r = p;

    /*
	QuotedChar
	    ← SimpleCharEscape
	    / !"\\" Char
    */

    p = s_kid(not, s_text(lit, "\\\\"));
    q = s_text(call, "Char");
    p = s_kid(seq, cons(p, q));
    p = cons(s_text(call, "SimpleCharEscape"), p);
    p = s_kid(alt, p);
    p = s_kid(seq, p);
    p = cons(s_text(type, "char *"), p);
    p = s_both(rule, "QuotedChar", p);
    r = cons(p, r);

    /*
	SimpleCharEscape
	    ← "\\\'" / "\\\"" / "\\\?" / "\\\\"
	    / "\\a" / "\\b" / "\\f" / "\\n" / "\\r" / "\\t" / "\\v"
    */

    p = s_text(lit, "\\\\v");
    p = cons(s_text(lit, "\\\\t"), p);
    p = cons(s_text(lit, "\\\\r"), p);
    p = cons(s_text(lit, "\\\\n"), p);
    p = cons(s_text(lit, "\\\\f"), p);
    p = cons(s_text(lit, "\\\\b"), p);
    p = cons(s_text(lit, "\\\\a"), p);
    p = cons(s_text(lit, "\\\\\\\\"), p);
    p = cons(s_text(lit, "\\\\\\\?"), p);
    p = cons(s_text(lit, "\\\\\\\""), p);
    p = cons(s_text(lit, "\\\\\\\'"), p);
    p = s_kid(alt, p);
    p = s_kid(seq, p);
    p = cons(s_text(type, "char *"), p);
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
    p = cons(s_text(type, "char *"), p);
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
	    ← n:(NameStart NameCont*) _ { ref_dup(n) }
    */
    p = s_both(expr, "ref_dup(n)", s_text(ident, "n"));
    p = cons(s_text(call, "_"), p);
    q = s_both(rep, 0, s_kid(seq, s_text(call, "NameCont")));
    q = cons(s_text(call, "NameStart"), q);
    q = s_kid(seq, q);
    q = s_both(bind, "n", q);
    p = s_kid(seq, cons(q, p));
    p = cons(s_text(type, "char *"), p);
    p = s_both(rule, "Name", p);
    r = cons(p, r);

#if 0
    /*
	Name :: char *
	    ← NameStart NameCont* { match() } _
    */

    p = s_new(call); p->text = "_"; q = p;
    p = s_new(expr); p->text = "match()"; p->next = q; q = p;
    p = s_new(call); p->text = "NameCont"; s = p;
    p = s_new(rep); p->first = s; p->next = q; q = p;
    p = s_new(call); p->text = "NameStart"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "char *"; p->next = q; q = p;
    p = s_new(rule); p->text = "Name"; p->first = q; p->next = r; r = p;
#endif

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
	    / r:Rule6 → r
     */

    p = s_both(expr, "r", s_text(ident, "r"));
    p = cons(s_both(bind, "r", s_text(call, "Rule6")), p);
    s = s_kid(seq, p);

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

    /* Defns ← d:Defn ds:Defns { cons(d, ds) } / ε { 0 } */

    p = s_new(expr); p->text = "0"; q = p;
    p = s_new(seq); p->first = q; t = p;

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
	    ← _ p:Preamble ds:Defns End →
	    { s_both(grammar, "yy", cons(p, ds)) }
    */
    p = s_new(ident); p->text = "ds"; i = p;
    p = s_new(ident); p->text = "p"; p->next = i; i = p;
    p = s_both(expr, "s_both(grammar, \"\", cons(p, ds))", i); q = p;
    p = s_new(call); p->text = "End"; p->next = q; q = p;
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
