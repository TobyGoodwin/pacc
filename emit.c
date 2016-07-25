#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arg.h"
#include "error.h"
#include "syntax.h"
#include "template.h"

static char *g_name; /* grammar name */
static struct s_node *g_node; /* grammar root */
static struct s_node *cur_rule;

struct assoc {
    char *name;
    struct s_node *value;
};
static struct assoc *a_stack;
static int a_ptr, a_alloc;

#if 0
static void assoc_dump(void) {
    int i;

    for (i = 0; i < a_ptr; ++i)
	fprintf(stderr, "var %s @ pos %d\n", a_stack[i].name, i);
}
#endif

/* Low level routines to output things. */
int nr = 1;
static int indent = 2;
static int need_indent = 0;

static void c_str(char *s) {
    assert(!strchr(s, '\n'));
    if (*s == '#') need_indent = 0;
    if (need_indent) {
	int i;
	for (i = 0; i < indent; ++i) { fputs("  ", stdout); }
	need_indent = 0;
    }
    fputs(s, stdout);
}

static void c_strln(char *s) {
    c_str(s);
    putchar('\n');
    ++nr;
    need_indent = 1;
}

static void c_raw(char *s) {
    while (*s) {
	if (*s == '\n') ++nr;
	putchar(*s);
	++s;
    }
}

static void c_semi(void) { c_strln(";"); }
/* static void c_char(char i) { printf("%c", i); } */
static void c_int(int i) { printf("%d", i); }
static void c_long(long l) { printf("%ld", l); }
static void c_open(void) { ++indent; c_strln(" {"); }
static void c_close(void) { --indent; c_strln("}"); }
static void c_closet(void) { --indent; c_str("} "); }

enum opt_assign { no_assign, assign };

static void c_code(struct s_node *n, enum opt_assign a) {
    /* XXX except for a couple of desugared cases, there is always a coords as
     * a first child of expr, so it would be better to fix sugar.c and assert
     * here */
    if (n->first && n->first->type == coords) {
	int i;
	c_strln("");
	c_str("#line "); c_int(n->first->pair[0]);
	c_str(" \""); c_str(arg_input()); c_strln("\"");
	/* 1-based counting; subtract 1 for upcoming '(' and maybe '=' */
	for (i = 1; i < n->first->pair[1] - 1 - assign; ++i) putchar(' ');
    }
    if (a) putchar('=');
    putchar('('); c_raw(n->text); putchar(')');
    if (a) putchar(';');
    c_raw("\n");
    /* now a #line to take us back to the C output; line number of next line */
    c_str("#line "); c_long(nr + 1);
    c_str(" \""); c_str(arg_output()); c_strln("\"");
}

static void c_defines(void) {
    c_str("#define PACC_NAME "); c_strln(g_node->text);
    if (arg_feed()) {
	c_str("#define PACC_FEED_NAME "); c_str(g_node->text); c_strln("_feed");
    }
    c_strln("");
    c_strln("#define PACC_ASSERT 1");
    c_strln("#define PACC_CAN_TRACE 1");
    c_strln("");
}

static void associate(char *n, struct s_node *s) {
    if (a_ptr == a_alloc) {
	int l = 2 * a_alloc + 1;
	struct assoc *a = realloc(a_stack, l * sizeof *a_stack);
	if (!a) nomem();
	a_stack = a;
	a_alloc = l;
    }
    a_stack[a_ptr].name = n;
    a_stack[a_ptr++].value = s;
    
    assert(a_ptr > 0);
}

int associating = 0;

static int *f_stack;
static int f_ptr, f_alloc;

static void frame_start() {
    if (f_ptr == f_alloc) {
	int l = 2 * f_alloc + 1;
	int *f = realloc(f_stack, l * sizeof *f_stack);
	if (!f) nomem();
	f_stack = f;
	f_alloc = l;
    }
    f_stack[f_ptr++] = a_ptr;
}

static void frame_end() {
    assert(f_ptr > 0);
    a_ptr = f_stack[--f_ptr];
}

static char **t_list;
static int t_max, t_alloc;

static int type_list(char *t) {
    int i;

    if (strcmp(t, "void") == 0) return 0;
    for (i = 0; i < t_max; ++i) {
	if (strcmp(t, t_list[i]) == 0) return i;
    }
    if (t_max == t_alloc) {
	int l = 2 * t_alloc + 1;
	char **t = realloc(t_list, l * sizeof *t_list);
	if (!t) nomem();
	t_list = t;
	t_alloc = l;
    }
    t_list[t_max] = t;
    return t_max++;
}

static int rule_u(struct s_node *r) {
    return type_list(r->first->text);
}


static void grammar_pre(struct s_node *n) {
    int i, r = 0;
    struct s_node *p;
    static int cooked = 0;

    g_node = n;
    if (arg_defines()) {
	c_str("#include \"");c_str(arg_defines());c_strln("\"");
	if (arg_feed() && cooked) {
	    c_strln("#undef PACC_NAME");
	    c_strln("#define PACC_NAME PACC_FEED_NAME");
	}
    } else
	c_defines();
    ++cooked;

    pre_decl();

    /* We slightly simplify both building & walking the tree and insist
     * that every grammar starts with a preamble, which may be null.
     * It's a bit odd to represent no preamble with an empty preamble
     * node. */
    p = n->first;
    assert(p->type == preamble);
    if (!arg_defines() && p->text) c_raw(p->text);
    p = p->next;

    for ( ; p; p = p->next) {
	assert(p->type == rule);
	++r;
    }
    c_str("static const int n_rules = "); c_int(r); c_semi();
    c_str("static const int start_rule_id = "); c_long(n->first->next->id);
    c_semi();
    g_name = n->text;
    /* type of start rule is always u0 */
    type_list(n->first->next->first->text);
    for (p = n->first; p; p = p->next)
	if (p->type == rule) type_list(p->first->text);
    c_str("union PACC_SYM(vals)"); c_open();
    for (i = 0; i < t_max; ++i) {
	c_str(t_list[i]); c_str(" u"); c_int(i); c_semi();
    }

    c_close(); c_semi();

    /* XXX just for debugging */
    c_str("#define TYPE_PRINTF ");
    if (strcmp(n->first->next->first->text, "int") == 0) c_str("\"%d\"");
    else if (strcmp(n->first->next->first->text, "char *") == 0) c_str("\"%s\"");
    else c_str("\"%p\"");
    c_strln("");

    c_str("#define PACC_TYPE "); c_strln(n->first->next->first->text);

    pre_engine();

    c_str("_st=");
    c_long(n->first->type == preamble ? n->first->next->id : n->first->id);
    c_semi();
    c_strln("goto top;");
    c_strln("contin:");
    c_strln("_st=_cont;");
    c_strln("PACC_TRACE fprintf(stderr, \"continuing in state %d\\n\", _cont);");
    c_strln("top:");
    c_strln("PACC_TRACE fprintf(stderr, \"switch to state %d\\n\", _st);");
    c_str("switch(_st)"); c_open();
}

static void grammar_post(__attribute__((unused)) struct s_node *n) {
    c_strln("case -1: break;");
    c_close();
    post_engine();
}

static void debug_pre(char *type, struct s_node *n) {
    c_str("PACC_TRACE fprintf(stderr, \""); c_str(type); c_str(" ");
    c_long(n->id); c_strln(" @ col %zu?\\n\", _x);");
}

static void debug_post(char *type, struct s_node *n) {
    //printf("PACC_TRACE fprintf(stderr, \"%s %ld @ col %%ld => %%s\\n\", _x, status != no_parse ? \"yes\" : \"no\");\n",
	    //type, n->id);
    c_str("PACC_TRACE fprintf(stderr, \""); c_str(type); c_str(" "); c_long(n->id);
    c_strln(" %zu => %s\\n\", _x, _status != no_parse ? \"yes\" : \"no\");");

}

/* We recognise a properly-escaped C string in the grammar, and copy
 * that verbatim into the generated parser. That means we have to be a
 * bit careful in calculating the string's length. */
/* XXX no, not a *bit* careful but *very* careful, as we need to support
 * the full C:1999 syntax, including octal, hex, and universal escapes.
 * Furthermore, we will need test cases for it all.
 */
static void literal(struct s_node *n) {
    char *p;
    int l; /* length of named string */

    l = 0;
    for (p = n->text; *p; ++p) {
	if (*p == '\\') {
	    ++p;
	    assert(*p);
	    switch (*p) {
		case '\'': case '"': case '?': case '\\':
		case 'a': case 'b': case 'f': case 'n':
		case 'r': case 't': case 'v':
		    break;
		default:
		    assert(0);
	    }
	}
	++l;
    }
    debug_pre("lit", n);
    c_str("PACC_TRACE fprintf(stderr, \"lit "); c_long(n->id);
    c_strln(" @ col %zu => \", _x);");
    c_str("if (_x+"); c_int(l); c_str(" <= _pacc->input_length && ");
    c_str("memcmp(\""); c_str(n->text); c_str("\", _pacc->string + _x, ");
    c_int(l); c_str(") == 0)"); c_open();
    c_strln("_status = parsed;");
    c_str("_x += "); c_int(l); c_semi();
    c_close(); c_str("else"); c_open();
    c_str("_pacc_error(_pacc, \".\\\""); c_str(n->text);
    c_strln("\\\"\", _x);");
    c_strln("_status = no_parse;");
    c_close();
    debug_post("lit", n);
}

/* assumes utf-8 encoding */
static void any_emit(__attribute__((unused)) struct s_node *n) {
    debug_pre("any", n);
    c_str("if (_x < _pacc->input_length)"); c_open();
    c_strln("_pacc_any_i = _pacc_utf8_char(_pacc->string+_x, _pacc->input_length - _x, &_pacc_utf_cp);");
    c_strln("if (!_pacc_any_i) pacc_panic(\"invalid UTF-8 input\");");
    c_strln("_x += _pacc_any_i;");
    c_closet(); c_strln("else _status = no_parse;");
    debug_post("any", n);
}

static void rule_pre(struct s_node *n) {
    cur_rule = n;
    c_str("case "); c_long(n->id); c_str(": /* "); c_str(n->text);
    c_strln(" */");
    c_str("PACC_TRACE fprintf(stderr, \"rule "); c_long(n->id);
    c_str(" ("); c_str(n->text); c_str(") col %zu\\n\", _x)"); c_semi();
    c_strln("_x_rule = _x;");
    c_str("cur = _pacc_result(_pacc, _x, "); c_int(cur_rule->id); c_strln(");");
    c_str("if ((cur->rule & 3) == uncomputed)"); c_open(); /* memoization ON */
    //c_str("if (1 || (cur->rule & 3) == uncomputed)"); c_open(); /* m9n OFF */
}

static void rule_post(struct s_node *n) {
    c_strln("cur->rule = (cur->rule & ~3) | _status;");
    c_strln("cur->remainder = _x;");

    /* Rule made no progress: over-write error */
    /* XXX: See test/pacc/err0.c. This is wrong. What is right? */
    c_str("if (_pacc->err_col == _x_rule)"); c_open();
    c_strln("_pacc->err_valid = 0;");
    c_str("_pacc_error(_pacc, \"."); c_str(n->text); c_strln("\", _x_rule);");
    c_close();

    c_close(); /* this closes the open in rule_pre() */ 
    c_strln("goto contin;");
}

static void savecol(void) {
    c_strln("PACC_TRACE fprintf(stderr, \"save column registers\\n\");");
    c_strln("_pacc_Push(_x); _pacc_Push(cur->ev_valid);");
}

static void restcol(void) {
    c_strln("PACC_TRACE fprintf(stderr, \"restore column registers\\n\");");
    c_strln("_pacc_Pop(cur->ev_valid); _pacc_Pop(_x);");
}

static void accept_col(void) {
    c_strln("PACC_TRACE fprintf(stderr, \"accept column registers\\n\");");
    c_strln("_pacc_Discard(cur->ev_valid); _pacc_Discard(_x);");
}

static void seq_pre(struct s_node *n) {
    frame_start();
    c_str("PACC_TRACE fprintf(stderr, \"seq "); c_long(n->id);
    c_strln(" @ col %zu?\\n\", _x);");
    c_strln("_pacc_Push(_cont);");
    c_str("_cont = "); c_long(n->id); c_semi();
    c_strln("_status = parsed;");
}

static void seq_mid(__attribute__((unused)) struct s_node *n) {
    c_str("if (_status == no_parse)"); c_open();
    c_strln("goto contin;");
    c_close();
}

static void seq_post(struct s_node *n) {
    c_str("case "); c_long(n->id); c_strln(":");
    c_strln("_pacc_Pop(_cont);");
    c_str("PACC_TRACE fprintf(stderr, \"seq "); c_long(n->id); 
    c_strln(" @ col %zu => %s\\n\", _x_rule,
            _status != no_parse ? \"yes\" : \"no\" );");
    c_strln("PACC_TRACE fprintf(stderr, \"col is %zu\\n\", _x);");
    frame_end();
}

static void and_pre(struct s_node *n) {
    debug_pre("and", n);
    savecol();
}

static void and_post(struct s_node *n) {
    restcol();
    debug_post("and", n);
}

static void not_pre(struct s_node *n) {
    debug_pre("not", n);
    savecol();
}

static void not_post(struct s_node *n) {
    c_strln("_status = (_status == no_parse) ? parsed : no_parse;");
    restcol();
    debug_post("not", n);
}

static void bind_pre(struct s_node *n) {
    /* A binding may only contain a call... */
    assert(n->first && n->first->type == call);
    /* ... which itself must refer to a rule. */
    assert(n->first->first->type == rule);
    c_str("/* bind: "); c_str(n->text); c_strln(" */");
    debug_pre("bind", n);
    /* Save the name bound, and the rule to which it is bound. */
    associate(n->text, n->first->first);
    associating = 1;
    c_str("PACC_TRACE fprintf(stderr, \"will bind "); c_str(n->text);
    c_str(" @ rule "); c_long(n->first->first->id);
    c_strln(", col %zd\\n\", _x);");
}

static void bind_post(struct s_node *n) {
    associating = 0;
    c_str("/* end bind: "); c_str(n->text); c_strln(" */");
}

static void declarations(struct s_node *n) {
    int i;
    struct s_node *p;

    for (p = n->first; p; p = p->next) {
	if (p->type == coords) continue;
	assert(p->type == ident);
	/* Search for the name. Start from the end, so scopes nest. */
	for (i = a_ptr - 1; i >= 0; --i)
	    if (a_stack[i].value && strcmp(a_stack[i].name, p->text) == 0)
		break;

	/* It is not an error if we have a name without a binding: the
	 * parser will pick out names like "printf" from the code. */
	if (i < 0)
	    continue;

	assert(a_stack[i].value->type == rule);
	assert(a_stack[i].value->first->type == type);
	c_str("/* i is "); c_int(i); c_str(", type is ");
	c_str(a_stack[i].value->first->text); c_strln(" */");
	c_str(a_stack[i].value->first->text); c_str(" ");
	c_str(a_stack[i].name);c_semi();
    }
}

static void bindings(struct s_node *n) {
    struct s_node *p;

    for (p = n->first; p; p = p->next) {
	int i, p0, p1;

	if (p->type == coords) continue;
	assert(p->type == ident);
/*
	for (i = 0; i < a_ptr; ++i)
	    fprintf(stderr, "var %s @ pos %d\n", a_stack[i].name, i);
*/

	for (i = a_ptr - 1; i >= 0; --i)
	    if (a_stack[i].name && strcmp(a_stack[i].name, p->text) == 0)
		break;
	if (i < 0)
	    continue;

	/* XXX this is ugly. having introduced <frame> markers, we need to
	 * calculate a separate position on our name stack, and in
	 * evlis. this makes dummy bindings a bit stupid, since they
	 * were invented to keep the two in step.
	 */
	p0 = p1 = i;
#if 0
	for ( ; i >= 0; --i)
	    if (!a_stack[i].value && a_stack[i].name &&
		    strcmp(a_stack[i].name, "<frame>") == 0)
		--p1;
#endif
	c_str("_pos = "); c_int(p1); c_semi();

	c_str("PACC_TRACE fprintf(stderr, \"binding of "); c_str(p->text);
	c_strln(": pos %zu holds col %zu\\n\", _pos, _pacc_p->evlis[_pos].col);");

	c_str("PACC_TRACE fprintf(stderr, \"bind "); c_str(p->text);
	c_str(" to r"); c_long(a_stack[p0].value->id);
	c_strln(" @ c%zu\\n\", _pacc_p->evlis[_pos].col);");
	c_str("cur = _pacc_result(_pacc, _pacc_p->evlis[_pos].col, ");
	c_long(a_stack[p0].value->id); c_strln(");");

	c_str("if ((cur->rule & 3) != evaluated)"); c_open();
	c_strln("_pacc_Push(_x); _pacc_Push(_cont);");
	c_str("_cont = "); c_long(p->id); c_semi();
	c_strln("_pacc_ev_i = 0; goto eval_loop;");
	c_str("case "); c_long(p->id); c_strln(":");
	c_strln("_pacc_Pop(_cont); _pacc_Pop(_x);");
	c_close();
	c_str(p->text);
	c_str(" = cur->value.u"); c_int(rule_u(a_stack[p0].value));
	c_semi();
	c_str("PACC_TRACE fprintf(stderr, \"bound "); c_str(p->text);
	c_str(" to r"); c_long(a_stack[p0].value->id);
	c_strln(" @ c%zu ==> \" TYPE_PRINTF \"\\n\", _pacc_p->evlis[_pos].col, cur->value.u0);");
    }
}

static void emit_expr(struct s_node *n) {
    debug_pre("expr", n);
    /* When we encounter an expression whilst parsing, simply record the
     * expression's id. This will become the new state when we evaluate.
     */
    c_strln("assert(cur->expr_id == 0);");
    c_str("cur->expr_id = "); c_long(n->id); c_semi();

    /* When evaluating, we need to evaluate the expression! */
    c_str("case "); c_long(n->id); c_strln(":");
    c_str("if (_evaling)"); c_open();
    c_strln("struct pacc_mid *_pacc_p;"); /* parent */
    declarations(n);
    c_str("PACC_TRACE fprintf(stderr, \""); c_long(n->id);
    c_strln(": evaluating\\n\");");
    c_str("_pacc_p = cur = _pacc_result(_pacc, _x, "); c_int(cur_rule->id);
    c_strln(");");
    bindings(n);
    c_strln("cur = _pacc_p;");
    c_str("cur->value.u"); c_int(rule_u(cur_rule));
    c_code(n, assign);
    c_str("PACC_TRACE fprintf(stderr, \"stash \" TYPE_PRINTF \" to (%zu, ");
    c_int(cur_rule->id); c_strln(")\\n\", cur->value.u0, _x);");
    c_strln("goto _pacc_expr_done;");
    c_close();
}

static void guard_pre(struct s_node *n) {
    debug_pre("guard", n);
    c_str("/* "); c_long(n->id); c_strln(": guard_pre() */");
    c_open();
    c_strln("struct pacc_mid *_pacc_p;"); /* parent */
    declarations(n);
    c_strln("_pacc_p = cur; _evaling = 1;");
    bindings(n);
    c_strln("cur = _pacc_p; _evaling = 0;");
}

/* obviously, the tricky part of a guard is the bindings! */
static void guard_post(struct s_node *n) {
    /* XXX doesn't work, why not?
    c_strln("if (!"); c_code(n); c_strln(") status = no_parse;");
    */
    c_strln("_status = ("); c_code(n, no_assign);
    c_strln(") ? parsed : no_parse;");
    debug_post("guard", n);
    c_close();
}

static void emit_call(struct s_node *n) {
    /* The number of bindings is equal to the number of rule calls -
     * this is assumed XXX where?, so save dummy "binding" if we're not
     * binding.
     */
    if (!associating) associate(0, 0); 
    c_str("_pacc_save_core("); c_long(n->first->id); c_strln(", _x);");
    c_strln("_pacc_Push(_x_rule);"); /* XXX this is not callee saves */
    c_strln("_pacc_Push(_cont);");
    c_str("_cont = "); c_long(n->id); c_semi();
    c_str("_st = "); c_long(n->first->id); c_semi();
    c_str("/* call "); c_str(n->text); c_strln(" */");
    c_strln("goto top;");
    c_str("case "); c_long(n->id); c_str(": /* return from ");
    c_str(n->text); c_strln(" */");
    c_strln("_pacc_Pop(_cont);");
    c_strln("_status = cur->rule & 3;");
    c_strln("_x = cur->remainder;");
    c_strln("_pacc_Pop(_x_rule);");
    c_str("cur = _pacc_result(_pacc, _x_rule, "); c_int(cur_rule->id);
    c_strln(");");
}

static void alt_pre(struct s_node *n) {
    debug_pre("alt", n);
    c_strln("_pacc_Push(_cont);");
    c_str("_cont = "); c_long(n->id); c_semi();
    savecol();
}

static void alt_mid(struct s_node *n) {
    c_str("PACC_TRACE fprintf(stderr, \"alt "); c_long(n->id);
    c_strln(" @ col %zu => %s\\n\", _x, _status!=no_parse?\"yes\":\"no\");");
    c_str("if (_status != no_parse)"); c_open();
    accept_col();
    c_strln("goto contin;");
    c_close();
    restcol();
    savecol();
    c_strln("PACC_TRACE fprintf(stderr, \"col restored to %zu\\n\", _x);");
    c_str("PACC_TRACE fprintf(stderr, \"alt "); c_long(n->id);
    c_strln(" @ col %zu? (next alternative)\\n\", _x);");
}

static void alt_post(struct s_node *n) {
    c_str("if (_status == no_parse)"); c_open();
    restcol();
    c_close(); c_str("else"); c_open();
    accept_col();
    c_close();
    c_str("case "); c_long(n->id); c_strln(":");
    c_strln("_pacc_Pop(_cont);");
    c_str("PACC_TRACE fprintf(stderr, \"alt "); c_long(n->id);
    c_strln(" @ col %zu => %s\\n\", _x, _status!=no_parse?\"yes\":\"no\");");
    c_strln("PACC_TRACE fprintf(stderr, \"col is %zu\\n\", _x);");
}

static void cclass_pre(struct s_node *n) {
    debug_pre("cclass", n);
    c_str("if (_x < _pacc->input_length)"); c_open();
    c_strln("_pacc_any_i = _pacc_utf8_char(_pacc->string+_x, _pacc->input_length - _x, &_pacc_utf_cp);");
    c_strln("if (!_pacc_any_i) pacc_panic(\"invalid UTF-8 input\");");
    c_str("if (");
    if (n->text[0] == '^') c_str("!(");
}

static void emit_cceq(struct s_node *n) {
    c_str("_pacc_utf_cp=="); c_int(n->number);
    if (n->next) c_str(" || ");
}

static void emit_ccge(struct s_node *n) {
    assert(n->next && n->next->type == ccle);
    c_str("(_pacc_utf_cp>="); c_int(n->number); c_str("&&");
}

static void emit_ccle(struct s_node *n) {
    c_str("_pacc_utf_cp<="); c_int(n->number); c_str(")");
    if (n->next) c_str(" || ");
}

static void cclass_post(struct s_node *n) {
    char *esc, *p, *q;

    /* escape " characters in the string */
    esc = realloc(0, 2 * strlen(n->text) + 1);
    if (!esc) nomem();
    for (p = n->text, q = esc; *p; ++p, ++q) {
        if (*p == '\"')
            *q++ = '\\';
        *q = *p;
    }
    *q = '\0';
    
    if (n->text[0] == '^') c_str(")");
    c_str(")"); c_open();
    c_strln("_status = parsed;");
    c_strln("_x += _pacc_any_i;");
    c_close(); c_str("else"); c_open();
    //error(n->text, 1);
    c_str("_pacc_error(_pacc, \".["); c_str(esc); c_strln("]\", _x);");
    c_strln("_status = no_parse;");
    c_close(); c_close();
    c_str("else"); c_open();
    c_str("_pacc_error(_pacc, \".["); c_str(esc); c_strln("]\", _x);");
    c_strln("_status = no_parse;");
    c_close();
    free(esc);
    debug_post("cclass", n);
}

static void rep_pre(__attribute__((unused)) struct s_node *n) {
    int sugar = 1;

    assert(!sugar);
}

static void (*pre[s_type_max])(struct s_node *);
static void (*mid[s_type_max])(struct s_node *);
static void (*post[s_type_max])(struct s_node *);

static void node(struct s_node *n) {
    struct s_node *p;

    if (pre[n->type]) pre[n->type](n);

    /* XXX could optimize with seq_last() etc. I think */
    if (s_has_children(n->type))
	for (p = n->first; p; p = p->next) {
	    node(p);
	    if (p->next)
		if (mid[n->type]) mid[n->type](n);
	}

    if (post[n->type]) post[n->type](n);
}

void emit(struct s_node *g) {
    pre[grammar] = grammar_pre; pre[rule] = rule_pre;
    pre[alt] = alt_pre; pre[seq] = seq_pre;
    pre[and] = and_pre; pre[not] = not_pre;
    pre[bind] = bind_pre; pre[expr] = emit_expr;
    pre[guard] = guard_pre;
    pre[call] = emit_call; pre[lit] = literal; pre[any] = any_emit;
    pre[cclass] = cclass_pre; pre[cceq] = emit_cceq;
    pre[ccge] = emit_ccge; pre[ccle] = emit_ccle;
    pre[rep] = rep_pre;

    mid[alt] = alt_mid; mid[seq] = seq_mid;

    post[grammar] = grammar_post; post[rule] = rule_post;
    post[alt] = alt_post; post[seq] = seq_post;
    post[and] = and_post; post[not] = not_post;
    post[bind] = bind_post;
    post[guard] = guard_post;
    post[cclass] = cclass_post;

    node(g);
    fflush(stdout);
}

static void h_lines(char *yy, char *type) {
    printf("extern int %s_trace;\n", yy);
    printf("extern struct pacc_parser *%s_new(void);\n", yy);
    printf("extern void %s_input(struct pacc_parser *, char *, size_t l);\n", yy);
    printf("extern void %s_destroy(struct pacc_parser *);\n", yy);
    printf("extern int %s_parse(struct pacc_parser *);\n", yy);
    printf("extern %s %s_result(struct pacc_parser *);\n", type, yy);
    printf("extern char *%s_error(struct pacc_parser *);\n", yy);
    printf("extern char *%s_pos(struct pacc_parser *, const char *);\n", yy);
    printf("extern int %s_wrap(const char *, char *, size_t, %s *result);\n", yy, type);
}

void header(struct s_node *g) {
    size_t l;
    char *newname;
    char *yy = g->text;
    char *preamble = g->first->text;
    char *type = g->first->next->first->text;
    
    c_defines();

    if (preamble) puts(preamble);

    printf("#include <sys/types.h>\n"); /* for off_t */
    printf("struct pacc_parser;\n");
    h_lines(yy, type);

    if (arg_feed()) {
	/* XXX cut'n'paste from cook.c */
	l = strlen(yy) + strlen("_feed") + 1;
	newname = realloc(0, l);
	if (!newname) nomem();
	strcpy(newname, g->text);
	strcat(newname, "_feed");
	h_lines(newname, type);
	free(newname);
    }

    fflush(stdout);
}
