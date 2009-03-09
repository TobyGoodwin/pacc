/* A manually generated, explicit-control packrat parser for the grammar
 *
 * S = A / B
 * A = 'a'
 * B = 'b'
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *string;

enum states {
    done, r0, r0_0, r0_1, r1, r2
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
    uncomputed, parsed, no_parse
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
    st = r0;
    col = 0;
    cont = done;

top:
    switch (st) {
    case r0:
	cur = matrix + col * n_rules + 0;
	if (cur->status == uncomputed) {
	    cur->status = no_parse;
	    st = r1;
	    pushcont(cont); pushm(cur);
	    cont = r0_0;
	    goto top;
    case r0_0:
	    cont = popcont();
	    last = cur; cur = popm();
	    if (last->status == parsed) {
		cur->remainder = last->remainder;
		cur->value = last->value;
		cur->status = parsed;
		goto r0_end;
	    }
	    st = r2;
	    pushcont(cont); pushm(cur);
	    cont = r0_1;
	    goto top;
    case r0_1:
	    cont = popcont(); last = cur; cur = popm();
	    if (last->status == parsed) {
		cur->remainder = last->remainder;
		cur->value = last->value;
		cur->status = parsed;
		goto r0_end;
	    }
	}
r0_end:
	st = popcont();
	goto top;


    case r1:
	cur = matrix + col * n_rules + 1;
	if (cur->status == uncomputed) {
	    cur->status = no_parse;
	    if (string[col] == 'a') {
		cur->remainder = col + 1;
		cur->value = 11;
		cur->status = parsed;
	    }
	}
	st = cont;
	goto top;

    case r2:
	cur = matrix + col * n_rules + 2;
	if (cur->status == uncomputed) {
	    cur->status = no_parse;
	    if (string[col] == 'b') {
		cur->remainder = col + 1;
		cur->value = 22;
		cur->status = parsed;
	    }
	}
	st = cont;
	goto top;

    case done:
	break;
    }
    return 1;
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
