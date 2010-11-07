#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "syntax.h"
#include "template.h"

static char *g_name; /* grammar name */
static struct s_node *g_node; /* grammar root */
static int cur_rule;

#define N_STACK_BODGE 30
static char *n_stack[N_STACK_BODGE];
static int n_ptr = 0;
static void pushn(char *n) {
    if (n_ptr == N_STACK_BODGE) {
	fprintf(stderr, "n_stack overflow\n");
	exit(1);
    }
    n_stack[n_ptr++] = n;
}

struct s_node *s_stack[N_STACK_BODGE];
int s_ptr = 0;
static void pushs(struct s_node *s) {
    if (s_ptr == N_STACK_BODGE) {
	fprintf(stderr, "s_stack overflow\n");
	exit(1);
    }
    s_stack[s_ptr++] = s;
}

int binding = 0;

/* Something to think about: should *every* grammar start with a
 * preamble? That would simplify a couple of things, and is how
 * pacc.pacc works at the moment. On the other hand, it's a bit odd to
 * represent no preamble with an empty preamble node. */
static void grammar_pre(struct s_node *n) {
    int i, r = 0;
    struct s_node *p;

    pre_decl();

    g_node = n;

    /* We slightly simplify both building & walking the tree and insist
     * that every grammar starts with a preamble, which may be null. */
    p = n->first;
    assert(p->type == preamble);
    if (p->text) printf("%s\n", p->text);
    p = p->next;

    for ( ; p; p = p->next) {
	assert(p->type == rule);
	++r;
    }
    printf("const int n_rules = %d;\n", r);
    printf("const int start_rule_id = %ld;\n", n->first->next->id);
    g_name = n->text;
    printf("union %s_union {\n", g_name);
    /* XXX we still need u0! */
    printf("    %s u0;\n", n->first->next->first->text);
    for (p = n->first, i = 0; p; p = p->next) {
	if (p->type != rule) continue;
	/* XXX obviously, we need to weed out duplicates, "void", etc. */
	printf("    %s u%ld;\n", p->first->text, p->id);
	++i;
    }
    printf("};\n");

    /* XXX just for debugging */
    printf("#define TYPE_PRINTF ");
    if (strcmp(n->first->next->first->text, "int") == 0) printf("\"%%d\"");
    else if (strcmp(n->first->next->first->text, "char *") == 0) printf("\"%%s\"");
    else printf("\"%%p\"");
    printf("\n");

    printf("#define PACC_TYPE %s\n", n->first->next->first->text);

    printf("/* not yet... %stype %svalue; */\n", g_name, g_name);
 
    pre_engine();

    printf("st = %ld;\ntop:\n",
	    n->first->type == preamble ? n->first->next->id : n->first->id);
    printf("Trace fprintf(stderr, \"switch to state %%d\\n\", st);\n");
    printf("switch(st) {\n");
}

static void grammar_post(struct s_node *n) {
    printf("case -1: break;\n");
    printf("}\n");
    post_engine();
}


/* XXX there are some serious problems here. First, this is way too much
 * code to include time after time; it will have to be made part of the
 * explicit control circuitry (but *that* probably means that we will
 * need more than one label per rule, so we'll have to use "2 * id + 1"
 * type labels.
 */
void error (char *t, int quote) {
    printf("{\n");
    printf("    int doit, append;\n");
    printf("Trace fprintf(stderr, \"error(%s, %d) at col %%ld\\n\", col);\n", t, quote);
    printf("    append = doit = 1;\n");
    printf("    if (col > _pacc_err_col) append = 0;\n");
    printf("    else if (col == _pacc_err_col) {\n");
    printf("        size_t i;\n");
    printf("        for (i = 0; i < _pacc_err_valid; ++i) {\n");
    printf("            if (strcmp(_pacc_err[i], ");
    if (quote) printf("\"\\\"%s\\\"\"", t);
    else printf("\"%s\"", t);
    printf(") == 0) doit = 0;\n");
    printf("        }\n");
    printf("    } else doit = 0;\n");
    printf("    if (doit) {\n");
    printf("        if (append) ++_pacc_err_valid;\n");
    printf("        else _pacc_err_valid = 1;\n");
    printf("        if (_pacc_err_valid > _pacc_err_alloc) {\n");
    printf("            _pacc_err_alloc = 2 * _pacc_err_alloc + 1;\n");
    printf("            _pacc_err = realloc(_pacc_err, _pacc_err_alloc * sizeof(char *));\n");
    printf("            if (!_pacc_err) nomem();\n");
    printf("        }\n");
    printf("        _pacc_err[_pacc_err_valid - 1] = ");
    if (quote) printf("\"\\\"%s\\\"\"", t);
    else printf("\"%s\"", t);
    printf(";\n");
    printf("        _pacc_err_col = col;\n");
    printf("    }\n");
    printf("}\n");
}

static void debug_pre(char *type, struct s_node *n) {
    printf("Trace fprintf(stderr, \"%s %ld @ col %%ld?\\n\", col);\n", type, n->id);
}

static void debug_post(char *type, struct s_node *n) {
    printf("Trace fprintf(stderr, \"%s %ld @ col %%ld => %%s\\n\", col, status != no_parse ? \"yes\" : \"no\");\n",
	    type, n->id);
}

/* We recognise a properly-escaped C string in the grammar, and copy
 * that verbatim into the generated parser. That means we have to be a
 * bit careful in calculating the string's length. */
static void literal(struct s_node *n) {
    char *p;
    int l;

    l = 0;
    for (p = n->text; *p; ++p) {
	if (*p == '\\') {
	    ++p;
	    assert(*p);
	}
	++l;
    }
    debug_pre("lit", n);
    printf("Trace fprintf(stderr, \"lit %ld @ col %%ld => \", col);\n", n->id);
    printf("if (col + %d <= _pacc->input_length &&\n", l);
    printf("        memcmp(\"%s\", _pacc->string + col, %d) == 0) {\n", n->text, l);
    printf("    status = parsed;\n");
    printf("    col += %d;\n", l);
    printf("} else {\n");
    error(n->text, 1);
    printf("    status = no_parse;\n");
    printf("}\n");
    debug_post("lit", n);
}

/* currently assumes 1 char == 1 byte (i.e. ASCII encoding) */
static void any_emit(struct s_node *n) {
    printf("if (col < _pacc->input_length) {\n");
    printf("    status = parsed;\n");
    printf("    ++col;\n");
    printf("} else status = no_parse;\n");
}

static void rule_pre(struct s_node *n) {
    cur_rule = n->id;
    printf("case %ld: /* %s */\n", n->id, n->text); 
    printf("Trace fprintf(stderr, \"rule %ld (%s) col %%ld\\n\", col);\n", n->id, n->text);
    printf("rule_col = col;\n");
    printf("cur = _pacc_result(_pacc, col, %d);\n", cur_rule);
    printf("if ((cur->rule & 3) == uncomputed) {\n");
    //printf("if (1 || (cur->rule & 3) == uncomputed) {\n"); /* memoization OFF */
}

static void rule_post(struct s_node *n) {
    printf("    cur->rule = (cur->rule & ~3) | status;\n");
    printf("    cur->remainder = col;\n");
    printf("    if (_pacc_err_col == rule_col) {\n");
    printf("        _pacc_err_valid = 0;\n"); /* Rule made no progress: over-write error */
    error(n->text, 0);
    printf("    }\n");
    printf("}\n");
    printf("goto contin;\n");
}

static void savecol(void) {
    printf("Trace fprintf(stderr, \"save column registers\\n\");\n");
    printf("_pacc_Push(col); /*_pacc_Push(col_ptr);*/\n");
    printf("_pacc_Push(cur->ev_valid);\n");
}

static void restcol(void) {
    printf("Trace fprintf(stderr, \"restore column registers\\n\");\n");
    printf("_pacc_Pop(cur->ev_valid);\n");
    printf("/*_pacc_Pop(col_ptr);*/ _pacc_Pop(col);\n");
}

static void accept_col(void) {
    printf("Trace fprintf(stderr, \"accept column registers\\n\");\n");
    printf("_pacc_Discard(cur->ev_valid);\n");
    printf("/*_pacc_Discard(col_ptr);*/ _pacc_Discard(col);\n");
}

static void seq_pre(struct s_node *n) {
    printf("Trace fprintf(stderr, \"seq %ld @ col %%ld?\\n\", col);\n", n->id);
    printf("_pacc_Push(cont);\n");
    printf("cont = %ld;\n", n->id);
    printf("status = parsed;\n"); /* empty sequence */
    s_ptr = n_ptr = 0;
}

static void seq_mid(struct s_node *n) {
    printf("if (status == no_parse) {\n");
    printf("    goto contin;\n");
    printf("}\n");
}

static void seq_post(struct s_node *n) {
    printf("case %ld:\n", n->id);
    printf("_pacc_Pop(cont);\n");
    printf("Trace fprintf(stderr, \"seq %ld @ col %%ld => %%s\\n\", rule_col, status!=no_parse?\"yes\":\"no\");\n", n->id);
    printf("Trace fprintf(stderr, \"col is %%ld\\n\", col);\n");
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
    printf("status = (status == no_parse) ? parsed : no_parse;\n");
    restcol();
    debug_post("not", n);
}

static void bind_pre(struct s_node *n) {
    /* A binding may only contain a call... */
    assert(n->first && n->first->type == call);
    /* ... which itself must refer to a rule. */
    assert(n->first->first->type == rule);
    printf("/* bind: %s */\n", n->text);
    debug_pre("bind", n);
    /* Save the name bound, and the rule to which it is bound. */
    pushn(n->text);
    pushs(n->first->first);
    binding = 1;
    printf("Trace fprintf(stderr, \"will bind %s @ rule %ld, col %%ld\\n\", col);\n", n->text, n->first->first->id);
}

static void bind_post(struct s_node *n) {
    binding = 0;
    printf("/* end bind: %s */\n", n->text);
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
	/* We have a list of names (n_stack) which are bound in the
	 * current sequence. XXX Should we not search from the back?
	 */
	for (i = 0; i < n_ptr; ++i)
	    if (n_stack[i] && strcmp(n_stack[i], p->text) == 0) break;
	/* It is not an error if we have a name without a binding: the
	 * parser will pick out names like "printf" from the code. */
	if (i == n_ptr) continue;
	assert(s_stack[i]->type == rule);
	assert(s_stack[i]->first->type == type);
	printf("/* i is %d */\n", i);
	printf("/* type is %s */\n", s_stack[i]->first->text);
	printf("    %s %s;\n", s_stack[i]->first->text, n_stack[i]);
    }
}

static void bindings(struct s_node *n) {
    int i;
    struct s_node *p;

    for (p = n->first; p; p = p->next) {

	printf("/* binding %s */\n", p->text);
#if 0
	for (i = 0; i < n_ptr; ++i)
	    fprintf(stderr, "var %s @ pos %d\n", n_stack[i], i);
#endif
	for (i = 0; i < n_ptr; ++i)
	    if (n_stack[i] && strcmp(n_stack[i], p->text) == 0) break;
	if (i == n_ptr) continue;
	printf("pos = %d;\n", i);
	printf("Trace fprintf(stderr, \"binding of %s: pos %%d holds col %%ld\\n\", pos, _pacc_p->evlis[pos].col);\n", p->text);

	printf("    Trace fprintf(stderr, \"bind %s to r%ld @ c%%ld\\n\", _pacc_p->evlis[pos].col);\n", p->text, s_stack[i]->id);
	printf("    cur = _pacc_result(_pacc, _pacc_p->evlis[pos].col, %ld);\n", s_stack[i]->id);
	printf("    if ((cur->rule & 3) != evaluated) {\n");
	printf("        _pacc_Push(col); _pacc_Push(cont);\n");
	printf("        cont = %ld;\n", p->id);
	printf("	_pacc_ev_i = 0; goto eval_loop;\n");
	printf("case %ld:     _pacc_Pop(cont); _pacc_Pop(col);\n", p->id);
	printf("    }\n");
	printf("    %s = cur->value.u%ld;\n", p->text, s_stack[i]->id);
	printf("    Trace fprintf(stderr, \"bound %s to r%ld @ c%%ld ==> \" TYPE_PRINTF \"\\n\", _pacc_p->evlis[pos].col, cur->value.u0);\n", p->text, s_stack[i]->id);
    }
}

static void emit_expr(struct s_node *n) {
    debug_pre("expr", n);
    /* When we encounter an expression whilst parsing, simply record the
     * expression's id. This will become the new state when we evaluate.
     */
    printf("assert(cur->expr_id == 0);\n");
    printf("cur->expr_id = %ld;\n", n->id);

    /* When evaluating, we need to evaluate the expression! */
    printf("case %ld:\n", n->id);
    printf("if (evaluating) {\n");
    printf("    struct intermed *_pacc_p;\n"); /* parent */
    declarations(n);
    printf("    Trace fprintf(stderr, \"%ld: evaluating\\n\");\n", n->id);
    printf("    _pacc_p = cur = _pacc_result(_pacc, col, %d);\n", cur_rule);
    bindings(n);
    printf("    cur = _pacc_p;\n");
    printf("    cur->value.u%d = (%s);\n", cur_rule, n->text);
    printf("    Trace fprintf(stderr, \"stash \" TYPE_PRINTF \" to (%%ld, %d)\\n\", cur->value.u0, col);\n", cur_rule);
    printf("    goto _pacc_expr_done;\n");
    printf("}\n");
}

static void guard_pre(struct s_node *n) {
    debug_pre("guard", n);
    printf("/* %ld: guard_pre() */\n", n->id);
    printf("{\n    struct intermed *_pacc_p;\n"); /* parent */
    declarations(n);
    printf("    _pacc_p = cur; evaluating = 1;\n");
    bindings(n);
    printf("    cur = _pacc_p; evaluating = 0;\n");
}

/* obviously, the tricky part of a guard is the bindings! */
static void guard_post(struct s_node *n) {
    printf("    status = (%s) ? parsed : no_parse;\n", n->text);
    debug_post("guard", n);
    printf("}\n");
}

static void emit_call(struct s_node *n) {
    if (!binding) {
	pushn(0); pushs(0); /* Save dummy "binding" if we're not binding */
    }
    //printf("_pacc_save_core(%ld, thr_%s, col);\n", n->first->id, binding ? "bound" : "rule");
    printf("_pacc_save_core(%ld, col);\n", n->first->id);
    //printf("_pacc_save_col(col);\n");
    printf("_pacc_Push(rule_col);\n"); /* XXX this is not callee saves */
    printf("_pacc_Push(cont);\n");
    printf("cont = %ld;\n", n->id);
    printf("st = %ld; /* call %s */\n", n->first->id, n->text);
    printf("goto top;\n");
    printf("case %ld: /* return from %s */\n", n->id, n->text);
    printf("last = cur;\n");
    printf("_pacc_Pop(cont);\n");
    printf("status = last->rule & 3;\n");
    printf("col = last->remainder;\n");
    printf("_pacc_Pop(rule_col);\n");
    printf("cur = _pacc_result(_pacc, rule_col, %d);\n", cur_rule);
}

static void alt_pre(struct s_node *n) {
    debug_pre("alt", n);
    printf("_pacc_Push(cont);\n");
    printf("cont = %ld;\n", n->id);
    savecol();
    s_ptr = n_ptr = 0;
}

static void alt_mid(struct s_node *n) {
    printf("Trace fprintf(stderr, \"alt %ld @ col %%ld => %%s\\n\", col, status!=no_parse?\"yes\":\"no\");\n", n->id);
    printf("if (status != no_parse) {\n");
    accept_col();
    printf("goto contin;\n");
    printf("}\n");
    restcol();
    savecol();
    printf("Trace fprintf(stderr, \"col restored to %%ld\\n\", col);\n");
    printf("Trace fprintf(stderr, \"alt %ld @ col %%ld? (next alternative)\\n\", col);\n", n->id);
    s_ptr = n_ptr = 0;
}

static void alt_post(struct s_node *n) {
    printf("if (status == no_parse) {\n");
    restcol();
    printf("} else {\n");
    accept_col();
    printf("}\n");
    printf("case %ld:\n", n->id);
    printf("_pacc_Pop(cont);\n");
    printf("Trace fprintf(stderr, \"alt %ld @ col %%ld => %%s\\n\", col, status!=no_parse?\"yes\":\"no\");\n", n->id);
    printf("Trace fprintf(stderr, \"col is %%ld\\n\", col);\n");
    s_ptr = n_ptr = 0;
}

static void rep_pre(struct s_node *n) {
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
    pre[rep] = rep_pre;

    mid[alt] = alt_mid; mid[seq] = seq_mid;

    post[grammar] = grammar_post; post[rule] = rule_post;
    post[alt] = alt_post; post[seq] = seq_post;
    post[and] = and_post; post[not] = not_post;
    post[bind] = bind_post;
    post[guard] = guard_post;

    node(g);
}
