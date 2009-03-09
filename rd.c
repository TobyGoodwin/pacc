/* A (memory leaking!) recursive descent parser for Bryan Ford's trivial
 * grammar:

Additive <- Multitive '+' Additive | Multitive
Multitive <- Primary '*' Multitive | Primary
Primary <- '(' Additive ')' | Decimal
Decimal <- '0' | '1' | ... | '9'

*/

#include <stdio.h>
#include <stdlib.h>

struct parsed {
    int value; /* semantic value */
    char *remainder; /* unparsed string */
};

struct parsed *additive(struct parsed *);
struct parsed *multitive(struct parsed *);
struct parsed *primary(struct parsed *);
struct parsed *decimal(struct parsed *);

struct parsed *additive(struct parsed *t) {
    struct parsed *m;

    m = multitive(t);
    if (m)
	if (m->remainder[0] == '+') {
	    struct parsed *a;
	    ++m->remainder;
	    a = additive(m);
	    if (a) {
		a->value = m->value + a->value;
		return a;
	    }
	}
    return multitive(t);
}

struct parsed *multitive(struct parsed *t) {
    struct parsed *p;
    p = primary(t);
    if (p) {
	if (p->remainder[0] == '*') {
	    struct parsed *m;
	    ++p->remainder;
	    m = multitive(p);
	    if (p) {
		m->value = p->value * m->value;
		return m;
	    }
	}
    }
    return primary(t);
}

struct parsed *primary(struct parsed *t) {
    if (t->remainder[0] == '(') {
	struct parsed *a;
	++t->remainder;
	a = additive(t);
	--t->remainder;
	if (a)
	    if (a->remainder[0] == ')') {
		++a->remainder;
		return a;
	    }
    }
    return decimal(t);
}

struct parsed *decimal(struct parsed *t) {
    char *in = t->remainder;
    if (in[0] >= '0' && in[0] <= '9') {
	struct parsed *sp;
	sp = malloc(sizeof (struct parsed));
	sp->value = in[0] - '0';
	sp->remainder = in + 1;
	return sp;
    }
    return 0;
}

int main(int argc, char **argv) {
    struct parsed i, *r;

    printf("%s\n", argv[1]);
    i.remainder = argv[1];
    r = additive(&i);
    if (r) {
	printf("==> %d\n", r->value);
	if (*r->remainder != '\0')
	    printf("input not completely consumed\n");
    } else {
	printf("parse error\n");
    }
    return 0;
}
