#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "syntax.h"
#include "template.h"

static char *g_name; /* grammar name */
static struct s_node *g_node; /* grammar root */
/* Rule numbers can be confusing. At the moment, each rule has both an
 * id (from the tree), and a number. The numbers start at 0 for the
 * start rule, and these numbers are used to index the array of
 * intermediate results.
 *
 * In the future, we will instead store intermed results in a hash
 * table, at which point we can - I think - drop the numbering, and just
 * hash on the rule id. The array lookup_rule maps from ids to numbers.
 */
static int cur_rule = 0; static struct s_node *cur_rule_node;

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

int i_stack[N_STACK_BODGE];
int i_ptr = 0;
static void pushi(int i) {
    if (i_ptr == N_STACK_BODGE) {
	fprintf(stderr, "i_stack overflow\n");
	exit(1);
    }
    i_stack[i_ptr++] = i;
}

int binding = 0;

/* should be temporary - nuke when we have hashing */
static int *lookup_rule;

/* Something to think about: should *every* grammar start with a
 * preamble? That would simplify a couple of things, and is how
 * pacc.pacc works at the moment. On the other hand, it's a bit odd to
 * represent no preamble with an empty preamble node. */
static void grammar_pre(struct s_node *n) {
    int i, r = 0;
    struct s_node *p;

    pre_decl();

    g_node = n;

    /* XXX old style: separate "prefix" */
    //if (prefix) printf("%s\n", prefix);
    /* XXX new style: preamble is first child of g */
    p = n->first;
    assert(p->type == preamble);
    if (p->text) printf("%s\n", p->text);
    p = p->next;

    /* Find the maximal rule id, and set up lookup_rule[]. */
    i = 0;
    for ( ; p; p = p->next)
	if (p->id > i) i = p->id;
    lookup_rule = malloc((i + 1) * sizeof(int));
    for (p = n->first->next; p; p = p->next)
	lookup_rule[p->id] = r++;

    printf("#define n_rules %d\n", r); /* XXX just temporary... soon we will hash */
    g_name = n->text;
    printf("union %s_union {\n", g_name);
    for (p = n->first, i = 0; p; p = p->next) {
	if (p->type != rule) continue;
	/* XXX obviously, we need to weed out duplicates, "void", etc. */
	printf("    %s u%d;\n", p->first->text, i);
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

    printf("st = %d;\ntop:\n",
	    n->first->type == preamble ? n->first->next->id : n->first->id);
    printf("Trace fprintf(stderr, \"switch to state %%d\\n\", st);\n");
    printf("switch(st) {\n");
}

static void grammar_post(struct s_node *n) {
    printf("case -1: break;\n");
    printf("}\n");
    post_engine();
}


static void preamble_emit(struct s_node *n) {
    printf("%s\n", n->text);
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
    printf("Trace fprintf(stderr, \"error(%s, %d) at col %%d\\n\", col);\n", t, quote);
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
#if 0
void error (char *t, int quote) {
    printf("{\n");
    printf("    struct _pacc_error *e, *f;\n");
    printf("    int gotit;\n");
    printf("fprintf(stderr, \"error(%s, %d) at col %%d\\n\", col);\n", t, quote);
    printf("    e = f = 0; gotit = 1;\n");
    printf("    if (!_pacc_err) f = 0;\n");
    printf("    else if (col > _pacc_err_col) {\n");
    printf("        e = _pacc_err;\n");
    printf("    } else if (col == _pacc_err_col) {\n");
    printf("        for (e = _pacc_err; e; e = e->next) {\n");
    printf("            f = e;\n");
    printf("            if (strcmp(e->x, ");
    if (quote) printf("\"\\\"%s\\\"\"", t);
    else printf("\"%s\"", t);
    printf(") == 0) gotit = 0;\n");
    printf("        }\n");
    printf("    } else gotit = 0;\n");
    printf("    if (gotit) {\n");
    printf("        if (!e) {\n");
    printf("            e = realloc(0, sizeof *e);\n");
    printf("            if (!e) nomem();\n");
    printf("        }\n");
    printf("        e->x = ");
    if (quote) printf("\"\\\"%s\\\"\"", t);
    else printf("\"%s\"", t);
    printf(";\n");
    printf("        _pacc_err_col = col;\n");
    printf("        if (f) f->next = e;\n");
    printf("        else _pacc_err = e;\n"); /* XXX memory leak! */
    printf("    }\n");
    printf("}\n");
}
#endif

/* XXX this has vacillated, but in the end we want to recognise a
 * properly-escaped C string in the grammar, and copy that verbatim into
 * the generated parser. No? That means we have to be a bit careful in
 * calculating the string's length.
 */
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
    printf("Trace fprintf(stderr, \"lit %d @ col %%d => \", col);\n", n->id);
    printf("if (col + %d <= input_length &&\n", l);
#if 0
    printf("        strncmp(\"");
    for (p = n->text; *p; ++p)
	switch (*p) {
	    case '\"': case '\\':
		printf("\\%c", *p); break;
	    case '\n': printf("\\n"); break;
	    default: printf("%c", *p); break;
	}
    printf("\", string + col, %d) == 0) {\n", l);
#endif
    printf("        strncmp(\"%s\", string + col, %d) == 0) {\n", n->text, l);
    printf("    status = parsed;\n");
    printf("    col += %d;\n", l);
    printf("    Trace fprintf(stderr, \"yes (col=%%d)\\n\", col);\n");
    printf("} else {\n");
    error(n->text, 1);
    printf("    status = no_parse;\n");
    printf("    Trace fprintf(stderr, \"no (col=%%d)\\n\", col);\n");
    printf("}\n");
}

/* currently assumes 1 char == 1 byte (i.e. ASCII encoding) */
static void any_emit(struct s_node *n) {
    printf("if (col < input_length) {\n");
    printf("    status = parsed;\n");
    printf("    ++col;\n");
    printf("} else status = no_parse;\n");
}

static void rule_pre(struct s_node *n) {
    cur_rule_node = n;
    printf("case %d: /* %s */\n", n->id, n->text); 
    printf("Trace fprintf(stderr, \"rule %d (%s) col %%d\\n\", col);\n", cur_rule, n->text);
    printf("rule_col = col;\n");
    printf("cur = _pacc_result(_pacc, col, %d);\n", cur_rule);
    printf("if (cur->status == uncomputed) {\n");
}

static void rule_post(struct s_node *n) {
    printf("    cur->status = status;\n");
    printf("    cur->remainder = col;\n");
    printf("    if (_pacc_err_col == rule_col) {\n");
    printf("        _pacc_err_valid = 0;\n"); /* Rule made no progress: over-write error */
    error(n->text, 0);
    printf("    }\n");
    printf("}\n");
    printf("goto contin;\n");
    ++cur_rule;
}

static void savecol(void) {
    printf("Trace fprintf(stderr, \"save column registers\\n\");\n");
    printf("pushcont(col); pushcont(col_ptr); pushcont(cur->thrs_ptr);\n");
}

static void restcol(void) {
    printf("Trace fprintf(stderr, \"restore column registers\\n\");\n");
    printf("cur->thrs_ptr = popcont(); col_ptr = popcont(); col = popcont();\n");
}

static void accept_col(void) {
    printf("Trace fprintf(stderr, \"accept column registers\\n\");\n");
    printf("popcont(); popcont(); popcont();\n");
}

static void seq_pre(struct s_node *n) {
    printf("Trace fprintf(stderr, \"seq %d @ col %%d?\\n\", col);\n", n->id);
    printf("pushcont(cont);\n");
    printf("cont = %d;\n", n->id);
    printf("status = parsed;\n"); /* empty sequence */
    i_ptr = n_ptr = 0;
}

static void seq_mid(struct s_node *n) {
    printf("if (status == no_parse) {\n");
    printf("    goto contin;\n");
    printf("}\n");
}

static void seq_post(struct s_node *n) {
    printf("case %d:\n", n->id);
    printf("cont = popcont();\n");
    printf("Trace fprintf(stderr, \"seq %d @ col %%d => %%s\\n\", rule_col, status!=no_parse?\"yes\":\"no\");\n", n->id);
    printf("Trace fprintf(stderr, \"col is %%d\\n\", col);\n");
}

static void debug_pre(char *type, struct s_node *n) {
    printf("Trace fprintf(stderr, \"%s %d @ col %%d?\\n\", col);\n", type, n->id);
}

static void debug_post(char *type, struct s_node *n) {
    printf("Trace fprintf(stderr, \"%s %d @ col %%d => %%s\\n\", col, status != no_parse ? \"yes\" : \"no\");\n",
	    type, n->id);
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

/* A binding may only contain a call. */
static void bind_pre(struct s_node *n) {
    int bind_rule;

    printf("/* bind: %s */\n", n->text);
    printf("Trace fprintf(stderr, \"%%d: bind_pre()\\n\", %d);\n", n->id);
    pushn(n->text);
    bind_rule = lookup_rule[n->first->first->id];
    pushi(bind_rule);
    binding = 1;
    printf("Trace fprintf(stderr, \"will bind %s @ rule %d, col %%d\\n\", col);\n", n->text, bind_rule);
}

static void bind_post(struct s_node *n) {
    binding = 0;
    printf("/* end bind: %s */\n", n->text);
}

#if 0
static void promises(struct s_node *n) {
    int i, j;
    struct s_node *p;

printf("/* promises() */\n");
    for (p = n->first; p; p = p->next) {
	struct s_node *q;
	int j;
printf("/* var is %s */\n", p->text);
	for (i = 0; i < n_ptr; ++i)
	    fprintf(stderr, "var %s @ pos %d\n", n_stack[i], i);
	for (i = 0; i < n_ptr; ++i)
	    if (strcmp(n_stack[i], p->text) == 0) break;
	if (i == n_ptr) continue;
	printf("_pacc_i = %d + 1;\n", i);
	printf("for (pos = 0; pos < cur->thrs_ptr; ++pos) {\n");
	printf("    if (cur->thrs[pos].discrim == thr_bound) --_pacc_i;\n");
	printf("    if (_pacc_i == 0) break;\n");
	printf("}\n");
	printf("printf(\"binding of %s: pos %%d holds col %%d\\n\", pos, cur->thrs[pos].col);\n", p->text);
    }
printf("/* promises() done */\n");
}
#endif

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
	struct s_node *q;
	int j;
	for (i = 0; i < n_ptr; ++i)
	    if (strcmp(n_stack[i], p->text) == 0) break;
	if (i == n_ptr) continue;
printf("/* i is %d */\n", i);
	q = g_node->first;
	if (q->type == preamble) q = q->next;
	for (j = 0; j < i_stack[i]; ++j)
	    q = q->next;
    printf("/* rule id is %d */\n", p->id);
    printf("/* type is %s */\n", q->first->text);
	printf("    %s %s;\n", q->first->text, n_stack[i]);
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
	    if (strcmp(n_stack[i], p->text) == 0) break;
	if (i == n_ptr) continue;
	printf("pushcont(_pacc_i);\n");
	printf("_pacc_i = %d + 1;\n", i);
	printf("for (pos = 0; pos < _pacc_p->thrs_ptr; ++pos) {\n");
	printf("    if (_pacc_p->thrs[pos].discrim == thr_bound) --_pacc_i;\n");
	printf("    if (_pacc_i == 0) break;\n");
	printf("}\n");
	printf("Trace fprintf(stderr, \"binding of %s: pos %%d holds col %%d\\n\", pos, _pacc_p->thrs[pos].col);\n", p->text);
	printf("_pacc_i = popcont();\n");

	printf("    Trace fprintf(stderr, \"bind %s to r%d @ c%%d\\n\", _pacc_p->thrs[pos].col);\n", p->text, i_stack[i]);
	printf("    cur = _pacc_result(_pacc, _pacc_p->thrs[pos].col, %d);\n", i_stack[i]);
	printf("    if (cur->status != evaluated) {\n");
	printf("        pushcol(col); pushcont(cont); cont = %d;\n", p->id);
	printf("	_pacc_i = 0; goto eval_loop;\n");
	printf("case %d:     cont = popcont(); col = popcol();\n", p->id);
	printf("    }\n");
	printf("    %s = cur->value.u%d;\n", p->text, i_stack[i]);
	printf("    Trace fprintf(stderr, \"bound %s to r%d @ c%%d ==> \" TYPE_PRINTF \"\\n\", _pacc_p->thrs[pos].col, cur->value.u0);\n", p->text, i_stack[i]);
#if 0
	for (i = 0; i < n_ptr; ++i)
	    if (strcmp(n_stack[i], p->text) == 0) break;
	if (i == n_ptr) continue;
	printf("    printf(\"bind %s to r%d @ c%%d\\n\", guard->thrs[guard->thrs_ptr - %d].x);\n", p->text, i_stack[i], 2 * n_ptr - 2 * i - 1);
	printf("    cur = matrix + guard->thrs[guard->thrs_ptr - %d].x * n_rules + %d;\n", 2 * n_ptr - 2 * i - 1, i_stack[i]);
	printf("    if (cur->status != evaluated) {\n");
	printf("        pushcol(col); pushcont(cont); cont = %d;\n", p->id);
	printf("	_pacc_i = 0; goto eval_loop;\n");
	printf("case %d:     cont = popcont(); col = popcol();\n", p->id);
	printf("    }\n");
	printf("    %s = cur->value.u0;\n", p->text); /* XXX u0 */
	printf("printf(\"bound %s to r%d @ c%%d ==> %%d\\n\", guard->thrs[guard->thrs_ptr - %d].x, cur->value.u0);\n", p->text, i_stack[i], 2 * n_ptr - 2 * i - 1);
#endif

    }
}

static void emit_expr(struct s_node *n) {
    printf("Trace fprintf(stderr, \"%d: emit_expr()\\n\");\n", n->id);
    /* uh, no, we need to push a col for each variable, like decls() and
     * bindings() do */
    printf("pusheval(%d, rule_col, thr_thunk);\n", n->id);
    printf("pusheval(0, col, thr_col);\n");
    //printf("pusheval(col, thr_col);\n");
    //promises(n);
    printf("case %d:\n", n->id);
    printf("if (evaluating) {\n");
    printf("    struct intermed *_pacc_p;\n"); /* parent */
    declarations(n);
    printf("    Trace fprintf(stderr, \"%d: evaluating\\n\");\n", n->id);
    printf("    _pacc_p = cur = _pacc_result(_pacc, col, %d);\n", cur_rule);
    printf("    evaluating = 1;\n");
    bindings(n);
    printf("    cur = _pacc_p;\n");
    printf("    cur->value.u%d = (%s);\n", cur_rule, n->text);
    printf("    Trace fprintf(stderr, \"stash \" TYPE_PRINTF \" to (%%d, %d)\\n\", cur->value.u0, col);\n", cur_rule);
    printf("    goto eval_loop;\n");
    printf("}\n");
}

#if 0
static void emit_expr(struct s_node *n) {
    int i;

    printf("printf(\"%%d: expr_pre()\\n\", %d);\n", n->id);
    printf("pusheval(%d, thr_thunk); pusheval(rule_col, thr_col);\n", n->id);
    printf("pusheval(col, thr_col);\n");
    for (i = 0; i < n_ptr; ++i)
	printf("pusheval(popcol(), thr_col);\n");
    printf("case %d:\n", n->id);
    printf("if (evaluating) {\n");
    if (n_ptr) printf("    int mycol;\n");
    for (i = 0; i < n_ptr; ++i) {
	static struct s_node *p;
	int j;
    printf("/* rule number is %d */\n", i_stack[i]);
	p = g_node->first;
	for (j = 0; j < i_stack[i]; ++j)
	    p = p->next;
    printf("/* rule id is %d */\n", p->id);
    printf("/* type is %s */\n", p->first->text);
	printf("    %s %s;\n", p->first->text, n_stack[i]);
    }
    printf("    cur = matrix + col * n_rules + %d;\n", cur_rule);
    for (i = n_ptr - 1; i >= 0; --i) {
	static struct s_node *p;
	int j;
    printf("/* rule number is %d */\n", i_stack[i]);
	p = g_node->first;
	for (j = 0; j < i_stack[i]; ++j)
	    p = p->next;
    printf("/* rule id is %d */\n", p->id);
    printf("/* type is %s */\n", p->first->text);
	printf("    mycol = cur->thrs[_pacc_i++].x;\n");
	printf("    %s = matrix[mycol * n_rules + %d].value.u%d;\n", n_stack[i], i_stack[i], i_stack[i]);
	printf("printf(\"assign %%p from (%%d, %%d) to %s\\n\", %s, mycol, %d);\n", n_stack[i], n_stack[i], i_stack[i]);
    }
    //printf("    %svalue.%stype%d = %s;\n", g_name, g_name, n->e_type, n->text);
    printf("/* rule number is %d */\n", cur_rule);
    printf("/* rule id is %d */\n", cur_rule_node->id);
    printf("/* type is %s */\n", cur_rule_node->first->text);
    printf("    cur->value.u%d = (%s);\n", cur_rule, n->text);
    printf("    cur->status = evaluated;\n");
    //printf("printf(\"stash \" TYPE_PRINTF \" to (%%d, %d)\\n\", cur->value.u0, col);\n", cur_rule);
    printf("printf(\"stash %%p to (%%d, %d)\\n\", cur->value.u0, col);\n", cur_rule);
    printf("    goto eval_loop;\n");
    printf("}\n");
}
#endif

static void guard_pre(struct s_node *n) {
    printf("Trace fprintf(stderr, \"r%d @ c%%d: guard %d?\\n\", col);\n", cur_rule, n->id);
    printf("/* %d: guard_pre() */\n", n->id);
    printf("{\n    struct intermed *_pacc_p;\n"); /* parent */
    declarations(n);
    printf("    _pacc_p = cur; evaluating = 1;\n");
    bindings(n);
    printf("    cur = _pacc_p; evaluating = 0;\n");
}

/* obviously, the tricky part of a guard is the bindings! */
static void guard_post(struct s_node *n) {
    printf("    status = (%s) ? parsed : no_parse;\n", n->text);
    printf("Trace fprintf(stderr, \"r%d @ c%%d: guard %d => %%s\\n\", col, status == parsed ? \"yes\" : \"no\");\n", cur_rule, n->id);
    printf("}\n");
}

static void emit_call(struct s_node *n) {
    printf("pusheval(%d, col, thr_%s);\n", lookup_rule[n->first->id],
	    binding ? "bound" : "rule");
    //if (binding)
//	printf("pusheval(%d, thr_bound);\n", lookup_rule[n->first->id]);
 //   else
//	printf("pusheval(%d, thr_rule);\n", lookup_rule[n->first->id]);
 //   printf("pusheval(col, thr_col);\n");
    printf("pushcont(rule_col);\n"); /* XXX this is not callee saves */
    printf("pushcont(cont); pushcol(col);\n");
    printf("cont = %d;\n", n->id);
    printf("st = %d; /* call %s */\n", n->first->id, n->text);
    printf("goto top;\n");
    printf("case %d: /* return from %s */\n", n->id, n->text);
    printf("cont = popcont();\n");
    printf("last = cur; cur = _pacc_result(_pacc, popcol(), %d);\n", cur_rule);
    printf("status = last->status;\n");
    printf("col = last->remainder;\n");
    printf("rule_col = popcont();\n");
}

static void alt_pre(struct s_node *n) {
    printf("Trace fprintf(stderr, \"alt %d @ col %%d?\\n\", col);\n", n->id);
    printf("pushcont(cont);\n");
    printf("cont = %d;\n", n->id);
    savecol();
    i_ptr = n_ptr = 0;
}

static void alt_mid(struct s_node *n) {
    printf("Trace fprintf(stderr, \"alt %d @ col %%d => %%s\\n\", col, status!=no_parse?\"yes\":\"no\");\n", n->id);
    printf("if (status != no_parse) {\n");
    accept_col();
    printf("goto contin;\n");
    printf("}\n");
    restcol();
    savecol();
    printf("Trace fprintf(stderr, \"col restored to %%d\\n\", col);\n");
    printf("Trace fprintf(stderr, \"alt %d @ col %%d? (next alternative)\\n\", col);\n", n->id);
    i_ptr = n_ptr = 0;
}

static void alt_post(struct s_node *n) {
    printf("if (status == no_parse) {\n");
    restcol();
    printf("} else {\n");
    accept_col();
    printf("}\n");
    printf("case %d:\n", n->id);
    printf("cont = popcont();\n");
    printf("Trace fprintf(stderr, \"alt %d @ col %%d => %%s\\n\", col, status!=no_parse?\"yes\":\"no\");\n", n->id);
    printf("Trace fprintf(stderr, \"col is %%d\\n\", col);\n");
    i_ptr = n_ptr = 0;
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
    pre[guard] = guard_pre; //pre[ident] = ident_emit;
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
