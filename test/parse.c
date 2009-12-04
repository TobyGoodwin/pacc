#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"

#define Trace if (0)

static char *string;

#define ST_STACK_BODGE 2000
static int st_stack[ST_STACK_BODGE];
static int st_ptr = 0;

static void pushcont(int c) {
    if (st_ptr == ST_STACK_BODGE) {
	fprintf(stderr, "st_stack overflow\n");
	exit(1);
    }
    Trace fprintf(stderr, "push(%d) -> stack[%d]\n", c, st_ptr);
    st_stack[st_ptr++] = c;
}
static int popcont(void) {
    Trace fprintf(stderr, "pop() stack[%d] -> %d\n", st_ptr - 1, st_stack[st_ptr - 1]);
    return st_stack[--st_ptr];
}

static void nomem(void) {
    fprintf(stderr, "out of memory\n");
    exit(1);
}

static char *copy(int from, int to) {
    char *r;
    int l;
    Trace fprintf(stderr, "copy(%d, %d)\n", from, to);
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
    Trace fprintf(stderr, "push(%d) -> col_stack[%d]\n", c, col_ptr);
    col_stack[col_ptr++] = c;
}
static int popcol(void) {
    Trace fprintf(stderr, "pop() col_stack[%d] -> %d\n", col_ptr - 1, col_stack[col_ptr - 1]);
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

/* A dynamic array of error strings */
char **_pacc_err = 0;
size_t _pacc_err_alloc = 0;
size_t _pacc_err_valid = 0;
off_t _pacc_err_col;

#define n_rules 2
union yy_union {
    int u0;
    int u1;
};
#define TYPE_PRINTF "%d"
#define PACC_TYPE int
/* not yet... yytype yyvalue; */

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
    Trace fprintf(stderr, "pusheval(%d, %d, %d) -> thrs[%d]\n", t, col, type, cur->thrs_ptr);
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

static int engine(PACC_TYPE *result) {
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

st = 106;
top:
Trace fprintf(stderr, "switch to state %d\n", st);
switch(st) {
case 106: /* P */
Trace fprintf(stderr, "rule 0 (P) col %d\n", col);
rule_col = col;
cur = matrix + col * n_rules + 0;
if (cur->status == uncomputed) {
Trace fprintf(stderr, "seq 104 @ col %d?\n", col);
pushcont(cont);
cont = 104;
status = parsed;
/* bind: a */
Trace fprintf(stderr, "%d: bind_pre()\n", 102);
Trace fprintf(stderr, "will bind a @ rule 1, col %d\n", col);
pusheval(1, col, thr_bound);
pushcont(rule_col);
pushcont(cont); pushm(cur);
cont = 101;
st = 110; /* call A */
goto top;
case 101: /* return from A */
cont = popcont();
last = cur; cur = popm();
status = last->status;
col = last->remainder;
rule_col = popcont();
/* end bind: a */
if (status == no_parse) {
    goto contin;
}
Trace fprintf(stderr, "%d: emit_expr()\n", 103);
pusheval(103, rule_col, thr_thunk);
pusheval(0, col, thr_col);
case 103:
if (evaluating) {
    struct intermed *_pacc_p;
    _pacc_p = cur = matrix + col * n_rules + 0;
    evaluating = 1;
    cur = _pacc_p;
    cur->value.u0 = ( 5 );
Trace fprintf(stderr, "stash %d to (%d, 0)\n", cur->value.u0, col);
    goto eval_loop;
}
case 104:
cont = popcont();
Trace fprintf(stderr, "seq 104 @ col %d => %s\n", rule_col, status!=no_parse?"yes":"no");
Trace fprintf(stderr, "col is %d\n", col);
    cur->status = status;
    cur->remainder = col;
    if (_pacc_err_col == rule_col) {
        _pacc_err_valid = 0;
{
    int doit, append;
Trace fprintf(stderr, "error(P, 0) at col %d\n", col);
    append = doit = 1;
    if (col > _pacc_err_col) append = 0;
    else if (col == _pacc_err_col) {
        size_t i;
        for (i = 0; i < _pacc_err_valid; ++i) {
            if (strcmp(_pacc_err[i], "P") == 0) doit = 0;
        }
    } else doit = 0;
    if (doit) {
        if (append) ++_pacc_err_valid;
        else _pacc_err_valid = 1;
        if (_pacc_err_valid > _pacc_err_alloc) {
            _pacc_err_alloc = 2 * _pacc_err_alloc + 1;
            _pacc_err = realloc(_pacc_err, _pacc_err_alloc * sizeof(char *));
            if (!_pacc_err) nomem();
        }
        _pacc_err[_pacc_err_valid - 1] = "P";
        _pacc_err_col = col;
    }
}
    }
}
goto contin;
case 110: /* A */
Trace fprintf(stderr, "rule 1 (A) col %d\n", col);
rule_col = col;
cur = matrix + col * n_rules + 1;
if (cur->status == uncomputed) {
Trace fprintf(stderr, "seq 108 @ col %d?\n", col);
pushcont(cont);
cont = 108;
status = parsed;
Trace fprintf(stderr, "lit 107 @ col %d => ", col);
if (col + 1 <= input_length &&
        strncmp("5", string + col, 1) == 0) {
    status = parsed;
    col += 1;
    Trace fprintf(stderr, "yes (col=%d)\n", col);
} else {
{
    int doit, append;
Trace fprintf(stderr, "error(5, 1) at col %d\n", col);
    append = doit = 1;
    if (col > _pacc_err_col) append = 0;
    else if (col == _pacc_err_col) {
        size_t i;
        for (i = 0; i < _pacc_err_valid; ++i) {
            if (strcmp(_pacc_err[i], "\"5\"") == 0) doit = 0;
        }
    } else doit = 0;
    if (doit) {
        if (append) ++_pacc_err_valid;
        else _pacc_err_valid = 1;
        if (_pacc_err_valid > _pacc_err_alloc) {
            _pacc_err_alloc = 2 * _pacc_err_alloc + 1;
            _pacc_err = realloc(_pacc_err, _pacc_err_alloc * sizeof(char *));
            if (!_pacc_err) nomem();
        }
        _pacc_err[_pacc_err_valid - 1] = "\"5\"";
        _pacc_err_col = col;
    }
}
    status = no_parse;
    Trace fprintf(stderr, "no (col=%d)\n", col);
}
case 108:
cont = popcont();
Trace fprintf(stderr, "seq 108 @ col %d => %s\n", rule_col, status!=no_parse?"yes":"no");
Trace fprintf(stderr, "col is %d\n", col);
    cur->status = status;
    cur->remainder = col;
    if (_pacc_err_col == rule_col) {
        _pacc_err_valid = 0;
{
    int doit, append;
Trace fprintf(stderr, "error(A, 0) at col %d\n", col);
    append = doit = 1;
    if (col > _pacc_err_col) append = 0;
    else if (col == _pacc_err_col) {
        size_t i;
        for (i = 0; i < _pacc_err_valid; ++i) {
            if (strcmp(_pacc_err[i], "A") == 0) doit = 0;
        }
    } else doit = 0;
    if (doit) {
        if (append) ++_pacc_err_valid;
        else _pacc_err_valid = 1;
        if (_pacc_err_valid > _pacc_err_alloc) {
            _pacc_err_alloc = 2 * _pacc_err_alloc + 1;
            _pacc_err = realloc(_pacc_err, _pacc_err_alloc * sizeof(char *));
            if (!_pacc_err) nomem();
        }
        _pacc_err[_pacc_err_valid - 1] = "A";
        _pacc_err_col = col;
    }
}
    }
}
goto contin;
case -1: break;
}

    if (parsed && !evaluating && matrix->status == parsed) {
	Trace fprintf(stderr, "PARSED! Time to start eval...\n");
	//pushthunk(-1); pushthunk(-1);
	evaluating = 1;
	_pacc_i = 0;
	cur = matrix;
    eval_loop:
	Trace fprintf(stderr, "eval loop with _pacc_i == %d\n", _pacc_i);
	if (_pacc_i < cur->thrs_ptr) {
	    if (cur->thrs[_pacc_i].discrim == thr_rule ||
		    cur->thrs[_pacc_i].discrim == thr_bound) {
		int col, rule;
		rule = cur->thrs[_pacc_i].x; col = cur->thrs[_pacc_i].col;
		++_pacc_i;
		Trace fprintf(stderr, "eval loop: r%d @ c%d\n", rule, col);
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
	Trace fprintf(stderr, "eval finished\n");
	goto contin;
    }

    if (matrix->status != evaluated) {
       size_t i;
       printf("expected ");
       for (i = 0; i < _pacc_err_valid; ++i) {
           printf("%s", _pacc_err[i]);
           if (i + 1 < _pacc_err_valid) {
               printf(", ");
               if (i + 2 == _pacc_err_valid) printf("or ");
           }
       }
       printf(" at column %ld\n", _pacc_err_col);
    }

    if (matrix->status == evaluated) {
	Trace fprintf(stderr, "parsed with value " TYPE_PRINTF "\n", matrix->value.u0); /* XXX u0 */
	*result = matrix->value.u0;
    } else if (matrix->status == parsed) {
	printf("parsed with void value\n");
    } else printf("not parsed\n");
    return matrix->status == evaluated;

contin:
    Trace fprintf(stderr, "continuing in state %d\n", cont);
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

int parse(char *addr, off_t l, PACC_TYPE *result) {
    int i, j, matrix_size;
    string = addr;
    input_length = l;
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
