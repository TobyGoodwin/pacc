#include "pacc.h"
#include "syntax.h"

char *prefix = 0; /* XXX */

int parse(char *ignore0, off_t ignore1, struct s_node **result) {
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
    p = s_new(call); p->text = "Comment"; q = p;
    p = s_new(lit); p->text = "\\n"; p->next = q; q = p;
    p = s_new(lit); p->text = "\\t"; p->next = q; q = p;
    p = s_new(lit); p->text = " "; p->next = q; q = p;
    p = s_new(alt); p->first = q; q = p;
    p = s_new(rep); p->first = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "int" /* XXX: "void" */; p->next = q; q = p;
    p = s_new(rule); p->text = "_"; p->first = q; p->next = r; r = p;

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
	    ← c:Char &{ isalnum(*c) || *c == '_' }
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

    /*
	QuotedChar :: void
	    ← "\\n" / "\\t" / "\\\\" / "\\\"" / !"\\" Char
    */

    p = s_new(call); p->text = "Char"; q = p;
    p = s_new(lit); p->text = "\\\\"; s = p;
    p = s_new(not); p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; t = p;

    p = s_new(lit); p->text = "\\\\\\\""; q = p;
    p = s_new(seq); p->first = q; p->next = t; t = p;

    p = s_new(lit); p->text = "\\\\\\\\"; q = p;
    p = s_new(seq); p->first = q; p->next = t; t = p;

    p = s_text(lit, "\\\\t");
    t = cons(s_kid(seq, p), t);

    p = s_new(lit); p->text = "\\\\n"; q = p;
    p = s_new(seq); p->first = q; p->next = t; t = p;

    p = s_new(alt); p->first = t; q = p;
    p = s_new(type); p->text = "int"; p->next = q; q = p;
    p = s_new(rule); p->text = "QuotedChar"; p->first = q; p->next = r; r = p;

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
	QuotedChars ← (!"\"" QuotedChar)* { match() }
    */

    p = s_new(expr); p->text = "match()"; t = p;
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

    /* Char ← . { match() } */

    p = s_new(expr); p->text = "match()"; q = p;
    p = s_new(any); p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "char *"; p->next = q; q = p;
    p = s_new(rule); p->text = "Char"; p->first = q; p->next = r; r = p;

    /*
	TypeOptional
	    ← ColCol TypeElement+ { s_stash_type(match()) }
	    / ε { s_stashed_type() }
    */
    p = s_new(expr); p->text = "s_stashed_type()"; q = p;
    p = s_new(seq); p->first = q; t = p;

    p = s_new(expr); p->text = "s_stash_type(match())"; q = p;
    p = s_new(call); p->text = "TypeElement"; s = p;
    p = s_text(rep, "1,"); p->first = s; p->next = q; q = p;
    p = s_new(call); p->text = "ColCol"; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = t; t = p;

    p = s_new(alt); p->first = t; q = p;
    p = s_new(type); p->text = "char *"; p->next = q; q = p;
    p = s_new(rule); p->text = "TypeOptional"; p->first = q; p->next = r; r = p;

#if 0
    /*
	RawCode
	    ← "{" c:Char &{ *c != 0x7d } "}" { s_text(bind, match()) } _
    */

    p = s_new(call); p->text = "_"; q = p;
    p = s_new(expr); p->text = "s_text(bind, match())"; p->next = q; q = p;
    p = s_new(lit); p->text = "}"; p->next = q; t = p;
    p = s_new(ident); p->text = "c"; s = p;
    p = s_new(guard); p->text = "*c != 0x7d"; p->first = s; q = p;
    p = s_new(call); p->text = "Char"; s = p;
    p = s_new(bind); p->text = "c"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(rep); p->first = q; p->next = t; q = p;
    p = s_new(lit); p->text = "{"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "struct s_node *"; p->next = q; q = p;
    p = s_new(rule); p->text = "RawCode"; p->first = q; p->next = r; r = p;
#endif

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
	Code
	    ← "{" n:CodeAndNames "}" _ → n
     */

    /* XXX probably want lBrace and rBrace */

    p = s_both(expr, "n", s_text(ident, "n"));
    p = cons(s_text(call, "_"), p);
    p = cons(s_text(lit, "}"), p);
    p = cons(s_both(bind, "n", s_text(call, "CodeAndNames")), p);
    p = cons(s_text(lit, "{"), p);
    p = s_kid(seq, p);
    p = cons(s_text(type, "struct s_node *"), p);
    p = s_both(rule, "Code", p);
    r = cons(p, r);

    /*
	CodeAndNames
	    ← n:CodeNames { s_both(expr, match(), n) }
     */

    p = s_both(expr, "s_both(expr, match(), n)", s_text(ident, "n"));
    p = cons(s_both(bind, "n", s_text(call, "CodeNames")), p);
    p = s_kid(seq, p);
    p = cons(s_text(type, "struct s_node *"), p);
    r = cons(s_both(rule, "CodeAndNames", p), r);

    /*
	CodeNames
	    ← n:Name ns:CodeNames { s_set_cons(s_text(ident, n), ns) }
	    / StringLit ns:CodeNames → ns
	    / CharLit ns:CodeNames → ns
	    / c:Char &{ *c != '}' } ns:CodeNames → ns
	    / ε { 0 }
     */

    p = s_text(expr, "0");
    p = s_kid(seq, p);
    t = p;

    p = s_both(expr, "ns", s_text(ident, "ns"));
    p = cons(s_both(bind, "ns", s_text(call, "CodeNames")), p);
    p = cons(s_both(guard, "*c != '}'", s_text(ident, "c")), p);
    p = cons(s_both(bind, "c", s_text(call, "Char")), p);
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
    p = cons(s_both(rep, ",1", s_text(call, "rArrow")), p);
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
	Matcher
	    ← Epsilon { s_new(seq) }
	    / r:Result → r
	    / And u:UnaryRule { s_kid(and, u) }
	    / Not u:UnaryRule { s_kid(not, u) }
	    / And c:Code { s_retype(guard, c) }
	    / n:Name Colon u:UnaryRule { s_bind(n, u) }
	    / u:UnaryRule → u
    */

    p = s_new(ident); p->text = "u"; i = p;
    p = s_new(expr); p->text="u"; p->first =i; q = p;
    p = s_new(call); p->text = "UnaryRule"; s = p;
    p = s_new(bind); p->text = "u"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; t = p;

    p = s_new(ident); p->text = "u"; i = p;
    p = s_new(ident); p->text = "n"; p->next = i; i = p;
    p = s_new(expr); p->text="s_bind(n, u)"; p->first = i; q = p;
    p = s_new(call); p->text = "UnaryRule"; s = p;
    p = s_new(bind); p->text = "u"; p->first = s; p->next = q; q = p;
    p = s_new(call); p->text = "Colon"; p->next = q; q = p;
    p = s_new(call); p->text = "Name"; s = p;
    p = s_new(bind); p->text = "n"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = t; t = p;

    p = s_new(ident); p->text = "c"; i = p;
    p = s_new(expr); p->text="s_retype(guard, c)"; p->first = i; q = p;
    p = s_new(call); p->text = "Code"; s = p;
    p = s_new(bind); p->text = "c"; p->first = s; p->next = q; q = p;
    p = s_new(call); p->text = "And"; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = t; t = p;

    p = s_new(ident); p->text = "u"; i = p;
    p = s_new(expr); p->text="s_kid(not, u)"; p->first = i; q = p;
    p = s_new(call); p->text = "UnaryRule"; s = p;
    p = s_new(bind); p->text = "u"; p->first = s; p->next = q; q = p;
    p = s_new(call); p->text = "Not"; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = t; t = p;

    p = s_new(ident); p->text = "u"; i = p;
    p = s_new(expr); p->text="s_kid(and, u)"; p->first = i; q = p;
    p = s_new(call); p->text = "UnaryRule"; s = p;
    p = s_new(bind); p->text = "u"; p->first = s; p->next = q; q = p;
    p = s_new(call); p->text = "And"; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = t; t = p;

    p = s_both(expr, "r", s_text(ident, "r"));
    p = cons(s_both(bind, "r", s_text(call, "Result")), p);
    t = cons(s_kid(seq, p), t);

    p = s_text(expr, "s_new(seq)");
    p = cons(s_text(call, "Epsilon"), p);
    t = cons(s_kid(seq, p), t);

    p = s_new(alt); p->first = t; q = p;
    p = s_new(type); p->text = "struct s_node *"; p->next = q; q = p;
    p = s_new(rule); p->text = "Matcher"; p->first = q; p->next = r; r = p;

    /*
	Matchers
	    ← m:Matcher ms:Matchers { cons(m, ms) }
	    / m:Matcher → m
    */
    p = s_new(ident); p->text = "m"; i = p;
    p = s_new(expr); p->text = "m"; p->first = i; q = p;
    p = s_new(call); p->text = "Matcher"; s = p;
    p = s_new(bind); p->text = "m"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; t = p;

    p = s_new(ident); p->text = "ms"; i = p;
    p = s_new(ident); p->text = "m"; p->next = i; i = p;
    p = s_new(expr); p->text = "cons(m, ms)"; p->first = i; q = p;
    p = s_new(call); p->text = "Matchers"; s = p;
    p = s_new(bind); p->text = "ms"; p->first = s; p->next = q; q = p;
    p = s_new(call); p->text = "Matcher"; s = p;
    p = s_new(bind); p->text = "m"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = t; t = p;

    p = s_new(alt); p->first = t; q = p;
    p = s_new(type); p->text = "struct s_node *"; p->next = q; q = p;
    p = s_new(rule); p->text = "Matchers"; p->first = q; p->next = r; r = p;

    /*
	Sequence
	    ← m:Matchers { s_kid(seq, m) }
    */

    p = s_text(ident, "m"); i = p;
    p = s_text(expr, "s_kid(seq, m)"); p->first = i; q = p;
    p = s_new(call); p->text = "Matchers"; s = p;
    p = s_new(bind); p->text = "m"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; t = p;

    p = s_new(type); p->text = "struct s_node *"; p->next = q; q = p;
    p = s_new(rule); p->text = "Sequence"; p->first = q; p->next = r; r = p;

    /*
	PrimRule
	    ← n:Name { s_text(call, n) } !lArrow !ColCol
	    / Dot { s_new(any) }
	    / l:StringLit { s_text(lit, l) }
	    / lParen r:Rule rParen → r
    */
    p = s_new(ident); p->text = "r"; i = p;
    p = s_new(expr); p->text="r"; p->first = i; q = p;
    p = s_new(call); p->text = "rParen"; p->next = q; q = p;
    p = s_new(call); p->text = "Rule"; s = p;
    p = s_new(bind); p->text = "r"; p->first = s; p->next = q; q = p;
    p = s_new(call); p->text = "lParen"; p->next = q; q = p;
    p = s_new(seq); p->first = q; t = p;

    p = s_new(ident); p->text = "l"; i = p;
    p = s_new(expr); p->text="s_text(lit, l)"; p->first = i; q = p;
    p = s_new(call); p->text = "StringLit"; s = p;
    p = s_new(bind); p->text = "l"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = t; t = p;

    p = s_text(expr, "s_new(any)");
    p = cons(s_text(call, "Dot"), p);
    t = cons(s_kid(seq, p), t);

    p = s_kid(not, s_text(call, "ColCol"));
    p = cons(s_kid(not, s_text(call, "lArrow")), p); q = p;
    p = s_new(ident); p->text = "n"; i = p;
    p = s_new(expr); p->text="s_text(call, n)"; p->first = i; p->next = q; q = p;
    p = s_new(call); p->text = "Name"; s = p;
    p = s_new(bind); p->text = "n"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = t; t = p;

    p = s_new(alt); p->first = t; q = p;
    p = s_new(type); p->text = "struct s_node *"; p->next = q; q = p;
    p = s_new(rule); p->text = "PrimRule"; p->first = q; p->next = r; r = p;

    /*
	UnaryRule
	    ← p:PrimRule Query { s_both(rep, ",1", p) }
	    / p:PrimRule Star { s_both(rep, 0, p) }
	    / p:PrimRule Plus { s_both(rep, "1,", p) }
	    / p:PrimRule → p
    */
    p = s_new(ident); p->text = "p"; i = p;
    p = s_new(expr); p->text="p"; p->first =i; q = p;
    p = s_new(call); p->text = "PrimRule"; s = p;
    p = s_new(bind); p->text = "p"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; t = p;

    p = s_new(ident); p->text = "p"; i = p;
    p = s_new(expr); p->text="s_both(rep, \"1,\", p)"; p->first = i; q = p;
    p = s_new(call); p->text = "Plus"; p->next = q; q = p;
    p = s_new(call); p->text = "PrimRule"; s = p;
    p = s_new(bind); p->text = "p"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = t; t = p;

    p = s_new(ident); p->text = "p"; i = p;
    p = s_new(expr); p->text="s_both(rep, 0, p)"; p->first = i; q = p;
    p = s_new(call); p->text = "Star"; p->next = q; q = p;
    p = s_new(call); p->text = "PrimRule"; s = p;
    p = s_new(bind); p->text = "p"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = t; t = p;

    p = s_new(ident); p->text = "p"; i = p;
    p = s_new(expr); p->text="s_both(rep, \",1\", p)"; p->first = i; q = p;
    p = s_new(call); p->text = "Query"; p->next = q; q = p;
    p = s_new(call); p->text = "PrimRule"; s = p;
    p = s_new(bind); p->text = "p"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = t; t = p;

    p = s_new(alt); p->first = t; q = p;
    p = s_new(type); p->text = "struct s_node *"; p->next = q; q = p;
    p = s_new(rule); p->text = "UnaryRule"; p->first = q; p->next = r; r = p;

    /*
       SeqRule
	   ← s:Sequence → s
	   / u:UnaryRule → u
    */
    p = s_new(ident); p->text = "u"; i = p;
    p = s_new(expr); p->text="u"; p->first = i; q = p;
    p = s_new(call); p->text = "UnaryRule"; s = p;
    p = s_new(bind); p->text = "u"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; t = p;

    p = s_new(ident); p->text = "s"; i = p;
    p = s_new(expr); p->text="s"; p->first = i; q = p;
    p = s_new(call); p->text = "Sequence"; s = p;
    p = s_new(bind); p->text = "s"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = t; q = p;

    p = s_new(alt); p->first = q; q = p;
    p = s_new(type); p->text = "struct s_node *"; p->next = q; q = p;
    p = s_new(rule); p->text = "SeqRule"; p->first = q; p->next = r; r = p;

    /*
	Rule
	    ← s:SeqRule Slash r:Rule { s_kid(alt, cons(s, r)) }
	    / s:SeqRule → s
    */
    p = s_new(ident); p->text = "s"; i = p;
    p = s_new(expr); p->text="s"; p->first = i; q = p;
    p = s_new(call); p->text = "SeqRule"; s = p;
    p = s_new(bind); p->text = "s"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; t = p;

    p = s_new(ident); p->text = "r"; i = p;
    p = s_new(ident); p->text = "s"; p->next = i; i = p;
    p = s_new(expr); p->text="s_kid(alt, cons(s, r))"; p->first = i; q = p;
    p = s_new(call); p->text = "Rule"; s = p;
    p = s_new(bind); p->text = "r"; p->first = s; p->next = q; q = p;
    p = s_new(call); p->text = "Slash"; p->next = q; q = p;
    p = s_new(call); p->text = "SeqRule"; s = p;
    p = s_new(bind); p->text = "s"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = t; q = p;

    p = s_new(alt); p->first = q; q = p;
    p = s_new(type); p->text = "struct s_node *"; p->next = q; q = p;
    p = s_new(rule); p->text = "Rule"; p->first = q; p->next = r; r = p;

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
    p = s_new(call); p->text = "Rule"; s = p;
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
    p = s_both(expr, "s_both(grammar, \"yy\", cons(p, ds))", i); q = p;
    p = s_new(call); p->text = "End"; p->next = q; q = p;
    p = s_new(call); p->text = "Defns"; s = p;
    p = s_new(bind); p->text = "ds"; p->first = s; p->next = q; q = p;
    p = s_new(call); p->text = "Preamble"; s = p;
    p = s_new(bind); p->text = "p"; p->first = s; p->next = q; q = p;
    p = s_new(call); p->text = "_"; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "struct s_node *"; p->next = q; q = p;
    p = s_new(rule); p->text = "Grammar"; p->first = q; p->next = r; r = p;

#if 0
    /* start anywhere... */
    p = s_new(ident); p->text = "r"; i = p;
    p = s_new(expr); p->text = "r"; p->first = i; q = p;
    p = s_new(call); p->text = "End"; p->next = q; q = p;
    p = s_new(call); p->text = "Sequence"; s = p;
    p = s_new(bind); p->text = "r"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "struct s_node *"; p->next = q; q = p;
    p = s_new(rule); p->text = "Start"; p->first = q; p->next = r;
    r = p;
#endif

    r = cons(s_text(preamble, "#include <ctype.h>\n#include \"syntax.h\"\n"), r);
    p = s_both(grammar, "yy", r);

    *result = p;
    return 1;
}
