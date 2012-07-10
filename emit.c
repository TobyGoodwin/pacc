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
//    printf(" %ld",nr);
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
static void c_char(char i) { printf("%c", i); }
static void c_int(int i) { printf("%d", i); }
static void c_long(long l) { printf("%ld", l); }
static void c_open(void) { ++indent; c_strln(" {"); }
static void c_close(void) { --indent; c_strln("}"); }

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

    pre_decl();

    g_node = n;

    /* We slightly simplify both building & walking the tree and insist
     * that every grammar starts with a preamble, which may be null.
     * It's a bit odd to represent no preamble with an empty preamble
     * node. */
    p = n->first;
    assert(p->type == preamble);
    if (p->text) c_raw(p->text);
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
    c_str("union "); c_str(g_name); c_str("_union"); c_open();
    for (i = 0; i < t_max; ++i) {
	c_str(t_list[i]); c_str(" u"); c_int(i); c_semi();
    }

#if 0
    c_str(n->first->next->first->text); c_strln(" u0;");
    for (p = n->first; p; p = p->next) {
	if (p->type != rule) continue;
	/* XXX obviously, we need to weed out duplicates, "void", etc. */
	type_list(p->first->text);
	//c_str(p->first->text); c_str(" u"); c_long(p->id); c_semi();
	c_str(p->first->text);
	c_str(" u"); type_list(p->first->text); c_semi();
    }
#endif
    c_close(); c_semi();

    /* XXX just for debugging */
    c_str("#define TYPE_PRINTF ");
    if (strcmp(n->first->next->first->text, "int") == 0) c_str("\"%d\"");
    else if (strcmp(n->first->next->first->text, "char *") == 0) c_str("\"%s\"");
    else c_str("\"%p\"");
    c_strln("");

    c_str("#define PACC_TYPE "); c_strln(n->first->next->first->text);

    pre_engine(n->text);

    c_str("st=");
    c_long(n->first->type == preamble ? n->first->next->id : n->first->id);
    c_semi();
    c_strln("top:");
    c_strln("Trace fprintf(stderr, \"switch to state %d\\n\", st);");
    c_str("switch(st)"); c_open();
}

static void grammar_post(__attribute__((unused)) struct s_node *n) {
    c_strln("case -1: break;");
    c_close();
    post_engine(n->text);
}

static void debug_pre(char *type, struct s_node *n) {
    c_str("Trace fprintf(stderr, \""); c_str(type); c_long(n->id);
    c_strln(" @ col %ld?\\n\", col);");
}

static void debug_post(char *type, struct s_node *n) {
    //printf("Trace fprintf(stderr, \"%s %ld @ col %%ld => %%s\\n\", col, status != no_parse ? \"yes\" : \"no\");\n",
	    //type, n->id);
    c_str("Trace fprintf(stderr, \""); c_str(type); c_str(" "); c_long(n->id);
    c_strln(" %ld => %s\\n\", col, status != no_parse ? \"yes\" : \"no\");");

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
		case '\'': case '\"': case '\?': case '\\':
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
    //printf("Trace fprintf(stderr, \"lit %ld @ col %%ld => \", col);\n", n->id);
    c_str("Trace fprintf(stderr, \"lit "); c_long(n->id);
    c_strln(" @ col %ld => \", col);");
    //printf("if (col + %d <= _pacc->input_length &&\n", l);
    c_str("if (col+"); c_int(l); c_str(" <= _pacc->input_length && ");
    c_str("memcmp(\""); c_str(n->text); c_str("\", _pacc->string + col, ");
    c_int(l); c_str(") == 0)"); c_open();
    //printf("        memcmp(\"%s\", _pacc->string + col, %d) == 0) {\n", n->text, l);
    c_strln("status = parsed;");
    c_str("col += "); c_int(l); c_semi();
    //printf("    status = parsed;\n");
    //printf("    col += %d;\n", l);
    c_close(); c_str("else"); c_open();
    //printf("} else {\n");
    //error(n->text, 1);
    c_str("error(_pacc, \"\\\""); c_str(n->text); c_strln("\\\"\", col);");
    //printf("    error(_pacc, \"\\\"%s\\\"\", col);\n", n->text);
    c_strln("status = no_parse;");
    c_close();
    //printf("    status = no_parse;\n");
    //printf("}\n");
    debug_post("lit", n);
}

/* assumes utf-8 encoding */
static void any_emit(__attribute__((unused)) struct s_node *n) {
    c_str("if (col < _pacc->input_length)"); c_open();
    c_strln("_pacc_any_i = _pacc_utf8_char(_pacc->string+col, _pacc->input_length - col, &_pacc_utf_cp);");
    c_strln("if (!_pacc_any_i) panic(\"invalid UTF-8 input\");");
    c_strln("status = parsed; col += _pacc_any_i;");
    c_close(); c_strln(" else status = no_parse;");
}

static void rule_pre(struct s_node *n) {
    cur_rule = n;
    c_str("case "); c_long(n->id); c_str(": /* "); c_str(n->text);
    c_strln(" */");
    //printf("case %ld: /* %s */\n", n->id, n->text); 
    c_str("Trace fprintf(stderr, \"rule "); c_long(n->id);
    c_str(" ("); c_str(n->text); c_str(") col %ld\\n\", col)"); c_semi();
    //printf("Trace fprintf(stderr, \"rule %ld (%s) col %%ld\\n\", col);\n", n->id, n->text);
    c_strln("rule_col = col;");
    //printf("rule_col = col;\n");
    c_str("cur = _pacc_result(_pacc, col, "); c_int(cur_rule->id); c_strln(");");
    //printf("cur = _pacc_result(_pacc, col, %d);\n", cur_rule);
    c_str("if ((cur->rule & 3) == uncomputed)"); c_open(); /* memoization ON */
    //c_str("if (1 || (cur->rule & 3) == uncomputed)"); c_open(); /* m9n OFF */
    //printf("if ((cur->rule & 3) == uncomputed) {\n");
    //printf("if (1 || (cur->rule & 3) == uncomputed) {\n"); /* memoization OFF */
}

static void rule_post(struct s_node *n) {
    c_strln("cur->rule = (cur->rule & ~3) | status;");
    c_strln("cur->remainder = col;");
    //printf("    cur->rule = (cur->rule & ~3) | status;\n");
    //printf("    cur->remainder = col;\n");

    /* XXX: See test/pacc/err0.c. This is wrong. What is right? */
    c_str("if (_pacc->err_col == rule_col)"); c_open();
    c_strln("_pacc->err_valid = 0;");
    c_str("error(_pacc, \""); c_str(n->text); c_strln("\", rule_col);");
    c_close();

    c_close(); /* this closes the open in rule_pre() */ 
    c_strln("goto contin;");
#if 0
    printf("    if (_pacc->err_col == rule_col) {\n");
    printf("        _pacc->err_valid = 0;\n"); /* Rule made no progress: over-write error */
    //error(n->text, 0);
    printf("    error(_pacc, \"%s\", col);\n", n->text);
    printf("    }\n");

    printf("}\n");
    printf("goto contin;\n");
#endif
}

static void savecol(void) {
    //printf("Trace fprintf(stderr, \"save column registers\\n\");\n");
    //printf("_pacc_Push(col); _pacc_Push(cur->ev_valid);\n");
    c_strln("Trace fprintf(stderr, \"save column registers\\n\");");
    c_strln("_pacc_Push(col); _pacc_Push(cur->ev_valid);");
}

static void restcol(void) {
    //printf("Trace fprintf(stderr, \"restore column registers\\n\");\n");
    //printf("_pacc_Pop(cur->ev_valid); _pacc_Pop(col);\n");
    c_strln("Trace fprintf(stderr, \"restore column registers\\n\");");
    c_strln("_pacc_Pop(cur->ev_valid); _pacc_Pop(col);");
}

static void accept_col(void) {
    c_strln("Trace fprintf(stderr, \"accept column registers\\n\");");
    c_strln("_pacc_Discard(cur->ev_valid); _pacc_Discard(col);");
}

static void seq_pre(struct s_node *n) {
    frame_start();
    //printf("Trace fprintf(stderr, \"seq %ld @ col %%ld?\\n\", col);\n", n->id);
    //printf("_pacc_Push(cont);\n");
    //printf("cont = %ld;\n", n->id);
    //printf("status = parsed;\n"); /* empty sequence */
    c_str("Trace fprintf(stderr, \"seq "); c_long(n->id);
    c_strln(" @ col %ld?\\n\", col);");
    c_strln("_pacc_Push(cont);");
    c_str("cont = "); c_long(n->id); c_semi();
    c_strln("status = parsed;");
}

static void seq_mid(__attribute__((unused)) struct s_node *n) {
    c_str("if (status == no_parse)"); c_open();
    c_strln("goto contin;");
    c_close();
    //printf("if (status == no_parse) {\n");
    //printf("    goto contin;\n");
    //printf("}\n");
}

static void seq_post(struct s_node *n) {
    //printf("case %ld:\n", n->id);
    //printf("_pacc_Pop(cont);\n");
    //printf("Trace fprintf(stderr, \"seq %ld @ col %%ld => %%s\\n\", rule_col, status!=no_parse?\"yes\":\"no\");\n", n->id);
    //printf("Trace fprintf(stderr, \"col is %%ld\\n\", col);\n");
    c_str("case "); c_long(n->id); c_strln(":");
    c_strln("_pacc_Pop(cont);");
    c_str("Trace fprintf(stderr, \"seq "); c_long(n->id); 
    c_strln(" @ col %ld => %s\\n\", rule_col, status!=no_parse?\"yes\":\"no\");");
    c_strln("Trace fprintf(stderr, \"col is %ld\\n\", col);");
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
    //printf("status = (status == no_parse) ? parsed : no_parse;\n");
    c_strln("status = (status == no_parse) ? parsed : no_parse;");
    restcol();
    debug_post("not", n);
}

static void bind_pre(struct s_node *n) {
    /* A binding may only contain a call... */
    assert(n->first && n->first->type == call);
    /* ... which itself must refer to a rule. */
    assert(n->first->first->type == rule);
    c_str("/* bind: "); c_str(n->text); c_strln(" */");
    //printf("/* bind: %s */\n", n->text);
    debug_pre("bind", n);
    /* Save the name bound, and the rule to which it is bound. */
    associate(n->text, n->first->first);
    associating = 1;
    //printf("Trace fprintf(stderr, \"will bind %s @ rule %ld, col %%ld\\n\", col);\n", n->text, n->first->first->id);
    c_str("Trace fprintf(stderr, \"will bind "); c_str(n->text);
    c_str(" @ rule "); c_long(n->first->first->id);
    c_strln(", col %ld\\n\", col);");
}

static void bind_post(struct s_node *n) {
    associating = 0;
    //printf("/* end bind: %s */\n", n->text);
    c_str("/* end bind: "); c_str(n->text); c_strln(" */");
}

static void declarations(struct s_node *n) {
    int i;
    struct s_node *p;

#if 0
    for (p = n->first; p; p = p->next) {
	fprintf(stderr, "%s, ", p->text);
    }
    fprintf(stderr, "\n");
#endif

    for (p = n->first; p; p = p->next) {
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
	//printf("/* i is %d */\n", i);
	//printf("/* type is %s */\n", a_stack[i].value->first->text);
	//printf("    %s %s;\n",
		//a_stack[i].value->first->text, a_stack[i].name);
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
	//printf("pos = %d;\n", p1);
	c_str("pos = "); c_int(p1); c_semi();

	//printf("Trace fprintf(stderr, \"binding of %s: pos %%ld holds col %%ld\\n\", pos, _pacc_p->evlis[pos].col);\n", p->text);
	c_str("Trace fprintf(stderr, \"binding of "); c_str(p->text);
	c_strln(": pos %ld holds col %ld\\n\", pos, _pacc_p->evlis[pos].col);");

	//printf("    Trace fprintf(stderr, \"bind %s to r%ld @ c%%ld\\n\", _pacc_p->evlis[pos].col);\n", p->text, a_stack[p0].value->id);
	c_str("Trace fprintf(stderr, \"bind "); c_str(p->text);
	c_str(" to r"); c_long(a_stack[p0].value->id);
	c_strln(" @ c%ld\\n\", _pacc_p->evlis[pos].col);");
	//printf("    cur = _pacc_result(_pacc, _pacc_p->evlis[pos].col, %ld);\n", a_stack[p0].value->id);
	c_str("cur = _pacc_result(_pacc, _pacc_p->evlis[pos].col, ");
	c_long(a_stack[p0].value->id); c_strln(");");

	//printf("    if ((cur->rule & 3) != evaluated) {\n");
	//printf("        _pacc_Push(col); _pacc_Push(cont);\n");
	//printf("        cont = %ld;\n", p->id);
	//printf("	_pacc_ev_i = 0; goto eval_loop;\n");
	//printf("case %ld:     _pacc_Pop(cont); _pacc_Pop(col);\n", p->id);
	//printf("    }\n");
	//printf("    %s = cur->value.u%ld;\n", p->text, a_stack[p0].value->id);
	//printf("    Trace fprintf(stderr, \"bound %s to r%ld @ c%%ld ==> \" TYPE_PRINTF \"\\n\", _pacc_p->evlis[pos].col, cur->value.u0);\n", p->text, a_stack[p0].value->id);
	c_str("if ((cur->rule & 3) != evaluated)"); c_open();
	c_strln("_pacc_Push(col); _pacc_Push(cont);");
	c_str("cont = "); c_long(p->id); c_semi();
	c_strln("_pacc_ev_i = 0; goto eval_loop;");
	c_str("case "); c_long(p->id); c_strln(":");
	c_strln("_pacc_Pop(cont); _pacc_Pop(col);");
	c_close();
	c_str(p->text);
	//c_str(" = cur->value.u"); c_long(a_stack[p0].value->id); c_semi();
	c_str(" = cur->value.u"); c_int(rule_u(a_stack[p0].value));
	c_semi();
	c_str("Trace fprintf(stderr, \"bound "); c_str(p->text);
	c_str(" to r"); c_long(a_stack[p0].value->id);
	c_strln(" @ c%ld ==> \" TYPE_PRINTF \"\\n\", _pacc_p->evlis[pos].col, cur->value.u0);");
    }
}

static void emit_expr(struct s_node *n) {
    debug_pre("expr", n);
    /* When we encounter an expression whilst parsing, simply record the
     * expression's id. This will become the new state when we evaluate.
     */
    //printf("assert(cur->expr_id == 0);\n");
    //printf("cur->expr_id = %ld;\n", n->id);
    c_strln("assert(cur->expr_id == 0);");
    c_str("cur->expr_id = "); c_long(n->id); c_semi();

    /* When evaluating, we need to evaluate the expression! */
//    printf("case %ld:\n", n->id);
//    printf("if (evaluating) {\n");
//    printf("    struct intermed *_pacc_p;\n"); /* parent */
//    declarations(n);
//    printf("    Trace fprintf(stderr, \"%ld: evaluating\\n\");\n", n->id);
//    printf("    _pacc_p = cur = _pacc_result(_pacc, col, %d);\n", cur_rule);
//    bindings(n);
//    printf("    cur = _pacc_p;\n");
//    printf("    cur->value.u%d = (%s);\n", cur_rule, n->text);
//    printf("    Trace fprintf(stderr, \"stash \" TYPE_PRINTF \" to (%%ld, %d)\\n\", cur->value.u0, col);\n", cur_rule);
//    printf("    goto _pacc_expr_done;\n");
//    printf("}\n");
    c_str("case "); c_long(n->id); c_strln(":");
    c_str("if (evaluating)"); c_open();
    c_strln("struct intermed *_pacc_p;"); /* parent */
    declarations(n);
    c_str("Trace fprintf(stderr, \""); c_long(n->id);
    c_strln(": evaluating\\n\");");
    c_str("_pacc_p = cur = _pacc_result(_pacc, col, "); c_int(cur_rule->id);
    c_strln(");");
    bindings(n);
    c_strln("cur = _pacc_p;");
    c_str("cur->value.u"); c_int(rule_u(cur_rule)); c_strln("=");
    /* XXX except for pacc0, there is always a coords as a first child of
     * expr, so it would be better to fix pacc0 and assert here */
    if (n->first && n->first->type == coords) {
	int i;
	c_str("#line "); c_int(n->first->pair[0]);
	c_str(" \""); c_str(arg_input()); c_strln("\"");
	/* We'll do our own indenting here */
	need_indent = 0;
	/* 1 because we've gone one too many; one for the upcoming ( */
	for (i = 0; i < n->first->pair[1] - 2; ++i) c_str(" ");
    }
    c_str("(");c_str(n->text); c_strln(");");
    c_str("#line "); c_long(nr + 1);
    c_str(" \""); c_str(arg_output()); c_strln("\"");
    c_str("Trace fprintf(stderr, \"stash \" TYPE_PRINTF \" to (%ld, ");
    c_int(cur_rule->id); c_strln(")\\n\", cur->value.u0, col);");
    c_strln("goto _pacc_expr_done;");
    c_close();
}

static void guard_pre(struct s_node *n) {
    debug_pre("guard", n);
    c_str("/* "); c_long(n->id); c_strln(": guard_pre() */");
    c_open();
    c_strln("struct intermed *_pacc_p;"); /* parent */
    declarations(n);
    c_strln("_pacc_p = cur; evaluating = 1;");
    bindings(n);
    c_strln("cur = _pacc_p; evaluating = 0;");
    //printf("/* %ld: guard_pre() */\n", n->id);
    //printf("{\n    struct intermed *_pacc_p;\n"); /* parent */
    //declarations(n);
    //printf("    _pacc_p = cur; evaluating = 1;\n");
    //bindings(n);
    //printf("    cur = _pacc_p; evaluating = 0;\n");
}

/* obviously, the tricky part of a guard is the bindings! */
static void guard_post(struct s_node *n) {
    c_strln("status =");
    /* XXX except for pacc0, there is always a coords as a first child
     * of expr, so it would be better to fix pacc0 and assert here */
    //assert(n->first && n->first->type == coords);
    if (n->first && n->first->type == coords) {
	int i;
	c_str("#line "); c_int(n->first->pair[0]);
	c_str(" \""); c_str(arg_input()); c_strln("\"");
	/* We'll do our own indenting here */
	need_indent = 0;
	/* 1 because we've gone one too many; one for the upcoming ( */
	for (i = 0; i < n->first->pair[1] - 2; ++i) c_str(" ");
    }
    c_str("("); c_str(n->text); c_strln(")");
    c_str("#line "); c_long(nr + 1);
    c_str(" \""); c_str(arg_output()); c_strln("\"");
    c_strln(" ? parsed : no_parse;");
    //printf("    status = (%s) ? parsed : no_parse;\n", n->text);
    debug_post("guard", n);
    c_close();
}

static void emit_call(struct s_node *n) {
    /* The number of bindings is equal to the number of rule calls -
     * this is assumed XXX where?, so save dummy "binding" if we're not
     * binding.
     */
    if (!associating) associate(0, 0); 
    //printf("_pacc_save_core(%ld, thr_%s, col);\n", n->first->id, binding ? "bound" : "rule");
    //printf("_pacc_save_core(%ld, col);\n", n->first->id);
    c_str("_pacc_save_core("); c_long(n->first->id); c_strln(", col);");
    //printf("_pacc_save_col(col);\n");
    //printf("_pacc_Push(rule_col);\n"); /* XXX this is not callee saves */
    //printf("_pacc_Push(cont);\n");
    //printf("cont = %ld;\n", n->id);
    //printf("st = %ld; /* call %s */\n", n->first->id, n->text);
    //printf("goto top;\n");
    //printf("case %ld: /* return from %s */\n", n->id, n->text);
    //printf("last = cur;\n");
    //printf("_pacc_Pop(cont);\n");
    //printf("status = last->rule & 3;\n");
    //printf("col = last->remainder;\n");
    //printf("_pacc_Pop(rule_col);\n");
    //printf("cur = _pacc_result(_pacc, rule_col, %d);\n", cur_rule);
    c_strln("_pacc_Push(rule_col);"); /* XXX this is not callee saves */
    c_strln("_pacc_Push(cont);");
    c_str("cont = "); c_long(n->id); c_semi();
    c_str("st = "); c_long(n->first->id); c_semi();
    c_str("/* call "); c_str(n->text); c_strln(" */");
    c_strln("goto top;");
    c_str("case "); c_long(n->id); c_str(": /* return from ");
    c_str(n->text); c_strln(" */");
    c_strln("last = cur;");
    c_strln("_pacc_Pop(cont);");
    c_strln("status = last->rule & 3;");
    c_strln("col = last->remainder;");
    c_strln("_pacc_Pop(rule_col);");
    c_str("cur = _pacc_result(_pacc, rule_col, "); c_int(cur_rule->id);
    c_strln(");");
}

static void alt_pre(struct s_node *n) {
    debug_pre("alt", n);
    c_strln("_pacc_Push(cont);");
    c_str("cont = "); c_long(n->id); c_semi();
    savecol();
}

static void alt_mid(struct s_node *n) {
    c_str("Trace fprintf(stderr, \"alt "); c_long(n->id);
    c_strln(" @ col %ld => %s\\n\", col, status!=no_parse?\"yes\":\"no\");");
    c_str("if (status != no_parse)"); c_open();
    accept_col();
    c_strln("goto contin;");
    c_close();
    restcol();
    savecol();
    c_strln("Trace fprintf(stderr, \"col restored to %ld\\n\", col);");
    c_str("Trace fprintf(stderr, \"alt "); c_long(n->id);
    c_strln(" @ col %ld? (next alternative)\\n\", col);");
}

static void alt_post(struct s_node *n) {
    c_str("if (status == no_parse)"); c_open();
    restcol();
    c_close(); c_str("else"); c_open();
    accept_col();
    c_close();
    c_str("case "); c_long(n->id); c_strln(":");
    c_strln("_pacc_Pop(cont);");
    c_str("Trace fprintf(stderr, \"alt "); c_long(n->id);
    c_strln(" @ col %ld => %s\\n\", col, status!=no_parse?\"yes\":\"no\");");
    c_strln("Trace fprintf(stderr, \"col is %ld\\n\", col);");
}

static void cclass_pre(struct s_node *n) {
    debug_pre("cclass", n);
    c_strln("_pacc_utf8_state = UTF8_ACCEPT; _pacc_any_i = 0;");
    c_strln("do "); c_open();
    c_strln("if (_pacc_utf8_state == UTF8_REJECT) panic(\"invalid UTF-8 input\");");
    c_str("if (col + _pacc_any_i == _pacc->input_length)"); c_open();
    c_strln("status = no_parse; break;"); c_close();
    c_close();
    c_strln("while (decode(&_pacc_utf8_state, &_pacc_utf_cp, (unsigned char)_pacc->string[col + _pacc_any_i++]));");
    c_str(" if (");
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

#if 0
static void emit_crange(struct s_node *n) {
    assert(n->text[0] == '>' || n->text[0] == '=' || n->text[0] == '<');
    assert(n->text[0] != '>' || (n->next && n->next->text[0] == '<'));

    c_str("_pacc_utf_cp"); c_char(n->text[0]); c_str("="); c_int(n->text[1]);
    if (n->next) {
	if (n->text[0] == '>') c_str("&&");
	else c_str(" || ");
    }
}
#endif

static void cclass_post(struct s_node *n) {
    if (n->text[0] == '^') c_str(")");
    c_str(")"); c_open();
    c_strln("status = parsed;");
    c_strln("  col += _pacc_any_i;");
    c_close(); c_str("else"); c_open();
    //error(n->text, 1);
    c_str("error(_pacc, \"\\\""); c_str(n->text); c_strln("\\\"\", col);");
    c_strln("status = no_parse;");
    c_close();
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
    printf("extern struct pacc_parser *%s_new(void);\n", yy);
    printf("extern void %s_input(struct pacc_parser *, char *, char *, off_t l);\n", yy);
    printf("extern void %s_destroy(struct pacc_parser *);\n", yy);
    printf("extern int %s_parse(struct pacc_parser *);\n", yy);
    printf("extern %s %s_result(struct pacc_parser *);\n", type, yy);
    printf("extern void %s_error(struct pacc_parser *);\n", yy);
    printf("extern int %s_wrap(const char *, char *, off_t, %s *result);\n",
	yy, type);
}

void header(struct s_node *g) {
    size_t l;
    char *newname;
    char *yy = g->text;
    char *preamble = g->first->text;
    char *type = g->first->next->first->text;
    
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
