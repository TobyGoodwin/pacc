/* A manually generated, explicit-control packrat parser for the (pacc's
 * version of) peg input files.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *string;

enum states {
    done, grammar, grammar_0, grammar_1, rule
};

enum states st_stack[25];
int st_ptr = 0;

void pushcont(enum states c) {
    st_stack[st_ptr++] = c;
}
enum states popcont(void) {
    return st_stack[--st_ptr];
}

enum status {
    no_parse, parsed, uncomputed
};

struct intermed {
    enum status status;
    int value; /* semantic value */
    int remainder; /* unparsed string */
};

static int n_rules = 3;
struct intermed *matrix;

struct intermed *m_stack[25];
int m_ptr = 0;

void pushm(struct intermed *i) { m_stack[m_ptr++] = i; }
struct intermed *popm(void) { return m_stack[--m_ptr]; }

int parse(void) {
    enum states cont, st;
    int col;
    struct intermed *cur, *last;
    st = grammar;
    col = 0;
    cont = done;

top:
    switch (st) {
    case grammar:
	cur = matrix + col * n_rules + 0;
	if (cur->status == uncomputed) {
	    cur->status = no_parse;
	    st = rule;
	    pushcont(cont); pushm(cur);
	    cont = grammar_0;
	    goto top;
    case grammar_0:
	    cont = popcont(); last = cur; cur = popm();
	    if (last->status != parsed)
		goto contin;
	    *cur = *last;
	    col = cur->remainder;
	    st = grammar;
	    pushcont(cont); pushm(cur);
	    cont = grammar_1;
	    goto top;
    case grammar_1:
	    cont = popcont(); last = cur; cur = popm();
	    if (last->status == parsed) {
		*cur = *last;
		col = cur->remainder;
		goto contin;
	    }
	    /* empty */
	    cur->status = parsed;
	    cur->remainder = col;
	}
	goto contin;

    case rule:
	cur = matrix + col * n_rules + 1;
	if (cur->status == uncomputed) {
	    cur->status = no_parse;
	    if (! (string[col] == 'a')) 
		goto contin;
	    cur->remainder = col + 1;
	    cur->value = 11;
	    cur->status = parsed;
	}
	goto contin;

    case done:
	break;
    }
    return matrix->status == parsed;

contin:
    st = cont;
    goto top;

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
    string = argv[1];
    n = strlen(string);
    matrix_size = n_rules * (n + 1);
    matrix = malloc(sizeof(struct intermed) * matrix_size);
    for (i = 0; i < matrix_size; ++i)
	matrix[i].status = uncomputed;

    printf("%d\n", parse());
    matrix_dump(n);
    return 0;
}
