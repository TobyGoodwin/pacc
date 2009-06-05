#include <stdio.h>
#include <stdlib.h>

#include "syntax.h"

static char *g_name; /* grammar name */
static int cur_rule = 0;
static int bind_rule;

static char *n_stack[25];
static int n_ptr = 0;
static void pushn(char *n) { n_stack[n_ptr++] = n; }

int i_stack[25];
int i_ptr = 0;
static void pushi(int i) { i_stack[i_ptr++] = i; }

/* should be temporary - nuke when we have hashing */
static int *lookup_rule;

static void grammar_pre(struct s_node *n) {
    int r = 0;
    struct s_node *p;

    lookup_rule = malloc(n->id * sizeof(int));
    for (p = n->first; p; p = p->next) {
	lookup_rule[p->id] = r;
	++r;
    }
    printf("#define n_rules %d\n", r); /* XXX just temporary... soon we will hash */
    g_name = n->text;
    printf("typedef union { int %stype0; } %stype;\n", g_name, g_name);
    printf("%stype %svalue;\n", g_name, g_name);
    printf("st = %d;\ntop:\n", n->first->id);
    printf("printf(\"switch to state %%d\\n\", st);\n");
    printf("switch(st) {\n");
}

static void grammar_post(struct s_node *n) {
    printf("case -1: break;\n");
    printf("}\n");
}

/* literal() currently matches just a single character */
static void literal(struct s_node *n) {
    printf("printf(\"%%c == %c? \", string[col]);\n", n->text[0]);
    printf("if (string[col] == '%c') {\n", n->text[0]);
    printf("    status = parsed;\n");
    printf("    ++col;\n");
    printf("    printf(\"yes (col=%%d)\\n\", col);\n");
    printf("} else {\n");
    printf("    status = no_parse;\n");
    printf("    printf(\"no (col=%%d)\\n\", col);\n");
    printf("}\n");
}

static void rule_pre(struct s_node *n) {
    printf("case %d: /* %s */\n", n->id, n->text); 
    printf("printf(\"rule %d (%s) col %%d\\n\", col);\n", cur_rule, n->text);
    printf("rule_col = col;\n");
    printf("cur = matrix + col * n_rules + %d;\n", cur_rule);
    printf("if (cur->status == uncomputed) {\n");
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
    printf("pushcont(col); pushcont(col_ptr); pushcont(cur->thrs_ptr);\n");
}

static void restcol(void) {
    printf("printf(\"restore column registers\\n\");\n");
    printf("cur->thrs_ptr = popcont(); col_ptr = popcont(); col = popcont();\n");
}

static void accept_col(void) {
    printf("printf(\"accept column registers\\n\");\n");
    printf("popcont(); popcont(); popcont();\n");
}

static void seq_pre(struct s_node *n) {
    printf("printf(\"seq %d @ col %%d?\\n\", col);\n", n->id);
    printf("pushcont(cont);\n");
    printf("cont = %d;\n", n->id);
}

static void seq_mid(struct s_node *n) {
    printf("if (status != parsed) {\n");
    printf("    goto contin;\n");
    printf("}\n");
}

static void seq_post(struct s_node *n) {
    printf("case %d:\n", n->id);
    printf("cont = popcont();\n");
    printf("printf(\"seq %d @ col %%d => %%s\\n\", col, status==parsed?\"yes\":\"no\");\n", n->id);
    printf("printf(\"col is %%d\\n\", col);\n");
}

/* A binding may only contain a call. */
static void bind_pre(struct s_node *n) {
    printf("/* bind: %s */\n", n->text);
    printf("printf(\"%%d: bind_pre()\\n\", %d);\n", n->id);
    pushn(n->text);
    bind_rule = lookup_rule[n->first->first->id];
    pushi(bind_rule);
    printf("/* bind_rule is %d */\n", bind_rule);
    printf("printf(\"binding %s at col %%d\\n\", col);\n", n->text);
    printf("pushcol(col);\n");
}

static void bind_post(struct s_node *n) {
    printf("/* end bind: %s */\n", n->text);
}

static void emit_expr(struct s_node *n) {
    int i;

    printf("printf(\"%%d: expr_pre()\\n\", %d);\n", n->id);
    printf("pushthunk(%d); pushthunk(rule_col);\n", n->id);
    for (i = 0; i < n_ptr; ++i)
	printf("pushthunk(popcol());\n");
    printf("case %d:\n", n->id);
    printf("if (evaluating) {\n");
    if (n_ptr) printf("    int mycol;\n");
    for (i = 0; i < n_ptr; ++i)
	printf("    int %s;\n", n_stack[i]);
    printf("    cur = matrix + col * n_rules + %d;\n", cur_rule);
    //printf("    cur->thrs_ptr = 0;\n");
    for (i = n_ptr - 1; i >= 0; --i) {
	printf("    mycol = cur->thrs[i++].x.thunk;\n");
	printf("    %s = matrix[mycol * n_rules + %d].value;\n", n_stack[i], i_stack[i]);
	printf("printf(\"assign %%d from (%%d, %%d) to %s\\n\", %s, mycol, %d);", n_stack[i], n_stack[i], i_stack[i]);
    }
    //printf("    %svalue.%stype%d = %s;\n", g_name, g_name, n->e_type, n->text);
    printf("    cur->value = %s;\n", n->text);
    printf("printf(\"stash %%d to (%%d, %d)\\n\", cur->value, col);\n", cur_rule);
    //printf("    st = cur->thrs[cur->thrs_ptr++].x.thunk;\n");
    //printf("    col = cur->thrs[cur->thrs_ptr++].x.thunk;\n");
    printf("    goto eval_loop;\n");
    printf("}\n");
}

static void emit_call(struct s_node *n) {
    printf("pushrule(%d, col);\n", lookup_rule[n->first->id]);
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
    printf("printf(\"alt %d @ col %%d => %%s\\n\", col, status==parsed?\"yes\":\"no\");\n", n->id);
    printf("if (status == parsed) {\n");
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
    printf("if (status != parsed) {\n");
    restcol();
    printf("} else {\n");
    accept_col();
    printf("}\n");
    printf("case %d:\n", n->id);
    printf("cont = popcont();\n");
    printf("printf(\"alt %d @ col %%d => %%s\\n\", col, status==parsed?\"yes\":\"no\");\n", n->id);
    printf("printf(\"col is %%d\\n\", col);\n");
    i_ptr = n_ptr = 0;
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
    pre[bind] = bind_pre; pre[expr] = emit_expr;
    pre[call] = emit_call; pre[lit] = literal;

    mid[alt] = alt_mid; mid[seq] = seq_mid;

    post[grammar] = grammar_post; post[rule] = rule_post;
    post[alt] = alt_post; post[seq] = seq_post;
    post[bind] = bind_post;

    node(g);
}
