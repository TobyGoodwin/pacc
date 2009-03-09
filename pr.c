/* A packrat parser for Bryan Ford's trivial grammar:

Additive <- Multitive '+' Additive | Multitive
Multitive <- Primary '*' Multitive | Primary
Primary <- '(' Additive ')' | Decimal
Decimal <- '0' | '1' | ... | '9'

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int n_rules = 4;

char *string;

enum status {
    uncomputed, parsed, no_parse
};

struct intermed {
    enum status status;
    int value; /* semantic value */
    int remainder; /* unparsed string */
};

struct intermed *matrix;

struct intermed *additive(int col);
struct intermed *multitive(int col);
struct intermed *primary(int col);
struct intermed *decimal(int col);

struct intermed *additive(int col) {
    int elem = col * n_rules + 0;
    if (matrix[elem].status == uncomputed) {
	struct intermed *m;

	m = multitive(col);
	if (m->status == parsed)
	    if (string[m->remainder] == '+') {
		struct intermed *a;
		a = additive(m->remainder + 1);
		if (a->status == parsed) {
		    matrix[elem].remainder = a->remainder;
		    matrix[elem].value = m->value + a->value;
		    matrix[elem].status = parsed;
		    return matrix + elem;
		}
	    }
	matrix[elem] = *multitive(col);
    }
    return matrix + elem;
}

struct intermed *multitive(int col) {
    int elem = col * n_rules + 1;
    if (matrix[elem].status == uncomputed) {
	struct intermed *p;
	p = primary(col);
	if (p->status == parsed) {
	    if (string[p->remainder] == '*') {
		struct intermed *m;
		m = multitive(p->remainder + 1);
		if (m->status == parsed) {
		    matrix[elem].remainder = m->remainder;
		    matrix[elem].value = p->value * m->value;
		    matrix[elem].status = parsed;
		    return matrix + elem;
		}
	    }
	}
	matrix[elem] = *primary(col);
    }
    return matrix + elem;
}

struct intermed *primary(int col) {
    int elem = col * n_rules + 2;
    if (matrix[elem].status == uncomputed) {
	if (string[col] == '(') {
	    struct intermed *a;
	    a = additive(col + 1);
	    if (a->status == parsed)
		if (string[a->remainder] == ')') {
		    matrix[elem].remainder = a->remainder + 1;
		    matrix[elem].value = a->value;
		    matrix[elem].status = parsed;
		    return matrix + elem;
		}
	}
	matrix[elem] = *decimal(col);
    }
    return matrix + elem;
}

struct intermed *decimal(int col) {
    int elem = col * n_rules + 3;
    if (matrix[elem].status == uncomputed) {
	char in = string[col];
	if (in >= '0' && in <= '9') {
	    matrix[elem].remainder = col + 1;
	    matrix[elem].value = in - '0';
	    matrix[elem].status = parsed;
	} else
	    matrix[elem].status = no_parse;
    }
    return matrix + elem;
}

void matrix_dump(int n) {
    int r, s;

    for (s = 0; s < n + 1; ++s) printf("   %c   ", string[s]);
    printf("\n");
    for (r = 0; r < n_rules; ++r) {
	for (s = 0; s < n + 1; ++s) {
	    int elem = s * n_rules + r;
	    switch (matrix[elem].status) {
	    case uncomputed:
		printf("   _   "); break;
	    case no_parse:
		printf("   X   "); break;
	    case parsed:
		printf("%3d,%2d ", matrix[elem].value, matrix[elem].remainder);
		break;
	    }
	}
	printf("\n");
    }
}

int main(int argc, char **argv) {
    int i, n, matrix_size;
    struct intermed *r;

    string = argv[1];
    printf("%s\n", string);
    n = strlen(string);
    matrix_size = n_rules * (n + 1);
    matrix = malloc(sizeof(struct intermed) * matrix_size);
    for (i = 0; i < matrix_size; ++i)
	matrix[i].status = uncomputed;

    r = additive(0);
    if (r->status == parsed) {
	printf("==> %d\n", r->value);
	if (string[r->remainder] != '\0')
	    printf("input not completely consumed\n");
    } else {
	printf("parse error\n");
    }
    matrix_dump(n);
    return 0;
}
