#include <stdio.h>

#include "syntax.h"

static char *g_name; /* grammar name */
static char *bind_name;

static void grammar_pre(struct s_node *n) {
    int r = 0;
    struct s_node *p;

    for (p = n->first; p; p = p->next)
	++r;
    printf("#define n_rules %d\n", r);
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
    static int count = 0;
    printf("case %d: /* %s */\n", n->id, n->text); 
    printf("printf(\"rule %d (%s) col %%d\\n\", col);\n", count, n->text);
    printf("cur = matrix + col * n_rules + %d;\n", count++);
    printf("if (cur->status == uncomputed) {\n");
}

static void rule_post(struct s_node *n) {
    printf("    cur->status = status;\n");
    printf("    cur->remainder = col;\n");
    printf("}\n");
    printf("goto contin;\n");
}

static void seq_pre(struct s_node *n) {
    printf("printf(\"seq %d @ col %%d?\\n\", col);\n", n->id);
    printf("pushcont(cont);\n");
    printf("cont = %d;\n", n->id);
    printf("pushcont(col);\n");
}

static void seq_mid(struct s_node *n) {
    printf("if (status != parsed) {\n");
    printf("    col = popcont();\n");
    printf("    goto contin;\n");
    printf("}\n");
}

static void seq_post(struct s_node *n) {
    printf("if (status != parsed) col = popcont();\n");
    printf("else popcont();\n");
    printf("case %d:\n", n->id);
    printf("cont = popcont();\n");
    printf("printf(\"seq %d @ col %%d => %%s\\n\", col, status==parsed?\"yes\":\"no\");\n", n->id);
    printf("printf(\"col is %%d\\n\", col);\n");
}

static void bind_pre(struct s_node *n) {
    printf("/* bind: %s */\n", n->text);
    bind_name = n->text;
}

static void bind_post(struct s_node *n) {
    printf("/* end bind: %s */\n", n->text);
}

static void expr_pre(struct s_node *n) {
    printf("pushthunk(%d, cur);\n", n->id);
}

static void expr_post(struct s_node *n) {
    printf("case %d:\n", n->id);
    printf("if (evaluating) {\n");
    printf("    int %s = bind_val->value;\n", bind_name);
    //printf("    %svalue.%stype%d = %s;\n", g_name, g_name, n->e_type, n->text);
    printf("    cur->value = %s;\n", n->text);
    printf("    st = th_stack[th_ptr].t;\n");
    printf("    cur = th_stack[th_ptr++].c;\n");
    printf("    goto top;\n");
    printf("}\n");
}

static void emit_call(struct s_node *n) {
    printf("pushcont(cont); pushm(cur);\n");
    printf("cont = %d;\n", n->id);
    printf("st = %d; /* call %s */\n", n->first->id, n->text);
    printf("goto top;\n");
    printf("case %d: /* return from %s */\n", n->id, n->text);
    printf("cont = popcont();\n");
    printf("last = cur; cur = popm();\n");
    printf("status = last->status;\n");
    printf("col = last->remainder;\n");
    printf("bind_val = last;\n");
}

static void node(struct s_node *);

static void alt_pre(struct s_node *n) {
    printf("printf(\"alt %d @ col %%d?\\n\", col);\n", n->id);
    printf("pushcont(cont);\n");
    printf("cont = %d;\n", n->id);
    printf("pushcont(col);\n");
}

static void alt_mid(struct s_node *n) {
    printf("printf(\"alt %d @ col %%d => %%s\\n\", col, status==parsed?\"yes\":\"no\");\n", n->id);
    printf("if (status == parsed) { popcont(); goto contin; }\n");
    printf("col = popcont(); pushcont(col);\n");
    printf("printf(\"col restored to %%d\\n\", col);\n");
    printf("printf(\"alt %d @ col %%d? (next alternative)\\n\", col);\n", n->id);
}

static void alt_post(struct s_node *n) {
    printf("if (status != parsed) col = popcont();\n");
    printf("else popcont();\n");
    printf("case %d:\n", n->id);
    printf("cont = popcont();\n");
    printf("printf(\"alt %d @ col %%d => %%s\\n\", col, status==parsed?\"yes\":\"no\");\n", n->id);
    printf("printf(\"col is %%d\\n\", col);\n");
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
    pre[bind] = bind_pre; pre[expr] = expr_pre;
    pre[call] = emit_call; pre[lit] = literal;

    mid[alt] = alt_mid; mid[seq] = seq_mid;

    post[grammar] = grammar_post; post[rule] = rule_post;
    post[alt] = alt_post; post[seq] = seq_post;
    post[bind] = bind_post; post[expr] = expr_post;

    node(g);
}
