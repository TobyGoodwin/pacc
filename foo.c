#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *string;

int st_stack[25];
int st_ptr = 0;

void pushcont(int c) {
    printf("push(%d) -> stack[%d]\n", c, st_ptr);
    st_stack[st_ptr++] = c;
}
int popcont(void) {
    printf("pop() stack[%d] -> %d\n", st_ptr - 1, st_stack[st_ptr - 1]);
    return st_stack[--st_ptr];
}

int th_stack[25];
int th_ptr = 0;

static void pushthunk(int t) {
    printf("push(%d) -> th_stack[%d]\n", t, th_ptr);
    th_stack[th_ptr++] = t;
}
/*
static int popthunk(void) {
    printf("pop() th_stack[%d] -> %d\n", th_ptr - 1, th_stack[th_ptr - 1]);
    return th_stack[--th_ptr];
}
*/
enum status {
    no_parse, parsed, uncomputed
};

struct intermed {
    enum status status;
    int value; /* semantic value  XXX needs to be generated union */
    int remainder; /* unparsed string */
};

struct intermed *matrix;

struct intermed *m_stack[25];
int m_ptr = 0;

void pushm(struct intermed *i) { m_stack[m_ptr++] = i; }
struct intermed *popm(void) { return m_stack[--m_ptr]; }

int parse(void) {
    struct intermed *bind_val;
    enum status status;
    int cont, st;
    int col, rule_col;
    int evaluating;
    struct intermed *cur, *last;
    col = 0;
    cont = -1;
    evaluating = 0;

#include "gen.c"

    if (parsed && !evaluating) {
	pushthunk(-1); pushthunk(-1);
	evaluating = 1;
	th_ptr = 0;
	st = th_stack[th_ptr++];
	col = th_stack[th_ptr++];
	goto top;
    }

    if (matrix->status == parsed) {
	printf("parsed with value %d\n", matrix->value);
    }
    return matrix->status == parsed;

contin:
    printf("continuing in state %d\n", cont);
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

int pparse(char *str) {
    int i, n, matrix_size;
    string = str;
    n = strlen(string);
    matrix_size = n_rules * (n + 1);
    matrix = malloc(sizeof(struct intermed) * matrix_size);
    for (i = 0; i < matrix_size; ++i)
	matrix[i].status = uncomputed;

    printf("%d\n", parse());
    matrix_dump(n);
    return 0;
}

int main(int argc, char **argv) {
    if (argc > 1)
	pparse(argv[1]);
    return 0;
}
