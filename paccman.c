#include "syntax.h"

/* Hmm. What are we going to do about the raw code prefix? It could be a
 * node in the tree, but that hardly seems worthwhile.
 */
char *prefix = "#include <ctype.h>\n#include \"syntax.h\"\n";

struct s_node *create(void) {
    struct s_node *i, *p, *q, *r, *s, *t;

    /*
    */

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

    /* _ ← (" " / "\n" / Comment) * */
    p = s_new(call); p->text = "Comment"; q = p;
    p = s_new(lit); p->text = "\n"; p->next = q; q = p;
    p = s_new(lit); p->text = " "; p->next = q; q = p;
    p = s_new(alt); p->first = q; q = p;
    p = s_new(rep); p->first = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "int" /* XXX: "void" */; p->next = q; q = p;
    p = s_new(rule); p->text = "_"; p->first = q; p->next = r; r = p;

    /* Comment ← "#" (c:Char &{ *c /= '\n' })* "\n" */
    p = s_new(lit); p->text = "\n"; q = p;
    p = s_new(ident); p->text = "c"; s = p;
    p = s_new(guard); p->text = "*c /= '\\n'" /* XXX: '\n' */; p->first = s; p->next = q; q = p;
    p = s_new(call); p->text = "Char"; s = p;
    p = s_new(bind); p->text = "c"; p->first = s; p->next = q; q = p;
    p = s_new(rep); p->first = q; q = p;
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
    p = s_new(seq); p->first = q; t = p;

    p = s_new(alt); p->first = t; q = p;
    p = s_new(type); p->text = "int" /* XXX: "void" */; p->next = q; q = p;
    p = s_new(rule); p->text = "TypeElement"; p->first = q; p->next = r; r = p;

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

    /*
	QuotedChar :: void
	    ← "\\n" / "\\\\" / "\\\"" / !"\\" Char
    */

    p = s_new(call); p->text = "Char"; q = p;
    p = s_new(lit); p->text = "\\"; s = p;
    p = s_new(not); p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; t = p;

    p = s_new(lit); p->text = "\\\""; q = p;
    p = s_new(seq); p->first = q; p->next = t; t = p;

    p = s_new(lit); p->text = "\\\\"; q = p;
    p = s_new(seq); p->first = q; p->next = t; t = p;

    p = s_new(lit); p->text = "\\n"; q = p;
    p = s_new(seq); p->first = q; p->next = t; t = p;

    p = s_new(alt); p->first = t; q = p;
    p = s_new(type); p->text = "int"; p->next = q; q = p;
    p = s_new(rule); p->text = "QuotedChar"; p->first = q; p->next = r; r = p;

    /*
	QuotedChars ← (!"\"" QuotedChar)* { match() }
    */

    p = s_new(expr); p->text = "match()"; t = p;
    p = s_new(call); p->text = "QuotedChar"; q = p;
    p = s_new(lit); p->text = "\""; s = p;
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
    p = s_new(lit); p->text = "\""; p->next = q; q = p;
    p = s_new(call); p->text = "QuotedChars"; s = p;
    p = s_new(bind); p->text = "q"; p->first = s; p->next = q; q = p;
    p = s_new(lit); p->text = "\""; p->next = q; q = p;
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
    p = s_new(call); p->text = "ColCol"; p->next = q; s = p;
    p = s_new(seq); p->first = q; p->next = t; t = p;

    p = s_new(alt); p->first = t; q = p;
    p = s_new(type); p->text = "char *"; p->next = q; q = p;
    p = s_new(rule); p->text = "TypeOptional"; p->first = q; p->next = r; r = p;

    /*
	RawCode
	    ← "{" c:Char &{ *c != 0x7d } "}" { match() } _
    */

    p = s_new(call); p->text = "_"; q = p;
    p = s_new(expr); p->text = "match()"; p->next = q; q = p;
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
	Preamble
	    ← r:RawCode _ → r
	    / ε { 0 }
    */

    p = s_new(expr); p->text = "0"; q = p;
    p = s_new(seq); p->first = q; t = p;

    p = s_new(ident); p->text = "r"; i = p;
    p = s_new(expr); p->text = "r"; p->first = i; q = p;
    p = s_new(call); p->text = "_"; p->next = q; q = p;
    p = s_new(call); p->text = "RawCode"; s = p;
    p = s_new(bind); p->text = "r"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = t; t = p;

    p = s_new(alt); p->first = t; q = p;
    p = s_new(type); p->text = "char *"; p->next = q; q = p;
    p = s_new(rule); p->text = "Preamble"; p->first = q; p->next = r; r = p;

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
	Result
	    ← rArrow n:Name → s_expr(n)
	    / rArrow? r:RawCode → s_expr(r)
    */

    p = s_new(ident); p->text = "r"; i = p;
    p = s_new(expr); p->text = "s_expr(r)"; p->first = i; q = p;
    p = s_new(call); p->text = "RawCode"; s = p;
    p = s_new(bind); p->text = "r"; p->first = s; p->next = q; q = p;
    p = s_text(call, "rArrow"); s = p;
    p = s_text(rep, ",1"); p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; t = p;

    p = s_new(ident); p->text = "n"; i = p;
    p = s_new(expr); p->text = "s_expr(n)"; p->first = i; q = p;
    p = s_new(call); p->text = "Name"; s = p;
    p = s_new(bind); p->text = "n"; p->first = s; p->next = q; q = p;
    p = s_text(call, "rArrow"); p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = t; t = p;

    p = s_new(alt); p->first = t; q = p;
    p = s_new(type); p->text = "struct s_node *"; p->next = q; q = p;
    p = s_new(rule); p->text = "Result"; p->first = q; p->next = r; r = p;

    /*
	Matcher
	    ← n:Name Colon u:UnaryRule { s_bind(n, u) }
	    / And u:UnaryRule { s_and(u) }
	    / Not u:UnaryRule { s_not(u) }
	    / And r:RawCode { s_guard(r) }
	    / u:UnaryRule → u
    */
    p = s_new(ident); p->text = "u"; i = p;
    p = s_new(expr); p->text="u"; p->first =i; q = p;
    p = s_new(call); p->text = "UnaryRule"; s = p;
    p = s_new(bind); p->text = "u"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; t = p;

    p = s_new(ident); p->text = "r"; i = p;
    p = s_new(expr); p->text="s_guard(r)"; p->first = i; q = p;
    p = s_new(call); p->text = "RawCode"; s = p;
    p = s_new(bind); p->text = "r"; p->first = s; p->next = q; q = p;
    p = s_new(call); p->text = "And"; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = t; t = p;

    p = s_new(ident); p->text = "u"; i = p;
    p = s_new(expr); p->text="s_not(u)"; p->first = i; q = p;
    p = s_new(call); p->text = "UnaryRule"; s = p;
    p = s_new(bind); p->text = "u"; p->first = s; p->next = q; q = p;
    p = s_new(call); p->text = "Not"; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = t; t = p;

    p = s_new(ident); p->text = "u"; i = p;
    p = s_new(expr); p->text="s_and(u)"; p->first = i; q = p;
    p = s_new(call); p->text = "UnaryRule"; s = p;
    p = s_new(bind); p->text = "u"; p->first = s; p->next = q; q = p;
    p = s_new(call); p->text = "And"; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = t; t = p;

    p = s_new(ident); p->text = "u"; i = p;
    p = s_new(ident); p->text = "n"; p->next = i; i = p;
    p = s_new(expr); p->text="s_bind(n, u)"; p->first = i; q = p;
    p = s_new(call); p->text = "UnaryRule"; s = p;
    p = s_new(bind); p->text = "u"; p->first = s; p->next = q; q = p;
    p = s_new(call); p->text = "Colon"; p->next = q; q = p;
    p = s_new(call); p->text = "Name"; s = p;
    p = s_new(bind); p->text = "n"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = t; t = p;

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
	    ← m:Matchers r:Result { s_seq(snoc(m, r)) }
	    / m:Matchers { s_seq(m) }
	Sequence
	    ← m:Matchers rArrow? r:Result { s_seq(snoc(m, r)) }
    */

    p = s_text(ident, "m"); i = p;
    p = s_text(expr, "s_seq(m)"); p->first = i; q = p;
    p = s_new(call); p->text = "Matchers"; s = p;
    p = s_new(bind); p->text = "m"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; t = p;

    p = s_new(ident); p->text = "r"; i = p;
    p = s_new(ident); p->text = "m"; p->next = i; i = p;
    p = s_new(expr); p->text="s_seq(snoc(m, r))"; p->first = i; q = p;
    p = s_new(call); p->text = "Result"; s = p;
    p = s_new(bind); p->text = "r"; p->first = s; p->next = q; q = p;
    p = s_new(call); p->text = "Matchers"; s = p;
    p = s_new(bind); p->text = "m"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = t; t = p;

    p = s_new(alt); p->first = t; q = p;
    p = s_new(type); p->text = "struct s_node *"; p->next = q; q = p;
    p = s_new(rule); p->text = "Sequence"; p->first = q; p->next = r; r = p;

    /*
	PrimRule
	    ← n:Name { s_call(n) } !lArrow !ColCol
	    / l:StringLit { s_lit(l) }
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

    p = s_kid(not, s_text(call, "ColCol"));
    p = cons(s_kid(not, s_text(call, "lArrow")), p); q = p;
    p = s_new(ident); p->text = "n"; i = p;
    p = s_new(expr); p->text="s_call(n)"; p->first = i; p->next = q; q = p;
    p = s_new(call); p->text = "Name"; s = p;
    p = s_new(bind); p->text = "n"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = t; t = p;

    p = s_new(alt); p->first = t; q = p;
    p = s_new(type); p->text = "struct s_node *"; p->next = q; q = p;
    p = s_new(rule); p->text = "PrimRule"; p->first = q; p->next = r; r = p;

    /*
	UnaryRule
	    ← p:PrimRule Query { s_query(p) }
	    / p:PrimRule Star { s_star(p) }
	    / p:PrimRule Plus { s_plus(p) }
	    / p:PrimRule → p
    */
    p = s_new(ident); p->text = "p"; i = p;
    p = s_new(expr); p->text="p"; p->first =i; q = p;
    p = s_new(call); p->text = "PrimRule"; s = p;
    p = s_new(bind); p->text = "p"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; t = p;

    p = s_new(ident); p->text = "p"; i = p;
    p = s_new(expr); p->text="s_plus(p)"; p->first = i; q = p;
    p = s_new(call); p->text = "Plus"; p->next = q; q = p;
    p = s_new(call); p->text = "PrimRule"; s = p;
    p = s_new(bind); p->text = "p"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = t; t = p;

    p = s_new(ident); p->text = "p"; i = p;
    p = s_new(expr); p->text="s_star(p)"; p->first = i; q = p;
    p = s_new(call); p->text = "Star"; p->next = q; q = p;
    p = s_new(call); p->text = "PrimRule"; s = p;
    p = s_new(bind); p->text = "p"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; p->next = t; t = p;

    p = s_new(ident); p->text = "p"; i = p;
    p = s_new(expr); p->text="s_query(p)"; p->first = i; q = p;
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
	    ← _ p:Preamble ds:Defns End { s_both(grammar, "yy", ds) }
    */
    p = s_new(ident); p->text = "ds"; i = p;
    p = s_new(ident); p->text = "p"; p->next = i; i = p;
    p = s_both(expr, "s_both(grammar, \"yy\", ds)", i); q = p;
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
    p = s_new(call); p->text = "Defns"; s = p;
    p = s_new(bind); p->text = "r"; p->first = s; p->next = q; q = p;
    p = s_new(seq); p->first = q; q = p;
    p = s_new(type); p->text = "struct s_node *"; p->next = q; q = p;
    p = s_new(rule); p->text = "Start"; p->first = q; p->next = r; r = p;
#endif

    p = s_new(grammar); p->text = "yy"; p->first = r;

    return p;
}
