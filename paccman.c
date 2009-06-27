/* For bootstrapping:

Rule ← AltRule
char *AltRule ← SeqRule (SLASH SeqRule)*
char *SeqRule ← i:Identifier { mkrule(i) }

char *Identifier	← IdentStart IdentCont* { match() } -
void IdentStart		← c:Char &{ (c>='a'&&c<='z') || (c>='A'&&c<='Z') || c == '_' }
void IdentCont		← IdentStart / c:Char &{ c >= '0' && c <= '9' }

char *Char		← . { match() }
void SLASH		← '/' -
-			← ' ' - / ε

*/

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

    /* char *SeqRule ← Identifier -- Not really! */
    p = new_node(expr); p->text = "i"; q = p;
    p = new_node(call); p->text = "Identifier"; s = p;
    p = new_node(bind); p->text = "i"; p->first = s; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "SeqRule"; p->first = q; p->next = r; r = p;

    /* char *AltRule ← SeqRule (SLASH SeqRule)* */
    /* but we don't have star yet so rewrite to:
     * char *AltRule ← SeqRule SeqRules0
     * char *SeqRules0 ← SLASH SeqRule SeqRules1 / ε
     */
    p = new_node(seq); s = p;
    p = new_node(call); p->text = "SeqRules0"; q = p;
    p = new_node(call); p->text = "SeqRule"; p->next = q; q = p;
    p = new_node(call); p->text = "SLASH"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "SeqRules0"; p->first = q; p->next = r; r = p;

    p = new_node(expr); p->text = "match()"; q = p;
    p = new_node(call); p->text = "SeqRules0"; p->next = q; q = p;
    p = new_node(call); p->text = "SeqRule"; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "AltRule"; p->first = q; p->next = r; r = p;

    p = new_node(expr); p->text = "i"; q = p;
    p = new_node(call); p->text = "AltRule"; s = p;
    p = new_node(bind); p->text = "i"; p->first = s; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "Start"; p->first = q; p->next = r; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    resolve(p, p);

    return p;
}
