#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *string;

static int st_stack[100];
static int st_ptr = 0;

static void pushcont(int c) {
    printf("push(%d) -> stack[%d]\n", c, st_ptr);
    st_stack[st_ptr++] = c;
}
static int popcont(void) {
    printf("pop() stack[%d] -> %d\n", st_ptr - 1, st_stack[st_ptr - 1]);
    return st_stack[--st_ptr];
}

static char *copy(int from, int to) {
    char *r;
    int l;
printf("copy(%d, %d)\n", from, to);
    l = to - from;
    r = realloc(0, l + 1);
    if (r) {
	memcpy(r, string + from, l); 
	r[l] = '\0';
    }
    return r;
}
#define match() copy(col, cur->remainder)

static int col_stack[25];
static int col_ptr = 0;
static void pushcol(int c) { printf("push(%d) -> col_stack[%d]\n", c, col_ptr); col_stack[col_ptr++] = c; }
static int popcol(void) { return col_stack[--col_ptr]; }

/* a "thr" is a thunk or a rule/column pair */
enum thr { thr_thunk, thr_rule, thr_col };
struct thunkrule {
    enum thr discrim;
    int x;
};

enum status {
    no_parse, parsed, evaluated, uncomputed
};

#include "gen.c"

struct intermed {
    enum status status;
    union yy_union value; /* semantic value XXX needs to use g_name */
    int remainder; /* unparsed string */
    struct thunkrule thrs[20]; /* XXX */
    int thrs_ptr;
};

static struct intermed *cur;

static void pusheval(int t, enum thr type) {
    printf("pusheval(%d, %d) -> thrs[%d]\n", t, type, cur->thrs_ptr);
    cur->thrs[cur->thrs_ptr].discrim = type;
    cur->thrs[cur->thrs_ptr].x = t;
    ++cur->thrs_ptr;
}

static struct intermed *matrix;

static struct intermed *m_stack[25];
static int m_ptr = 0;
static void pushm(struct intermed *i) { m_stack[m_ptr++] = i; }
static struct intermed *popm(void) { return m_stack[--m_ptr]; }

static struct intermed *m2_stack[25];
static int m2_ptr = 0;
static void pushm2(struct intermed *i) { m2_stack[m2_ptr++] = i; }
static struct intermed *popm2(void) { return m2_stack[--m2_ptr]; }

static int input_length;

static int parse(void) {
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
	//pushthunk(-1); pushthunk(-1);
	evaluating = 1;
	i = 0;
	cur = matrix;
    eval_loop:
	if (i < cur->thrs_ptr) {
	    if (cur->thrs[i].discrim == thr_rule) {
		int col, rule;
		rule = cur->thrs[i].x; ++i;
		col = cur->thrs[i].x; ++i;
		pushm2(cur); pushcont(i);
		cur = matrix + col * n_rules + rule;
		i = 0;
		goto eval_loop;
	    } else {
		st = cur->thrs[i++].x;
		col = cur->thrs[i++].x;
		goto top;
	    }
	    goto eval_loop;
	}
	if (m2_ptr) {
	    i = popcont(); cur = popm2();
	    goto eval_loop;
	}
	goto contin;
    }

    if (matrix->status == evaluated) {
	printf("parsed with value " TYPE_PRINTF "\n", matrix->value.u0); /* XXX u0 */
    } else printf("not parsed\n");
    return matrix->status == evaluated;

contin:
    printf("continuing in state %d\n", cont);
    st = cont;
    goto top;

}

static void matrix_dump(void) {
    int r, s;

    for (s = 0; s < input_length + 1; ++s) printf("   %c   ", string[s]);
    printf("\n");
    for (r = 0; r < n_rules; ++r) {
	for (s = 0; s < input_length + 1; ++s) {
	    int elem = s * n_rules + r;
	    switch (matrix[elem].status) {
	    case uncomputed:
		printf("   _   "); break;
	    case no_parse:
		printf("   X   "); break;
	    case parsed:
		printf(" ? ,%2d ", matrix[elem].remainder);
		break;
	    case evaluated:
		printf(TYPE_PRINTF ",%2d ", matrix[elem].value.u0, matrix[elem].remainder); /* XXX u0 */
		break;
	    }
	}
	printf("\n");
    }
}

int pparse(char *str) {
    int i, matrix_size;
    string = str;
    input_length = strlen(string);
    matrix_size = n_rules * (input_length + 1);
    matrix = malloc(sizeof(struct intermed) * matrix_size);
    for (i = 0; i < matrix_size; ++i)
	matrix[i].status = uncomputed;

    printf("%d\n", parse());
    matrix_dump();
    return 0;
}

int main(int argc, char **argv) {
    if (argc > 1)
	pparse(argv[1]);
    return 0;
}
