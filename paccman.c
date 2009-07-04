/* For bootstrapping:

struct s_node *Start ← Definition

# need types!
Definition ← i:Identifier EQUALS r:AltRule { mkrule(i, r) }
AltRule ← s:SeqRule t:SeqRules0 { cons(s, t) }
SeqRules0 ← SLASH u:SeqRule v:SeqRules0 { cons(u, v) } / ε { 0 }
SeqRule ← i:Identifier { mkcall(i) }

char *Identifier	← IdentStart IdentCont* { match() } -
void IdentStart		← c:Char &{ (c>='a'&&c<='z') || (c>='A'&&c<='Z') || c == '_' }
void IdentCont		← IdentStart / c:Char &{ c >= '0' && c <= '9' }

char *Char		← . { match() }
void EQUALS		← '=' -
void SLASH		← '/' -
-			← ' ' - / ε

*/

#include "syntax.h"

/* Hmm. What are we going to do about the raw code prefix? It could be a
 * node in the tree, but that hardly seems worthwhile.
 */
char *prefix = "#include \"syntax.h\"\n";

struct s_node *create(void) {
    struct s_node *p, *q, *r, *s, *t;

    /* void - ← ' ' - / ε */
    p = new_node(seq); s = p;
    p = new_node(call); p->text = "-"; q = p;
    p = new_node(lit); p->text = " "; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;
    p = new_node(alt); p->first = q;
    p = new_node(type); p->text = "int"; p->next = q; q = p;
    p = new_node(rule); p->text = "-"; p->first = q; r = p;

    /* void SLASH ← '/' - */
    p = new_node(call); p->text = "-"; q = p;
    p = new_node(lit); p->text = "/"; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "int"; p->next = q; q = p;
    p = new_node(rule); p->text = "SLASH"; p->first = q; p->next = r; r = p;

    /* void EQUALS ← '=' - */
    p = new_node(call); p->text = "-"; q = p;
    p = new_node(lit); p->text = "="; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "int"; p->next = q; q = p;
    p = new_node(rule); p->text = "EQUALS"; p->first = q; p->next = r; r = p;

    /* char *Char		← . { match() } */
    p = new_node(expr); p->text = "match()"; q = p;
    p = new_node(any); p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "Char"; p->first = q; p->next = r; r = p;

    /* void IdentCont		← IdentStart / c:Char &{ c >= '0' && c <= '9' } */
    p = new_node(ident); p->text = "c"; s = p;
    p = new_node(guard); p->text = "*c >= '0' && *c <= '9'"; p->first = s; q = p;
    p = new_node(call); p->text = "Char"; s = p;
    p = new_node(bind); p->text = "c"; p->first = s; p->next = q; q = p;
    p = new_node(seq); p->first = q; s = p;
    p = new_node(call); p->text = "IdentStart"; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "IdentCont"; p->first = q; p->next = r; r = p;

    /* void IdentStart		← c:Char &{ (c>='a'&&c<='z') || (c>='A'&&c<='Z') || c == '_' } */
    p = new_node(expr); p->text = "match()"; q = p;
    p = new_node(ident); p->text = "c"; s = p;
    p = new_node(guard); p->text = "(*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z') || *c == '_'"; p->first = s; p->next = q; q = p;
    p = new_node(call); p->text = "Char"; s = p;
    p = new_node(bind); p->text = "c"; p->first = s; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "IdentStart"; p->first = q; p->next = r; r = p;

    /* char *Identifier	← IdentStart IdentCont* { match() } - */
    p = new_node(seq); s = p;
    p = new_node(call); p->text = "IdentConts0"; q = p;
    p = new_node(call); p->text = "IdentCont"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "IdentConts0"; p->first = q; p->next = r; r = p;

    p = new_node(call); p->text = "-"; q = p;
    p = new_node(expr); p->text = "match()"; p->next = q; q = p;
    p = new_node(call); p->text = "IdentConts0"; p->next = q; q = p;
    p = new_node(call); p->text = "IdentStart"; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "Identifier"; p->first = q; p->next = r; r = p;

    /* struct s_node *SeqRule ← i:Identifier { mkcall(i) } */
    p = new_node(expr); p->text = "printf(\"!!! mkcall(%s)\\n\", i), mkcall(i)"; q = p;
    p = new_node(call); p->text = "Identifier"; s = p;
    p = new_node(bind); p->text = "i"; p->first = s; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "struct s_node *"; p->next = q; q = p;
    p = new_node(rule); p->text = "SeqRule"; p->first = q; p->next = r; r = p;

    /* struct s_node *AltRule ← SeqRule (SLASH SeqRule)* */
    /* but we don't have star yet, and it's not clear how the semantic
     * expressions would work, so rewrite to:
     * struct s_node *AltRule ← s:SeqRule t:SeqRules0 { cons(s, t) }
     * struct s_node *SeqRules0 ← SLASH u:SeqRule v:SeqRules0 { cons(u, v) } / ε { 0 }
     */
    p = new_node(expr); p->text = "0"; q = p;
    p = new_node(seq); p->first = q; s = p;
    p = new_node(expr); p->text = "cons(u, v)"; q = p;
    p = new_node(call); p->text = "SeqRules0"; t = p;
    p = new_node(bind); p->text = "v"; p->first = t; p->next = q; q = p;
    p = new_node(call); p->text = "SeqRule"; t = p;
    p = new_node(bind); p->text = "u"; p->first = t; p->next = q; q = p;
    p = new_node(call); p->text = "SLASH"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(type); p->text = "struct s_node *"; p->next = q; q = p;
    p = new_node(rule); p->text = "SeqRules0"; p->first = q; p->next = r; r = p;

    p = new_node(expr); p->text = "cons(s, t)"; q = p;
    p = new_node(call); p->text = "SeqRules0"; t = p;
    p = new_node(bind); p->text = "t"; p->first = t; p->next = q; q = p;
    p = new_node(call); p->text = "SeqRule"; t = p;
    p = new_node(bind); p->text = "s"; p->first = t; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "struct s_node *"; p->next = q; q = p;
    p = new_node(rule); p->text = "AltRule"; p->first = q; p->next = r; r = p;

    p = new_node(expr); p->text = "mkrule(i, r)"; q = p;
    p = new_node(call); p->text = "AltRule"; t = p;
    p = new_node(bind); p->text = "r"; p->first = t; p->next = q; q = p;
    p = new_node(call); p->text = "EQUALS"; p->next = q; q = p;
    p = new_node(call); p->text = "Identifier"; t = p;
    p = new_node(bind); p->text = "i"; p->first = t; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "struct s_node *"; p->next = q; q = p;
    p = new_node(rule); p->text = "Definition"; p->first = q; p->next = r; r = p;


    p = new_node(expr); p->text = "i"; q = p;
    p = new_node(call); p->text = "Definition"; s = p;
    p = new_node(bind); p->text = "i"; p->first = s; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "struct s_node *"; p->next = q; q = p;
    p = new_node(rule); p->text = "Start"; p->first = q; p->next = r; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    resolve(p, p);

    return p;
}
