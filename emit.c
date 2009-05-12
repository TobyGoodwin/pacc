#include <stdio.h>

#include "syntax.h"

static void grammar_pre(struct s_node *n) {
    int r = 0;
    struct s_node *p;

    for (p = n->first; p; p = p->next)
	++r;
    printf("#define n_rules %d\n", r);
    printf("st = %d;\ntop:\n", n->first->id);
    printf("printf(\"switch to state %%d\\n\", st);\n");
    printf("switch(st) {\n");
}

static void grammar_post(struct s_node *n) {
    printf("case -1: break;\n");
    printf("}\n");
}

#if 0
static void literal(char *t) {
    printf("cur->status = no_parse;\n");
    printf("cur->remainder = col;\n");
    printf("printf(\"%%c == %c? \", string[col]);\n", t[0]);
    printf("if (! (string[col] == '%c')) printf(\"no (col=%%d)\\n\", col);\n", t[0]);
    printf("if (! (string[col] == '%c')) goto contin;\n", t[0]);
    printf("printf(\"yes (col=%%d)\\n\", col);");
    printf("cur->remainder = col + 1;\n");
    printf("cur->status = parsed;\n");
}
#endif

/* literal() currently matches just a single character */
static void literal(char *t) {
    printf("printf(\"%%c == %c? \", string[col]);\n", t[0]);
    printf("if (string[col] == '%c') {\n", t[0]);
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

    
static void emit_call(struct s_node *n) {
    printf("cur->status = no_parse;\n");
    printf("pushcont(cont); pushm(cur);\n");
    printf("cont = %d;\n", 1000 + n->id); /* XXX */
    printf("st = %d;\n", n->first->id);
    printf("goto top;\n");
    printf("case %d: /* subcall: %s */\n", 1000 + n->id, n->text); /* XXX !!! */
    printf("cont = popcont();\n");
    printf("last = cur; cur = popm();\n");
    //printf("printf(\"%%s\\n\", last->status == parsed ? \"parsed\" : \"not parsed\");\n");
    printf("if (last->status != parsed) { col = last->remainder; goto contin; }\n");
    printf("col = cur->remainder = last->remainder;\n");
    printf("cur->value = last->value;\n");
    printf("cur->status = parsed;\n");
}

static void node(struct s_node *);

#if 0
static void alternative(struct s_node *n) {
    struct s_node *p;

    for (p = n->first; p; p = p->next) {
	printf("printf(\"alternative...\");\n");
	printf("pushcont(col);\n");
	printf("cur->status = no_parse;\n");
	printf("pushcont(cont); pushm(cur);\n");
	printf("cont = %d;\n", p->id);
	node(p);
	printf("case %d:\n", p->id);
	printf("cont = popcont();\n");
	printf("last = cur; cur = popm();\n");
	printf("printf(last->status==parsed?\"yes\\n\":\"no\\n\");\n");
	printf("col = popcont(); printf(\"restoring col to %%d\\n\", col);\n");
	printf("if (last->status == parsed) {\n");
	printf("  col = cur->remainder = last->remainder;\n");
	printf("  cur->value = last->value;\n");
	printf("  cur->status = parsed;\n");
	printf("  goto contin;\n");
	printf("}\n");
    }
}
#endif

static void alt_pre(struct s_node *n) {
    printf("printf(\"alt %d @ col %%d?\\n\", col);\n", n->id);
    printf("pushcont(cont);\n");
    printf("cont = %d;\n", n->id);
    printf("pushcont(col);\n");
}

static void alt_mid(struct s_node *n) {
    printf("if (status == parsed) { popcont(); goto contin; }\n");
    printf("else col = popcont();\n");
}

static void alt_post(struct s_node *n) {
    printf("if (status != parsed) col = popcont();\n");
    printf("else popcont();\n");
    printf("case %d:\n", n->id);
    printf("cont = popcont();\n");
    printf("printf(\"alt %d @ col %%d => %%s\\n\", col, status==parsed?\"yes\":\"no\");\n", n->id);
    printf("printf(\"col is %%d\\n\", col);\n");
}


/* XXX this should all be done in a data-directed stylee (i.e. with
 * function pointers) */
static void node(struct s_node *n) {
    struct s_node *p;

#if 0
    if (n->type == alt) {
	alternative(n);
    } else {
#endif
    switch (n->type) {
    case grammar:
	grammar_pre(n); break;
    case rule:
	rule_pre(n); break;
    case alt:
	alt_pre(n); break;
    case lit:
	literal(n->text); break;
    case call:
	emit_call(n); break;
    case seq:
	seq_pre(n); break;
    default: break;
    }

    /* XXX could optimize with seq_last() etc. I think */
    if (s_has_children(n->type))
	for (p = n->first; p; p = p->next) {
	    node(p);
	    if (p->next)
		switch (n->type) {
		case alt:
		    alt_mid(n); break;
		case seq:
		    seq_mid(n); break;
		default: break;
		}
	}

    switch(n->type) {
    case grammar:
	grammar_post(n); break;
    case rule:
	rule_post(n); break;
    case alt:
	alt_post(n); break;
    case seq:
	seq_post(n); break;
    default: break;
    }
}

void emit(struct s_node *g) {
    node(g);
}
