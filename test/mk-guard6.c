#include "syntax.h"

char *prefix = 0;

/*
 * char *Identifier ← IdentStart IdentCont* { match() } 
 * void IdentStart ← c:Char &{ *c >= 'a' && *c <= 'z' }
 * void IdentCont ← IdentStart / d:Char &{ *d >= '0' && *d <= '9' }
 * char *Char ← . { match() }
 */
struct s_node *create(void) {
    struct s_node *p, *q, *r, *s, *t;

    /* char *Char ← . { match() } */
    p = new_node(expr); p->text = "match()"; q = p;
    p = new_node(any); p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "Char"; p->first = q; r = p;

    /* void IdentCont ← IdentStart / d:Char &{ *d >= '0' && *d <= '9' } */
    p = new_node(ident); p->text = "d"; s = p;
    p = new_node(guard); p->text = "*d >= '0' && *d <= '9'"; p->first = s; q = p;
    p = new_node(call); p->text = "Char"; s = p;
    p = new_node(bind); p->text = "d"; p->first = s; p->next = q; q = p;
    p = new_node(seq); p->first = q; s = p;
    p = new_node(call); p->text = "IdentStart"; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "IdentCont"; p->first = q; p->next = r; r = p;

    /* void IdentStart ← c:Char &{ *c >= 'a' && *c <= 'z' } */
    p = new_node(ident); p->text = "c"; s = p;
    p = new_node(guard); p->text = "*c >= 'a' && *c <= 'z'"; p->first = s; q = p;
    p = new_node(call); p->text = "Char"; s = p;
    p = new_node(bind); p->text = "c"; p->first = s; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "IdentStart"; p->first = q; p->next = r; r = p;

    /* char *Identifier	← IdentStart IdentCont* { match() } */
    p = new_node(seq); s = p;
    p = new_node(call); p->text = "IdentConts0"; q = p;
    p = new_node(call); p->text = "IdentCont"; p->next = q; q = p;
    p = new_node(seq); p->first = q; p->next = s; q = p;
    p = new_node(alt); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "IdentConts0"; p->first = q; p->next = r; r = p;

    p = new_node(expr); p->text = "match()"; q = p;
    p = new_node(call); p->text = "IdentConts0"; p->next = q; q = p;
    p = new_node(call); p->text = "IdentStart"; p->next = q; q = p;
    p = new_node(seq); p->first = q; q = p;
    p = new_node(type); p->text = "char *"; p->next = q; q = p;
    p = new_node(rule); p->text = "Identifier"; p->first = q; p->next = r; r = p;

    p = new_node(grammar); p->text = "yy"; p->first = r;

    return p;
}
