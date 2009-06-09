#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *string;

int st_stack[100];
int st_ptr = 0;

void pushcont(int c) {
    printf("push(%d) -> stack[%d]\n", c, st_ptr);
    st_stack[st_ptr++] = c;
}
int popcont(void) {
    printf("pop() stack[%d] -> %d\n", st_ptr - 1, st_stack[st_ptr - 1]);
    return st_stack[--st_ptr];
}

int col_stack[25];
int col_ptr = 0;
static void pushcol(int c) { printf("push(%d) -> col_stack[%d]\n", c, col_ptr); col_stack[col_ptr++] = c; }
static int popcol(void) { return col_stack[--col_ptr]; }

/*
static int popthunk(void) {
    printf("pop() th_stack[%d] -> %d\n", th_ptr - 1, th_stack[th_ptr - 1]);
    return th_stack[--th_ptr];
}
*/

/* a "thr" is a thunk or a rule/column pair */
union thr {
    int thunk;
    int rulecol[2];
};
struct thunkrule {
    enum { thr_thunk, thr_rule } discrim;
    union thr x;
};

enum status {
    no_parse, parsed, uncomputed
};

struct intermed {
    enum status status;
    int value; /* semantic value  XXX needs to be generated union */
    int remainder; /* unparsed string */
    struct thunkrule thrs[20]; /* XXX */
    int thrs_ptr;
};

static struct intermed *cur;

//int th_stack[100];
//int th_ptr = 0;
static void pushthunk(int t) {
    printf("pushthunk(%d) -> thrs[%d]\n", t, cur->thrs_ptr);
    cur->thrs[cur->thrs_ptr].discrim = thr_thunk;
    cur->thrs[cur->thrs_ptr].x.thunk = t;
    ++cur->thrs_ptr;
}

static void pushrule(int rule, int col) {
    printf("pushrule(%d, %d) -> thrs[%d]\n", rule, col, cur->thrs_ptr);
    cur->thrs[cur->thrs_ptr].discrim = thr_rule;
    cur->thrs[cur->thrs_ptr].x.rulecol[0] = rule;
    cur->thrs[cur->thrs_ptr].x.rulecol[1] = col;
    ++cur->thrs_ptr;
}

struct intermed *matrix;

struct intermed *m_stack[25];
int m_ptr = 0;

void pushm(struct intermed *i) { m_stack[m_ptr++] = i; }
struct intermed *popm(void) { return m_stack[--m_ptr]; }

int parse(void) {
    enum status status;
    int cont, st;
    int col, rule_col;
    int i;
    int evaluating;
    struct intermed *last;
    col = 0;
    cont = -1;
    evaluating = 0;

#include "gen.c"

    if (parsed && !evaluating && matrix->status == parsed) {
	printf("PARSED! Time to start eval...\n");
	pushthunk(-1); pushthunk(-1);
#if 0
	printf("thunks: ");
	for (i = 0; i < th_ptr; ++i)
	    printf("%d ", th_stack[i]);
	printf("\ncols: ");
	for (i = 0; i < col_ptr; ++i)
	    printf("%d ", col_stack[i]);
	printf("\n");
#endif
	evaluating = 1;
	i = 0;
	cur = matrix;
    eval_loop:
	if (i < cur->thrs_ptr) {
	    if (cur->thrs[i].discrim == thr_rule) {
		pushm(cur); pushcont(i);
		cur = matrix + cur->thrs[i].x.rulecol[1] * n_rules +
		    cur->thrs[i].x.rulecol[0];
		i = 0;
		goto eval_loop;
	    } else {
		st = cur->thrs[i++].x.thunk;
		col = cur->thrs[i++].x.thunk;
		goto top;
	    }
	    goto eval_loop;
	}
	if (m_ptr) {
	    i = popcont(); cur = popm();
	    ++i;
	    goto eval_loop;
	}
	//matrix->thrs_ptr = 0;
	//st = matrix->thrs[matrix->thrs_ptr++].x.thunk;
	//col = matrix->thrs[matrix->thrs_ptr++].x.thunk;
	//col_ptr = 0;
	//goto top;
    }

    if (matrix->status == parsed) {
	printf("parsed with value %d\n", matrix->value);
    } else printf("not parsed\n");
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
