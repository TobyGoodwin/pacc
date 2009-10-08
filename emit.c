#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "syntax.h"

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


static char *n_stack[25];
static int n_ptr = 0;
static void pushn(char *n) { n_stack[n_ptr++] = n; }

int i_stack[25];
int i_ptr = 0;
static void pushi(int i) { i_stack[i_ptr++] = i; }

int binding = 0;

/* should be temporary - nuke when we have hashing */
static int *lookup_rule;

static void grammar_pre(struct s_node *n) {
    int i, r = 0;
    struct s_node *p;

    g_node = n;
    lookup_rule = malloc(n->id * sizeof(int));
    for (p = n->first; p; p = p->next) {
	lookup_rule[p->id] = r;
	++r;
    }
    printf("#ifndef DECLS\n"); /* phew! what a loony hack: one day we will write the whole of foo.c from here */
    printf("#define DECLS 1\n");
    if (prefix) printf("%s\n", prefix);
    printf("#define n_rules %d\n", r); /* XXX just temporary... soon we will hash */
    g_name = n->text;
    printf("union %s_union {\n", g_name);
    for (p = n->first, i = 0; p; p = p->next, ++i) {
	/* XXX obviously, we need to weed out duplicates, "void", etc. */
	printf("    %s u%d;\n", p->first->text, i);
    }
    printf("};\n");
    printf("#define TYPE_PRINTF "); /* XXX just for debugging */
    if (strcmp(n->first->first->text, "int") == 0) printf("\"%%d\"");
    else if (strcmp(n->first->first->text, "char *") == 0) printf("\"%%s\"");
    else printf("\"%%d\"");
    printf("\n");
    printf("/* not yet... %stype %svalue; */\n", g_name, g_name);
    printf("#else\n");
    printf("st = %d;\ntop:\n", n->first->id);
    printf("printf(\"switch to state %%d\\n\", st);\n");
    printf("switch(st) {\n");
}

static void grammar_post(struct s_node *n) {
    printf("case -1: break;\n");
    printf("}\n");
    printf("#endif\n");
}


static void literal(struct s_node *n) {
    char *p;
    int l;

    l = strlen(n->text);
    printf("printf(\"lit %d @ col %%d => \", col);\n", n->id);
    printf("if (col + %d <= input_length &&\n", l);
    printf("        strncmp(\"");
    for (p = n->text; *p; ++p)
	switch (*p) {
	    case '\"': case '\\':
		printf("\\%c", *p); break;
	    case '\n': printf("\\n"); break;
	    default: printf("%c", *p); break;
	}
    printf("\", string + col, %d) == 0) {\n", l);
    printf("    status = parsed;\n");
    printf("    col += %d;\n", l);
    printf("    printf(\"yes (col=%%d)\\n\", col);\n");
    printf("} else {\n");
    printf("    status = no_parse;\n");
    printf("    printf(\"no (col=%%d)\\n\", col);\n");
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
    printf("printf(\"rule %d (%s) col %%d\\n\", col);\n", cur_rule, n->text);
    printf("rule_col = col;\n");
    printf("cur = matrix + col * n_rules + %d;\n", cur_rule);
    printf("if (cur->status == uncomputed) {\n");
    i_ptr = n_ptr = 0;
}

static void rule_post(struct s_node *n) {
    printf("    cur->status = status;\n");
    printf("    cur->remainder = col;\n");
    printf("}\n");
    printf("goto contin;\n");
    ++cur_rule;
}

static void savecol(void) {
    printf("printf(\"save column registers\\n\");\n");
    printf("pushcont(col); pushcont(col_ptr); pushcont(bcol_ptr); pushcont(cur->thrs_ptr);\n");
}

static void restcol(void) {
    printf("printf(\"restore column registers\\n\");\n");
    printf("cur->thrs_ptr = popcont(); bcol_ptr = popcont(); col_ptr = popcont(); col = popcont();\n");
}

static void accept_col(void) {
    printf("printf(\"accept column registers\\n\");\n");
    printf("popcont(); popcont(); popcont(); popcont();\n");
}

static void seq_pre(struct s_node *n) {
    printf("printf(\"seq %d @ col %%d?\\n\", col);\n", n->id);
    printf("pushcont(cont);\n");
    printf("cont = %d;\n", n->id);
    printf("status = parsed;\n"); /* empty sequence */
}

static void seq_mid(struct s_node *n) {
    printf("if (status == no_parse) {\n");
    printf("    goto contin;\n");
    printf("}\n");
}

static void seq_post(struct s_node *n) {
    printf("case %d:\n", n->id);
    printf("cont = popcont();\n");
    printf("printf(\"seq %d @ col %%d => %%s\\n\", rule_col, status!=no_parse?\"yes\":\"no\");\n", n->id);
    printf("printf(\"col is %%d\\n\", col);\n");
}

static void debug_pre(char *type, struct s_node *n) {
    printf("printf(\"%s %d @ col %%d?\\n\", col);\n", type, n->id);
}

static void debug_post(char *type, struct s_node *n) {
    printf("printf(\"%s %d @ col %%d => %%s\\n\", col, status != no_parse ? \"yes\" : \"no\");\n",
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
    printf("printf(\"%%d: bind_pre()\\n\", %d);\n", n->id);
    pushn(n->text);
    bind_rule = lookup_rule[n->first->first->id];
    pushi(bind_rule);
    binding = 1;
    printf("printf(\"will bind %s @ rule %d, col %%d\\n\", col);\n", n->text, bind_rule);
    //printf("pushbcol(col);\n");
}

static void bind_post(struct s_node *n) {
    binding = 0;
    printf("/* end bind: %s */\n", n->text);
}

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
	printf("printf(\"promise of %s: pos %%d holds col %%d\\n\", pos, cur->thrs[pos].col);\n", p->text);
    }
printf("/* promises() done */\n");
}

static void declarations(struct s_node *n) {
    int i;
    struct s_node *p;

    for (p = n->first; p; p = p->next) {
	struct s_node *q;
	int j;
	for (i = 0; i < n_ptr; ++i)
	    if (strcmp(n_stack[i], p->text) == 0) break;
	if (i == n_ptr) continue;
printf("/* i is %d */\n", i);
	q = g_node->first;
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
	for (i = 0; i < n_ptr; ++i)
	    fprintf(stderr, "var %s @ pos %d\n", n_stack[i], i);
	for (i = 0; i < n_ptr; ++i)
	    if (strcmp(n_stack[i], p->text) == 0) break;
	if (i == n_ptr) continue;
	printf("pushcont(_pacc_i);\n");
	printf("_pacc_i = %d + 1;\n", i);
	printf("for (pos = 0; pos < expr->thrs_ptr; ++pos) {\n");
	printf("    if (expr->thrs[pos].discrim == thr_bound) --_pacc_i;\n");
	printf("    if (_pacc_i == 0) break;\n");
	printf("}\n");
	printf("printf(\"promise of %s: pos %%d holds col %%d\\n\", pos, expr->thrs[pos].col);\n", p->text);
	printf("_pacc_i = popcont();\n");

	printf("    printf(\"bind %s to r%d @ c%%d\\n\", expr->thrs[pos].col);\n", p->text, i_stack[i]);
	printf("    cur = matrix + expr->thrs[pos].col * n_rules + %d;\n",
		i_stack[i]);
	printf("    if (cur->status != evaluated) {\n");
	printf("        pushcol(col); pushcont(cont); cont = %d;\n", p->id);
	printf("	_pacc_i = 0; goto eval_loop;\n");
	printf("case %d:     cont = popcont(); col = popcol();\n", p->id);
	printf("    }\n");
	printf("    %s = cur->value.u0;\n", p->text); /* XXX u0 */
	printf("    printf(\"bound %s to r%d @ c%%d ==> %%d\\n\", expr->thrs[pos].col, cur->value.u0);\n", p->text, i_stack[i]);
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
    printf("printf(\"%%d: emit_expr()\\n\", %d);\n", n->id);
    /* uh, no, we need to push a col for each variable, like decls() and
     * bindings() do */
    printf("pusheval(%d, rule_col, thr_thunk);\n", n->id);
    printf("pusheval(0, col, thr_col);\n");
    //printf("pusheval(col, thr_col);\n");
    //promises(n);
    printf("case %d:\n", n->id);
    printf("if (evaluating) {\n");
    printf("    struct intermed *expr;\n");
    declarations(n);
    printf("    cur = matrix + col * n_rules + %d;\n", cur_rule);
    printf("    expr = cur; pushm(cur); evaluating = 1;\n");
    bindings(n);
    printf("    cur = expr;\n", cur_rule);
    printf("    cur->value.u%d = (%s);\n", cur_rule, n->text);
    printf("    cur->status = evaluated;\n");
    printf("printf(\"stash %%d to (%%d, %d)\\n\", cur->value.u0, col);\n", cur_rule);
    printf("    cur = popm();\n");
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
    int i;
    struct s_node *p;

    printf("printf(\"r%d @ c%%d: guard %d?\\n\", col);\n", cur_rule, n->id);
    printf("/* %d: guard_pre() */\n", n->id);
    printf("{\n    struct intermed *guard;\n");
    declarations(n);
    printf("    guard = cur; pushm(cur); evaluating = 1;\n");
    bindings(n);
    printf("    cur = popm(); evaluating = 0;\n");
}

/* obviously, the tricky part of a guard is the bindings! */
static void guard_post(struct s_node *n) {
    printf("    status = (%s) ? parsed : no_parse;\n", n->text);
    printf("printf(\"r%d @ c%%d: guard %d => %%s\\n\", col, status == parsed ? \"yes\" : \"no\");\n", cur_rule, n->id);
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
    printf("pushcont(cont); pushm(cur);\n");
    printf("cont = %d;\n", n->id);
    printf("st = %d; /* call %s */\n", n->first->id, n->text);
    printf("goto top;\n");
    printf("case %d: /* return from %s */\n", n->id, n->text);
    printf("cont = popcont();\n");
    printf("last = cur; cur = popm();\n");
    printf("status = last->status;\n");
    printf("col = last->remainder;\n");
    printf("rule_col = popcont();\n");
}

static void alt_pre(struct s_node *n) {
    printf("printf(\"alt %d @ col %%d?\\n\", col);\n", n->id);
    printf("pushcont(cont);\n");
    printf("cont = %d;\n", n->id);
    savecol();
    i_ptr = n_ptr = 0;
}

static void alt_mid(struct s_node *n) {
    printf("printf(\"alt %d @ col %%d => %%s\\n\", col, status!=no_parse?\"yes\":\"no\");\n", n->id);
    printf("if (status != no_parse) {\n");
    accept_col();
    printf("goto contin;\n");
    printf("}\n");
    restcol();
    savecol();
    printf("printf(\"col restored to %%d\\n\", col);\n");
    printf("printf(\"alt %d @ col %%d? (next alternative)\\n\", col);\n", n->id);
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
    printf("printf(\"alt %d @ col %%d => %%s\\n\", col, status!=no_parse?\"yes\":\"no\");\n", n->id);
    printf("printf(\"col is %%d\\n\", col);\n");
    i_ptr = n_ptr = 0;
}

/* I rather like this implementation of rep, but according to both Bryan
 * Ford and Robert Grimm treating rep as anything other than sugar can
 * introduce non-linearity into the parser. I've yet to produce a
 * convincing example of this.
 */
static void rep_pre(struct s_node *n) {
    int min, max;

    min = (n->number & 0xffff0000) >> 16;
    max = n->number & 0xffff;
    printf("printf(\"rep %d @ col %%d?\\n\", col);\n", n->id);
    printf("pushcont(_pacc_rep);\n");
    printf("_pacc_rep = 0;\n");
    printf("case %d:\n", n->id);
    savecol();
}

static void rep_post(struct s_node *n) {
    int min, max;

    min = (n->number & 0xffff0000) >> 16;
    max = n->number & 0xffff;
    printf("if (status == no_parse) {\n");
    restcol();
    printf("    if (_pacc_rep >= %d", min);
    if (max)
	printf(" && _pacc_rep <= %d", max);
    printf(") {\n");
    printf("        status = parsed;\n");
    printf("    }\n");
    printf("} else {\n");
    accept_col();
    printf("    ++_pacc_rep;\n");
    if (max) printf("    if (_pacc_rep < %d) {\n", max);
    printf("        st = %d; goto top;\n", n->id);
    if (max) printf("    }\n");
    printf("}\n");
    printf("_pacc_rep = popcont();\n");
    printf("printf(\"rep %d @ col %%d => %%s\\n\", col, status!=no_parse?\"yes\":\"no\");\n", n->id);
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
    post[rep] = rep_post;

    node(g);
}
