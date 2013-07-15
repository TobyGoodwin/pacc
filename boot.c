#include <stdio.h>

#include "load.h"
#include "syntax.h"

extern int pacc_wrap(const char *, char *, size_t, struct s_node **);

void walk(struct s_node *p, int d) {
    int kid = p->first != 0;
    int sib = p->next != 0;
    char *t = decode_type(p->type);

    if (sib) walk(p->next, d);
    if (kid) walk(p->first, d + 1);
    printf("n = ");
    if (sib) printf("cons(");
    if (kid) printf("s_kid(%s, p%d)", t, d + 1);
    else printf("s_new(%s)", t);
    if (sib) printf(", p%d)", d);
    printf("; ");
    if (s_is_number(p->type))
        printf("n->number = %d; ", p->number);
    if (s_is_pair(p->type))
        printf("n->pair[0] = %d; n->pair[1] = %d; ", p->pair[0], p->pair[1]);
    if (s_is_text(p->type) && p->text) {
	char *s;
        printf("n->text = \"");
	for (s = p->text; *s; ++s) {
	    switch (*s) {
		case '\n': printf("\\n"); break;
		case '"': printf("\\\""); break;
		case '\\': printf("\\\\"); break;
		default: printf("%c", *s);
	    }
	}
	printf("\"; ");
    }
    printf("p%d = n;\n", d);
}


int main(void) {
    char *in;
    char *n = "pacc.pacc";
    off_t size;
    struct s_node *p;

    in = load(n, &size);

    if (!pacc_wrap(n, in, size, &p))
        return 1;

    printf("#include <sys/types.h>\n");
    printf("#include \"syntax.h\"\n");
    printf("int pacc_wrap(\n");
    printf("    __attribute__((unused)) const char *ign0,\n");
    printf("    __attribute__((unused)) char *ign1,\n");
    printf("    __attribute__((unused)) size_t ign2,\n");
    printf("    struct s_node **result) {\n");
    printf("struct s_node *n, *p0, *p1, *p2, *p3, *p4, *p5, *p6, *p7;\n");

    walk(p, 0);

    printf("*result = p0;\n");
    printf("return 1;\n");
    printf("}\n");
}
