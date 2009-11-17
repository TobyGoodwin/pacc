#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *string;

#define ST_STACK_BODGE 2000
static int st_stack[ST_STACK_BODGE];
static int st_ptr = 0;

static void pushcont(int c) {
    if (st_ptr == ST_STACK_BODGE) {
	fprintf(stderr, "st_stack overflow\n");
	exit(1);
    }
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
#define match() copy(col, col_expr)
#define rmatch() copy(col_expr, cur->remainder)

static int col_stack[25];
static int col_ptr = 0;
static void pushcol(int c) {
    printf("push(%d) -> col_stack[%d]\n", c, col_ptr);
    col_stack[col_ptr++] = c;
}
static int popcol(void) {
    printf("pop() col_stack[%d] -> %d\n", col_ptr - 1, col_stack[col_ptr - 1]);
    return col_stack[--col_ptr];
}

/* a "thr" is a thunk or a rule/column pair */
enum thr { thr_thunk = 71, thr_bound, thr_rule, thr_col };
struct thunkrule {
    enum thr discrim;
    int x;
    int col;
};

enum status {
    no_parse, parsed, evaluated, uncomputed
};

#include "pacc-part.c"

#define THR_STACK_BODGE 40

struct intermed {
    enum status status;
    union yy_union value; /* semantic value XXX needs to use g_name */
    int remainder; /* unparsed string */
    struct thunkrule thrs[THR_STACK_BODGE]; /* XXX */
    int thrs_ptr;
    int rule, col; /* XXX: redundant, but handy for debugging */
};

static struct intermed *cur;

static void pusheval(int t, int col, enum thr type) {
    printf("pusheval(%d, %d, %d) -> thrs[%d]\n", t, col, type, cur->thrs_ptr);
    cur->thrs[cur->thrs_ptr].discrim = type;
    cur->thrs[cur->thrs_ptr].x = t;
    cur->thrs[cur->thrs_ptr].col = col;
    ++cur->thrs_ptr;
}

static struct intermed *matrix;

#define M_STACK_BODGE 500
static struct intermed *m_stack[M_STACK_BODGE];
static int m_ptr = 0;
static void pushm(struct intermed *i) {
    if (m_ptr == M_STACK_BODGE) { printf("out of m stack space\n"); exit(0); }
    m_stack[m_ptr++] = i;
}
static struct intermed *popm(void) { return m_stack[--m_ptr]; }

static struct intermed *m2_stack[M_STACK_BODGE];
static int m2_ptr = 0;
static void pushm2(struct intermed *i) {
    if (m2_ptr == M_STACK_BODGE) { printf("out of m2 stack space\n"); exit(0); }
    m2_stack[m2_ptr++] = i;
}
static struct intermed *popm2(void) { return m2_stack[--m2_ptr]; }

static int input_length;

static int engine(struct s_node **result) {
    enum status status;
    int cont, st;
    int col, rule_col, col_expr;
    int _pacc_i;
    int evaluating;
    struct intermed *last;
    col = 0;
    cont = -1;
    evaluating = 0;
    int pos;

#include "pacc-part.c"

    if (parsed && !evaluating && matrix->status == parsed) {
	printf("PARSED! Time to start eval...\n");
	//pushthunk(-1); pushthunk(-1);
	evaluating = 1;
	_pacc_i = 0;
	cur = matrix;
    eval_loop:
	printf("eval loop with _pacc_i == %d\n", _pacc_i);
	if (_pacc_i < cur->thrs_ptr) {
	    if (cur->thrs[_pacc_i].discrim == thr_rule ||
		    cur->thrs[_pacc_i].discrim == thr_bound) {
		int col, rule;
		rule = cur->thrs[_pacc_i].x; col = cur->thrs[_pacc_i].col;
		++_pacc_i;
printf("eval loop: r%d @ c%d\n", rule, col);
		pushm2(cur); pushcont(_pacc_i);
		cur = matrix + col * n_rules + rule;
		_pacc_i = 0;
		goto eval_loop;
	    } else {
		st = cur->thrs[_pacc_i].x; col = cur->thrs[_pacc_i].col;
		++_pacc_i;
		col_expr = cur->thrs[_pacc_i].col;
		++_pacc_i;
		goto top;
	    }
	    goto eval_loop;
	}
	cur->status = evaluated;
	if (m2_ptr) {
	    _pacc_i = popcont(); cur = popm2();
	    goto eval_loop;
	}
	printf("eval finished\n");
	goto contin;
    }

    if (matrix->status == evaluated) {
	printf("parsed with value " TYPE_PRINTF "\n", matrix->value.u0); /* XXX u0 */
	*result = matrix->value.u0;

	if (0) {
	    struct s_node *p;
	    p = matrix->value.u0;
	    s_dump(p);
	    if (1) {
		desugar(p);
		resolve(p);
		printf("---cut here\n");
		emit(p);
		printf("---cut here\n");
	    }
	}

    } else if (matrix->status == parsed) {
	printf("parsed with void value\n");
    } else printf("not parsed\n");
    return matrix->status == evaluated;

contin:
    printf("continuing in state %d\n", cont);
    st = cont;
    goto top;

}

/* XXX Let's put prefixes in the tree, please? */
char *prefix = 0;

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

int parse(struct s_node **result, char *str) {
    int i, j, matrix_size;
    string = str;
    input_length = strlen(string);
    matrix_size = n_rules * (input_length + 1);
    matrix = malloc(sizeof(struct intermed) * matrix_size);
    for (i = 0; i < n_rules; ++i)
	for (j = 0; j < input_length + 1; ++j) {
	    matrix[j * n_rules + i].status = uncomputed;
	    matrix[j * n_rules + i].rule = i;
	    matrix[j * n_rules + i].col = j;
	    matrix[j * n_rules + i].thrs_ptr = 0;
	}

    return engine(result);
}
