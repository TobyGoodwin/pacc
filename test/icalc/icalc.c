#define PACC_NAME pacc
    
#define PACC_ASSERT 1
#define PACC_CAN_TRACE 1
    
/*
vim: syntax=c
*/

#if PACC_ASSERT
#undef NDEBUG
#else
#define NDEBUG 1
#endif
#define PACC_XGLUE(a, b) a ## _ ## b
#define PACC_GLUE(a, b) PACC_XGLUE(a, b)
#define PACC_SYM(s) PACC_GLUE(PACC_NAME, s)
#define PACC_TRACE if (PACC_CAN_TRACE && PACC_SYM(trace))

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int PACC_SYM(trace) = 0;

static void pacc_panic(const char *e) {
    fprintf(stderr, "pacc: panic: %s\n", e);
    exit(1);
}

static void pacc_nomem(void) { pacc_panic("out of memory"); }

enum pacc_status {
    no_parse, parsed, evaluated, uncomputed
};

typedef size_t *ref_t;

    static const int n_rules = 15;
    static const int start_rule_id = 113;
    union PACC_SYM(vals) {
      int u0;
    }
    ;
#define TYPE_PRINTF "%d"
#define PACC_TYPE int

struct pacc_mid {
    /* Where in the matrix are we? Note that the bottom two bits of rule
     * also encode a status. */
    long rule; size_t col;
    size_t remainder; /* unparsed string */
    long expr_id; /* id of the expression to evaluate, yielding... */
    union PACC_SYM(vals) value; /* ... the semantic value */
    struct {
	long call_id; /* a call */
	size_t col; /* a column */
    } *evlis;
    size_t ev_alloc;
    size_t ev_valid;
};

static struct pacc_mid *cur;

static void _pacc_save_core(long c, size_t col) {
    PACC_TRACE fprintf(stderr, "_pacc_save_core(%ld, %ld)\n", c, col);
    if (cur->ev_valid == cur->ev_alloc) {
	cur->ev_alloc = cur->ev_alloc * 2 + 1;
	cur->evlis = realloc(cur->evlis, cur->ev_alloc * sizeof(*cur->evlis));
	if (!cur->evlis) pacc_nomem();
    }
    cur->evlis[cur->ev_valid].call_id = c;
    cur->evlis[cur->ev_valid].col = col;
    ++cur->ev_valid;
}

struct _pacc_coord {
    size_t n;
    int c[2];
};

/* a pacc parser */
struct pacc_parser {
    unsigned char *string;
    size_t input_length;
    PACC_TYPE value;
    struct pacc_mid **m_bkt;
    unsigned int m_bkt_cnt;
    unsigned char *m_valid;
    unsigned char m_chain_max;
    
    unsigned char *stack; /* the stack */
    unsigned char *sp; /* next slot in stack */
    unsigned char *stack_alloc; /* last slot in stack */

    /* Dynamic array of error strings */
    char **err;
    size_t err_alloc;
    size_t err_valid;
    /* The highest column to have associated error */
    size_t err_col;

    /* Dynamic array of co-ordinates */
    struct _pacc_coord *coord;
    size_t coord_alloc;
    size_t coord_valid;
};

static void _pacc_push(void *x, size_t s, struct pacc_parser *p) {
    if (p->sp + s >= p->stack_alloc) {
	size_t l = 2 * (p->stack_alloc - p->stack) + s;
	unsigned char *n = realloc(p->stack, l);
	if (!n) pacc_nomem();
	p->sp = n + (p->sp - p->stack);
	p->stack = n;
	p->stack_alloc = n + l + 1;
    }
    assert(p->sp >= p->stack && p->sp + s < p->stack_alloc);
    memcpy(p->sp, x, s);
    p->sp += s;
}

#define _pacc_Push(v) _pacc_push(&(v), sizeof (v), _pacc)

static void *_pacc_pop(size_t s, struct pacc_parser *p) {
    assert(p->sp - s >= p->stack);
    p->sp -= s;
    return p->sp;
}

#define _pacc_Pop(v) memcpy(&(v), _pacc_pop(sizeof (v), _pacc), sizeof (v))
#define _pacc_Discard(v) ((void)_pacc_pop(sizeof (v), _pacc))

#define ref() (&cur->col)
#define ref_0(a) (_pacc->string[*a])
#define ref_cmp(a, s) (_pacc_ref_cmp((a), (s), _pacc))
#define ref_dup(a) (_pacc_ref_dup((a), _pacc))
#define ref_len(a) ((a)[1] - (a)[0])
#define ref_ptr(a) (_pacc->string + *(a))
#define ref_str() (_pacc_ref_dup(ref(), _pacc))
#define ref_streq(a, b) (_pacc_ref_streq((a), (b), _pacc))

static char *_pacc_ref_dup(ref_t a, struct pacc_parser *p) {
    char *restrict r;
    const char *restrict s;
    size_t l;

    l = a[1] - a[0];
    r = realloc(0, l + 1); if (!r) pacc_nomem();
    s = (char *)p->string + a[0];
    strncpy(r, s, l);
    r[l] = '\0';
    return r;
}


// Copyright (c) 2008-2010 Bjoern Hoehrmann <bjoern@hoehrmann.de>
// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

#define PACC_UTF8_ACCEPT 0
#define PACC_UTF8_REJECT 12

static const uint8_t utf8d[] = {
  // The first part of the table maps bytes to character classes that
  // to reduce the size of the transition table and create bitmasks.
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
   7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
   8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3, 11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,

  // The second part is a transition table that maps a combination
  // of a state of the automaton and a character class to a state.
   0,12,24,36,60,96,84,12,12,12,48,72, 12,12,12,12,12,12,12,12,12,12,12,12,
  12, 0,12,12,12,12,12, 0,12, 0,12,12, 12,24,12,12,12,12,12,24,12,24,12,12,
  12,12,12,12,12,12,12,24,12,12,12,12, 12,24,12,12,12,12,12,12,12,24,12,12,
  12,12,12,12,12,12,12,36,12,36,12,12, 12,36,12,12,12,12,12,36,12,36,12,12,
  12,36,12,12,12,12,12,12,12,12,12,12, 
};

inline static uint32_t
pacc_decode(uint32_t* state, uint32_t* codep, uint32_t byte) {
  uint32_t type = utf8d[byte];

  *codep = (*state != PACC_UTF8_ACCEPT) ?
    (byte & 0x3fu) | (*codep << 6) :
    (0xff >> type) & (byte);

  *state = utf8d[256 + *state + type];
  return *state;
}

/*

License

Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

/* _pacc_utf8_char attempts to read a single UTF8 encoded character from
 * the string p with length m. Its return value is the number of bytes
 * in the character, 0 in case of error; *c holds the character read. */
static int _pacc_utf8_char(unsigned char *p, int m, uint32_t *c) {
    uint32_t state = PACC_UTF8_ACCEPT;
    int i = 0;
    do
	if (state == PACC_UTF8_REJECT || i == m) return 0;
    while (pacc_decode(&state, c, p[i++]));
    return i;
}

static int _pacc_ref_streq(ref_t a, char *b, struct pacc_parser *p) {
    /* XXX this could be made quicker */
    if (strlen(b) != (size_t)(a[1] - a[0])) return 0;
    return strncmp((const char *)p->string + a[0], b, a[1] - a[0]) == 0; 
}

/* Find the largest available prime which is smaller than the target
 * bucket count. This exponential selection of primes was generated by
 * Dan Bernstein's primegen-0.97 package, using the following:

perl -le '$x=1;while(($e=int(exp($x/1)))<2**32){print "./primes ",$e," ",100+$e," | sed 1q";++$x}' | sh

 * except that I replaced 2 with 3.
 */
static void _pacc_set_bkt_cnt(struct pacc_parser *p) {
    static unsigned int primes[] = {
	3, 7, 23, 59, 149, 409, 1097, 2999, 8111, 22027, 59879, 162779, 442439,
	1202609, 3269029, 8886113, 24154957, 65659969, 178482319, 485165237,
	1318815761, 3584912873U
    };
    int i, p_sz = sizeof(primes) / sizeof(*primes);
    unsigned long bkt_cnt = n_rules * (p->input_length + 1) / 100;

    for (i = 1; i < p_sz; ++i) {
	if (primes[i] > bkt_cnt) break;
    }
    p->m_bkt_cnt = primes[i - 1];
}

struct pacc_parser *PACC_SYM(new)(void) {
    struct pacc_parser *p;

    p = realloc(0, sizeof *p);
    if (!p) pacc_nomem();
    p->m_chain_max = 0;
    p->sp = 0;
    p->stack = p->stack_alloc = 0;
    p->err = 0;
    p->err_alloc = p->err_valid = 0;
    p->coord = 0;
    p->coord_alloc = p->coord_valid = 0;
    return p;
}

void PACC_SYM(input)(struct pacc_parser *p, char *s, size_t l) {
    unsigned int i;

    p->string = (unsigned char *)s;
    p->input_length = l;
    _pacc_set_bkt_cnt(p);
    p->m_bkt = realloc(0, sizeof(struct pacc_mid *) * p->m_bkt_cnt);
    if (!p->m_bkt) pacc_nomem();
    p->m_valid = realloc(0, 2 * p->m_bkt_cnt);
    for (i = 0; i < p->m_bkt_cnt; ++i) {
	p->m_bkt[i] = 0;
	p->m_valid[i * 2] = 0; /* valid */
	p->m_valid[i * 2 + 1] = 0; /* allocated */
    }
}

void PACC_SYM(destroy)(struct pacc_parser *p) {
    free(p->m_bkt);
    free(p->m_valid);
    free(p);
}

/* hash chains */
static struct pacc_mid *_pacc_result(struct pacc_parser *p, size_t col, long rule) {
    unsigned char i;
    unsigned int h;
    struct pacc_mid *bkt, *r;

    assert(col < p->input_length + 1);
    PACC_TRACE fprintf(stderr, "_pacc_result(%ld, %ld)\n", col, rule);
    h = (col + (rule << 6) + (rule << 16) - rule) % p->m_bkt_cnt;
    bkt = p->m_bkt[h];
    for (i = 0; i < p->m_valid[h * 2]; ++i) {
	r = bkt + i;
	if (r->col == col && r->rule >> 4 == rule)
	    return r;
    }
    if (i == p->m_valid[h * 2 + 1]) {
	if (i == 255) pacc_panic("bucket too large");
	if (i + 1 > p->m_chain_max) p->m_chain_max = i + 1;
	p->m_bkt[h] = bkt = realloc(bkt, p->m_chain_max * sizeof(struct pacc_mid));
	if (!bkt) pacc_nomem();
	p->m_valid[h * 2 + 1] = p->m_chain_max;
    }
    r = bkt + i;
    /* Initialize the new element. */
    r->col = col; r->rule = rule << 4 | uncomputed;
    r->evlis = 0;
    r->ev_alloc = r->ev_valid = 0;
    /* Correct use of a side effect in an (unfailing) assert. */
    assert((r->expr_id = 0) == 0);
    ++(p->m_valid[h * 2]);
    return r;
}

static int _pacc_error(struct pacc_parser *_pacc, char *what, size_t col) {
    int doit, append;

    PACC_TRACE fprintf(stderr, "_pacc_error(%s, %ld)\n", what, col);
    append = doit = 1;
    if (col > _pacc->err_col) append = 0;
    else if (col == _pacc->err_col) {
        size_t i;
        for (i = 0; i < _pacc->err_valid; ++i) {
            if (strcmp(_pacc->err[i], what) == 0) doit = 0;
        }
    } else doit = 0;
    if (doit) {
        if (append) ++_pacc->err_valid;
        else _pacc->err_valid = 1;
        if (_pacc->err_valid > _pacc->err_alloc) {
            _pacc->err_alloc = 2 * _pacc->err_alloc + 1;
            _pacc->err = realloc(_pacc->err, _pacc->err_alloc * sizeof(char *));
            if (!_pacc->err) pacc_nomem();
        }
        _pacc->err[_pacc->err_valid - 1] = what;
        _pacc->err_col = col;
    }
    return 0; /* for the fail() macro */
}

/* Given a parser p, and a column col, return the "coordinates" in a
 * pair of ints: coord[0] is the line number, and coord[1] is the column
 * within the line, both 1-based. */
static int *_pacc_coords(struct pacc_parser *p, size_t col) {
    unsigned int i;
    int x, y, hi, lo;
    struct _pacc_coord *c, *cs = p->coord;

    /* binary search */
    lo = 0; hi = p->coord_valid;
    while (lo < hi) {
	int mid = (lo + hi) / 2;
	if (cs[mid].n < col) lo = mid + 1;
	else if (cs[mid].n > col) hi = mid;
	else return cs[mid].c;
    }

    /* increase list if necessary */
    if (p->coord_valid + 1 > p->coord_alloc) {
    	struct _pacc_coord *n;
	p->coord_alloc = p->coord_alloc * 2 + 1;
	n = realloc(p->coord, p->coord_alloc * sizeof *n);
	if (!n) pacc_nomem();
	cs = p->coord = n;
    }

    /* move hole to the right place */
    memmove(cs + lo + 1, cs + lo, (p->coord_valid - lo) * sizeof *cs);
    ++p->coord_valid;

    /* find the coordinates */
    c = cs + lo; c->n = col;
    if (lo > 0) { y = cs[lo - 1].c[0]; i = cs[lo - 1].n; }
    else { y = 1; /* line numbering is 1-based, natch */ i = 0; }
    x = 1;
    while (i < col) {
	uint32_t c;
	int l = _pacc_utf8_char(p->string + i, p->input_length - i, &c);
	if (!l) pacc_panic("invalid UTF-8 input");
	i += l;
	++x;
	if (c == '\n') { ++y; x = 1; }
    }

    /* memoize them */
    c->c[0] = y;
    /* column numbering is also 1-based; start points to \n */
    c->c[1] = x;
    return c->c;
}
/* The userland version */
#define pacc_coords _pacc_coords(_pacc, _x)

static void pacc_pr(char **buf, size_t *l, char *fmt, ...) {
    size_t n;
    va_list argp;

    va_start(argp, fmt);
    n = vsnprintf(0, 0, fmt, argp);
    va_end(argp);

    *buf = realloc(*buf, *l + n + 1);
    if (!buf) pacc_nomem();

    va_start(argp, fmt);
    vsnprintf(*buf + *l, n + 1, fmt, argp);
    va_end(argp);
    *l += n;
}

char *PACC_SYM(pos)(struct pacc_parser *p, const char *s) {
    int *coords;
    size_t l = 0;
    char *r = 0;

    coords = _pacc_coords(p, p->err_col);
    pacc_pr(&r, &l, "%d:%d: %s", coords[0], coords[1], s);

    return r;
}

char *PACC_SYM(error)(struct pacc_parser *p) {
    size_t i, l = 0;
    char *r = 0;

    /* XXX this, or something like it, is very handy, and needs to go
     * in. But we're holding off for now as all failing test cases would
     * need to be changed. */
    //printf("got `%c', ", p->string[p->err_col]); /* XXX UTF-8? */

    if (p->err_valid && p->err[0][0] == '.')
	pacc_pr(&r, &l, "expected ");
    for (i = 0; i < p->err_valid; ++i) {
	char *s = p->err[i];
	if (*s == '.') ++s;
	for ( ; *s; ++s) {
	    if (isprint(*s)) pacc_pr(&r, &l, "%c", *s);
	    else switch (*s) {
		case '\n':
		    pacc_pr(&r, &l, "\\n");
		    break;
		default:
		    pacc_pr(&r, &l, "\\x%02x", *s);
		    break;
	    }
	}

	if (i + 1 < p->err_valid) {
	    pacc_pr(&r, &l, ", ");
	    if (i + 2 == p->err_valid) pacc_pr(&r, &l, "or ");
	}
    }
    return PACC_SYM(pos)(p, r);
    //printf("%s\n", r);
    //printf("(column %ld)\n", _pacc->err_col);
}

int PACC_SYM(parse)(struct pacc_parser *_pacc) {
    enum pacc_status _status;
    int _cont = -1, _st, _pacc_any_i, _evaling = 0;
    size_t _x = 0, _x_rule, _pos, _pacc_ev_i;
    uint32_t _pacc_utf_cp;

    _st=113;
    goto top;
    contin:
    _st=_cont;
    PACC_TRACE fprintf(stderr, "continuing in state %d\n", _cont);
    top:
    PACC_TRACE fprintf(stderr, "switch to state %d\n", _st);
    switch(_st) {
      case 113: /* Expression */
      PACC_TRACE fprintf(stderr, "rule 113 (Expression) col %ld\n", _x);
      _x_rule = _x;
      cur = _pacc_result(_pacc, _x, 113);
      if ((cur->rule & 3) == uncomputed) {
        PACC_TRACE fprintf(stderr, "seq 111 @ col %ld?\n", _x);
        _pacc_Push(_cont);
        _cont = 111;
        _status = parsed;
        _pacc_save_core(217, _x);
        _pacc_Push(_x_rule);
        _pacc_Push(_cont);
        _cont = 103;
        _st = 217;
        /* call _ */
        goto top;
        case 103: /* return from _ */
        _pacc_Pop(_cont);
        _status = cur->rule & 3;
        _x = cur->remainder;
        _pacc_Pop(_x_rule);
        cur = _pacc_result(_pacc, _x_rule, 113);
        if (_status == no_parse) {
          goto contin;
        }
        /* bind: s */
        PACC_TRACE fprintf(stderr, "bind 105 @ col %ld?\n", _x);
        PACC_TRACE fprintf(stderr, "will bind s @ rule 134, col %ld\n", _x);
        _pacc_save_core(134, _x);
        _pacc_Push(_x_rule);
        _pacc_Push(_cont);
        _cont = 104;
        _st = 134;
        /* call Sum */
        goto top;
        case 104: /* return from Sum */
        _pacc_Pop(_cont);
        _status = cur->rule & 3;
        _x = cur->remainder;
        _pacc_Pop(_x_rule);
        cur = _pacc_result(_pacc, _x_rule, 113);
        /* end bind: s */
        if (_status == no_parse) {
          goto contin;
        }
        if (_status == no_parse) {
          goto contin;
        }
        _pacc_save_core(199, _x);
        _pacc_Push(_x_rule);
        _pacc_Push(_cont);
        _cont = 106;
        _st = 199;
        /* call Equals */
        goto top;
        case 106: /* return from Equals */
        _pacc_Pop(_cont);
        _status = cur->rule & 3;
        _x = cur->remainder;
        _pacc_Pop(_x_rule);
        cur = _pacc_result(_pacc, _x_rule, 113);
        if (_status == no_parse) {
          goto contin;
        }
        PACC_TRACE fprintf(stderr, "expr 110 @ col %ld?\n", _x);
        assert(cur->expr_id == 0);
        cur->expr_id = 110;
        case 110:
        if (_evaling) {
          struct pacc_mid *_pacc_p;
          /* i is 1, type is int */
          int s;
          PACC_TRACE fprintf(stderr, "110: evaluating\n");
          _pacc_p = cur = _pacc_result(_pacc, _x, 113);
          _pos = 1;
          PACC_TRACE fprintf(stderr, "binding of s: pos %ld holds col %ld\n", _pos, _pacc_p->evlis[_pos].col);
          PACC_TRACE fprintf(stderr, "bind s to r134 @ c%ld\n", _pacc_p->evlis[_pos].col);
          cur = _pacc_result(_pacc, _pacc_p->evlis[_pos].col, 134);
          if ((cur->rule & 3) != evaluated) {
            _pacc_Push(_x); _pacc_Push(_cont);
            _cont = 108;
            _pacc_ev_i = 0; goto eval_loop;
            case 108:
            _pacc_Pop(_cont); _pacc_Pop(_x);
          }
          s = cur->value.u0;
          PACC_TRACE fprintf(stderr, "bound s to r134 @ c%ld ==> " TYPE_PRINTF "\n", _pacc_p->evlis[_pos].col, cur->value.u0);
          cur = _pacc_p;
          cur->value.u0
#line 1 "icalc.pacc"
                                =(s);
#line 586 "icalc.c"
          PACC_TRACE fprintf(stderr, "stash " TYPE_PRINTF " to (%ld, 113)\n", cur->value.u0, _x);
          goto _pacc_expr_done;
        }
        case 111:
        _pacc_Pop(_cont);
        PACC_TRACE fprintf(stderr, "seq 111 @ col %ld => %s\n", _x_rule, _status!=no_parse?"yes":"no");
        PACC_TRACE fprintf(stderr, "col is %ld\n", _x);
        cur->rule = (cur->rule & ~3) | _status;
        cur->remainder = _x;
        if (_pacc->err_col == _x_rule) {
          _pacc->err_valid = 0;
          _pacc_error(_pacc, ".Expression", _x_rule);
        }
      }
      goto contin;
      case 134: /* Sum */
      PACC_TRACE fprintf(stderr, "rule 134 (Sum) col %ld\n", _x);
      _x_rule = _x;
      cur = _pacc_result(_pacc, _x, 134);
      if ((cur->rule & 3) == uncomputed) {
        PACC_TRACE fprintf(stderr, "alt 132 @ col %ld?\n", _x);
        _pacc_Push(_cont);
        _cont = 132;
        PACC_TRACE fprintf(stderr, "save column registers\n");
        _pacc_Push(_x); _pacc_Push(cur->ev_valid);
        PACC_TRACE fprintf(stderr, "seq 125 @ col %ld?\n", _x);
        _pacc_Push(_cont);
        _cont = 125;
        _status = parsed;
        /* bind: p */
        PACC_TRACE fprintf(stderr, "bind 115 @ col %ld?\n", _x);
        PACC_TRACE fprintf(stderr, "will bind p @ rule 155, col %ld\n", _x);
        _pacc_save_core(155, _x);
        _pacc_Push(_x_rule);
        _pacc_Push(_cont);
        _cont = 114;
        _st = 155;
        /* call Product */
        goto top;
        case 114: /* return from Product */
        _pacc_Pop(_cont);
        _status = cur->rule & 3;
        _x = cur->remainder;
        _pacc_Pop(_x_rule);
        cur = _pacc_result(_pacc, _x_rule, 134);
        /* end bind: p */
        if (_status == no_parse) {
          goto contin;
        }
        if (_status == no_parse) {
          goto contin;
        }
        _pacc_save_core(189, _x);
        _pacc_Push(_x_rule);
        _pacc_Push(_cont);
        _cont = 116;
        _st = 189;
        /* call Plus */
        goto top;
        case 116: /* return from Plus */
        _pacc_Pop(_cont);
        _status = cur->rule & 3;
        _x = cur->remainder;
        _pacc_Pop(_x_rule);
        cur = _pacc_result(_pacc, _x_rule, 134);
        if (_status == no_parse) {
          goto contin;
        }
        if (_status == no_parse) {
          goto contin;
        }
        /* bind: s */
        PACC_TRACE fprintf(stderr, "bind 118 @ col %ld?\n", _x);
        PACC_TRACE fprintf(stderr, "will bind s @ rule 134, col %ld\n", _x);
        _pacc_save_core(134, _x);
        _pacc_Push(_x_rule);
        _pacc_Push(_cont);
        _cont = 117;
        _st = 134;
        /* call Sum */
        goto top;
        case 117: /* return from Sum */
        _pacc_Pop(_cont);
        _status = cur->rule & 3;
        _x = cur->remainder;
        _pacc_Pop(_x_rule);
        cur = _pacc_result(_pacc, _x_rule, 134);
        /* end bind: s */
        if (_status == no_parse) {
          goto contin;
        }
        PACC_TRACE fprintf(stderr, "expr 124 @ col %ld?\n", _x);
        assert(cur->expr_id == 0);
        cur->expr_id = 124;
        case 124:
        if (_evaling) {
          struct pacc_mid *_pacc_p;
          /* i is 0, type is int */
          int p;
          /* i is 2, type is int */
          int s;
          PACC_TRACE fprintf(stderr, "124: evaluating\n");
          _pacc_p = cur = _pacc_result(_pacc, _x, 134);
          _pos = 0;
          PACC_TRACE fprintf(stderr, "binding of p: pos %ld holds col %ld\n", _pos, _pacc_p->evlis[_pos].col);
          PACC_TRACE fprintf(stderr, "bind p to r155 @ c%ld\n", _pacc_p->evlis[_pos].col);
          cur = _pacc_result(_pacc, _pacc_p->evlis[_pos].col, 155);
          if ((cur->rule & 3) != evaluated) {
            _pacc_Push(_x); _pacc_Push(_cont);
            _cont = 122;
            _pacc_ev_i = 0; goto eval_loop;
            case 122:
            _pacc_Pop(_cont); _pacc_Pop(_x);
          }
          p = cur->value.u0;
          PACC_TRACE fprintf(stderr, "bound p to r155 @ c%ld ==> " TYPE_PRINTF "\n", _pacc_p->evlis[_pos].col, cur->value.u0);
          _pos = 2;
          PACC_TRACE fprintf(stderr, "binding of s: pos %ld holds col %ld\n", _pos, _pacc_p->evlis[_pos].col);
          PACC_TRACE fprintf(stderr, "bind s to r134 @ c%ld\n", _pacc_p->evlis[_pos].col);
          cur = _pacc_result(_pacc, _pacc_p->evlis[_pos].col, 134);
          if ((cur->rule & 3) != evaluated) {
            _pacc_Push(_x); _pacc_Push(_cont);
            _cont = 121;
            _pacc_ev_i = 0; goto eval_loop;
            case 121:
            _pacc_Pop(_cont); _pacc_Pop(_x);
          }
          s = cur->value.u0;
          PACC_TRACE fprintf(stderr, "bound s to r134 @ c%ld ==> " TYPE_PRINTF "\n", _pacc_p->evlis[_pos].col, cur->value.u0);
          cur = _pacc_p;
          cur->value.u0
#line 2 "icalc.pacc"
                                  =( p + s );
#line 720 "icalc.c"
          PACC_TRACE fprintf(stderr, "stash " TYPE_PRINTF " to (%ld, 134)\n", cur->value.u0, _x);
          goto _pacc_expr_done;
        }
        case 125:
        _pacc_Pop(_cont);
        PACC_TRACE fprintf(stderr, "seq 125 @ col %ld => %s\n", _x_rule, _status!=no_parse?"yes":"no");
        PACC_TRACE fprintf(stderr, "col is %ld\n", _x);
        PACC_TRACE fprintf(stderr, "alt 132 @ col %ld => %s\n", _x, _status!=no_parse?"yes":"no");
        if (_status != no_parse) {
          PACC_TRACE fprintf(stderr, "accept column registers\n");
          _pacc_Discard(cur->ev_valid); _pacc_Discard(_x);
          goto contin;
        }
        PACC_TRACE fprintf(stderr, "restore column registers\n");
        _pacc_Pop(cur->ev_valid); _pacc_Pop(_x);
        PACC_TRACE fprintf(stderr, "save column registers\n");
        _pacc_Push(_x); _pacc_Push(cur->ev_valid);
        PACC_TRACE fprintf(stderr, "col restored to %ld\n", _x);
        PACC_TRACE fprintf(stderr, "alt 132 @ col %ld? (next alternative)\n", _x);
        PACC_TRACE fprintf(stderr, "seq 131 @ col %ld?\n", _x);
        _pacc_Push(_cont);
        _cont = 131;
        _status = parsed;
        /* bind: p */
        PACC_TRACE fprintf(stderr, "bind 127 @ col %ld?\n", _x);
        PACC_TRACE fprintf(stderr, "will bind p @ rule 155, col %ld\n", _x);
        _pacc_save_core(155, _x);
        _pacc_Push(_x_rule);
        _pacc_Push(_cont);
        _cont = 126;
        _st = 155;
        /* call Product */
        goto top;
        case 126: /* return from Product */
        _pacc_Pop(_cont);
        _status = cur->rule & 3;
        _x = cur->remainder;
        _pacc_Pop(_x_rule);
        cur = _pacc_result(_pacc, _x_rule, 134);
        /* end bind: p */
        if (_status == no_parse) {
          goto contin;
        }
        PACC_TRACE fprintf(stderr, "expr 130 @ col %ld?\n", _x);
        assert(cur->expr_id == 0);
        cur->expr_id = 130;
        case 130:
        if (_evaling) {
          struct pacc_mid *_pacc_p;
          /* i is 0, type is int */
          int p;
          PACC_TRACE fprintf(stderr, "130: evaluating\n");
          _pacc_p = cur = _pacc_result(_pacc, _x, 134);
          _pos = 0;
          PACC_TRACE fprintf(stderr, "binding of p: pos %ld holds col %ld\n", _pos, _pacc_p->evlis[_pos].col);
          PACC_TRACE fprintf(stderr, "bind p to r155 @ c%ld\n", _pacc_p->evlis[_pos].col);
          cur = _pacc_result(_pacc, _pacc_p->evlis[_pos].col, 155);
          if ((cur->rule & 3) != evaluated) {
            _pacc_Push(_x); _pacc_Push(_cont);
            _cont = 128;
            _pacc_ev_i = 0; goto eval_loop;
            case 128:
            _pacc_Pop(_cont); _pacc_Pop(_x);
          }
          p = cur->value.u0;
          PACC_TRACE fprintf(stderr, "bound p to r155 @ c%ld ==> " TYPE_PRINTF "\n", _pacc_p->evlis[_pos].col, cur->value.u0);
          cur = _pacc_p;
          cur->value.u0
#line 3 "icalc.pacc"
                  =(p);
#line 791 "icalc.c"
          PACC_TRACE fprintf(stderr, "stash " TYPE_PRINTF " to (%ld, 134)\n", cur->value.u0, _x);
          goto _pacc_expr_done;
        }
        case 131:
        _pacc_Pop(_cont);
        PACC_TRACE fprintf(stderr, "seq 131 @ col %ld => %s\n", _x_rule, _status!=no_parse?"yes":"no");
        PACC_TRACE fprintf(stderr, "col is %ld\n", _x);
        if (_status == no_parse) {
          PACC_TRACE fprintf(stderr, "restore column registers\n");
          _pacc_Pop(cur->ev_valid); _pacc_Pop(_x);
        }
        else {
          PACC_TRACE fprintf(stderr, "accept column registers\n");
          _pacc_Discard(cur->ev_valid); _pacc_Discard(_x);
        }
        case 132:
        _pacc_Pop(_cont);
        PACC_TRACE fprintf(stderr, "alt 132 @ col %ld => %s\n", _x, _status!=no_parse?"yes":"no");
        PACC_TRACE fprintf(stderr, "col is %ld\n", _x);
        cur->rule = (cur->rule & ~3) | _status;
        cur->remainder = _x;
        if (_pacc->err_col == _x_rule) {
          _pacc->err_valid = 0;
          _pacc_error(_pacc, ".Sum", _x_rule);
        }
      }
      goto contin;
      case 155: /* Product */
      PACC_TRACE fprintf(stderr, "rule 155 (Product) col %ld\n", _x);
      _x_rule = _x;
      cur = _pacc_result(_pacc, _x, 155);
      if ((cur->rule & 3) == uncomputed) {
        PACC_TRACE fprintf(stderr, "alt 153 @ col %ld?\n", _x);
        _pacc_Push(_cont);
        _cont = 153;
        PACC_TRACE fprintf(stderr, "save column registers\n");
        _pacc_Push(_x); _pacc_Push(cur->ev_valid);
        PACC_TRACE fprintf(stderr, "seq 146 @ col %ld?\n", _x);
        _pacc_Push(_cont);
        _cont = 146;
        _status = parsed;
        /* bind: t */
        PACC_TRACE fprintf(stderr, "bind 136 @ col %ld?\n", _x);
        PACC_TRACE fprintf(stderr, "will bind t @ rule 174, col %ld\n", _x);
        _pacc_save_core(174, _x);
        _pacc_Push(_x_rule);
        _pacc_Push(_cont);
        _cont = 135;
        _st = 174;
        /* call Term */
        goto top;
        case 135: /* return from Term */
        _pacc_Pop(_cont);
        _status = cur->rule & 3;
        _x = cur->remainder;
        _pacc_Pop(_x_rule);
        cur = _pacc_result(_pacc, _x_rule, 155);
        /* end bind: t */
        if (_status == no_parse) {
          goto contin;
        }
        if (_status == no_parse) {
          goto contin;
        }
        _pacc_save_core(194, _x);
        _pacc_Push(_x_rule);
        _pacc_Push(_cont);
        _cont = 137;
        _st = 194;
        /* call Star */
        goto top;
        case 137: /* return from Star */
        _pacc_Pop(_cont);
        _status = cur->rule & 3;
        _x = cur->remainder;
        _pacc_Pop(_x_rule);
        cur = _pacc_result(_pacc, _x_rule, 155);
        if (_status == no_parse) {
          goto contin;
        }
        if (_status == no_parse) {
          goto contin;
        }
        /* bind: p */
        PACC_TRACE fprintf(stderr, "bind 139 @ col %ld?\n", _x);
        PACC_TRACE fprintf(stderr, "will bind p @ rule 155, col %ld\n", _x);
        _pacc_save_core(155, _x);
        _pacc_Push(_x_rule);
        _pacc_Push(_cont);
        _cont = 138;
        _st = 155;
        /* call Product */
        goto top;
        case 138: /* return from Product */
        _pacc_Pop(_cont);
        _status = cur->rule & 3;
        _x = cur->remainder;
        _pacc_Pop(_x_rule);
        cur = _pacc_result(_pacc, _x_rule, 155);
        /* end bind: p */
        if (_status == no_parse) {
          goto contin;
        }
        PACC_TRACE fprintf(stderr, "expr 145 @ col %ld?\n", _x);
        assert(cur->expr_id == 0);
        cur->expr_id = 145;
        case 145:
        if (_evaling) {
          struct pacc_mid *_pacc_p;
          /* i is 0, type is int */
          int t;
          /* i is 2, type is int */
          int p;
          PACC_TRACE fprintf(stderr, "145: evaluating\n");
          _pacc_p = cur = _pacc_result(_pacc, _x, 155);
          _pos = 0;
          PACC_TRACE fprintf(stderr, "binding of t: pos %ld holds col %ld\n", _pos, _pacc_p->evlis[_pos].col);
          PACC_TRACE fprintf(stderr, "bind t to r174 @ c%ld\n", _pacc_p->evlis[_pos].col);
          cur = _pacc_result(_pacc, _pacc_p->evlis[_pos].col, 174);
          if ((cur->rule & 3) != evaluated) {
            _pacc_Push(_x); _pacc_Push(_cont);
            _cont = 143;
            _pacc_ev_i = 0; goto eval_loop;
            case 143:
            _pacc_Pop(_cont); _pacc_Pop(_x);
          }
          t = cur->value.u0;
          PACC_TRACE fprintf(stderr, "bound t to r174 @ c%ld ==> " TYPE_PRINTF "\n", _pacc_p->evlis[_pos].col, cur->value.u0);
          _pos = 2;
          PACC_TRACE fprintf(stderr, "binding of p: pos %ld holds col %ld\n", _pos, _pacc_p->evlis[_pos].col);
          PACC_TRACE fprintf(stderr, "bind p to r155 @ c%ld\n", _pacc_p->evlis[_pos].col);
          cur = _pacc_result(_pacc, _pacc_p->evlis[_pos].col, 155);
          if ((cur->rule & 3) != evaluated) {
            _pacc_Push(_x); _pacc_Push(_cont);
            _cont = 142;
            _pacc_ev_i = 0; goto eval_loop;
            case 142:
            _pacc_Pop(_cont); _pacc_Pop(_x);
          }
          p = cur->value.u0;
          PACC_TRACE fprintf(stderr, "bound p to r155 @ c%ld ==> " TYPE_PRINTF "\n", _pacc_p->evlis[_pos].col, cur->value.u0);
          cur = _pacc_p;
          cur->value.u0
#line 4 "icalc.pacc"
                                       =( t * p );
#line 937 "icalc.c"
          PACC_TRACE fprintf(stderr, "stash " TYPE_PRINTF " to (%ld, 155)\n", cur->value.u0, _x);
          goto _pacc_expr_done;
        }
        case 146:
        _pacc_Pop(_cont);
        PACC_TRACE fprintf(stderr, "seq 146 @ col %ld => %s\n", _x_rule, _status!=no_parse?"yes":"no");
        PACC_TRACE fprintf(stderr, "col is %ld\n", _x);
        PACC_TRACE fprintf(stderr, "alt 153 @ col %ld => %s\n", _x, _status!=no_parse?"yes":"no");
        if (_status != no_parse) {
          PACC_TRACE fprintf(stderr, "accept column registers\n");
          _pacc_Discard(cur->ev_valid); _pacc_Discard(_x);
          goto contin;
        }
        PACC_TRACE fprintf(stderr, "restore column registers\n");
        _pacc_Pop(cur->ev_valid); _pacc_Pop(_x);
        PACC_TRACE fprintf(stderr, "save column registers\n");
        _pacc_Push(_x); _pacc_Push(cur->ev_valid);
        PACC_TRACE fprintf(stderr, "col restored to %ld\n", _x);
        PACC_TRACE fprintf(stderr, "alt 153 @ col %ld? (next alternative)\n", _x);
        PACC_TRACE fprintf(stderr, "seq 152 @ col %ld?\n", _x);
        _pacc_Push(_cont);
        _cont = 152;
        _status = parsed;
        /* bind: t */
        PACC_TRACE fprintf(stderr, "bind 148 @ col %ld?\n", _x);
        PACC_TRACE fprintf(stderr, "will bind t @ rule 174, col %ld\n", _x);
        _pacc_save_core(174, _x);
        _pacc_Push(_x_rule);
        _pacc_Push(_cont);
        _cont = 147;
        _st = 174;
        /* call Term */
        goto top;
        case 147: /* return from Term */
        _pacc_Pop(_cont);
        _status = cur->rule & 3;
        _x = cur->remainder;
        _pacc_Pop(_x_rule);
        cur = _pacc_result(_pacc, _x_rule, 155);
        /* end bind: t */
        if (_status == no_parse) {
          goto contin;
        }
        PACC_TRACE fprintf(stderr, "expr 151 @ col %ld?\n", _x);
        assert(cur->expr_id == 0);
        cur->expr_id = 151;
        case 151:
        if (_evaling) {
          struct pacc_mid *_pacc_p;
          /* i is 0, type is int */
          int t;
          PACC_TRACE fprintf(stderr, "151: evaluating\n");
          _pacc_p = cur = _pacc_result(_pacc, _x, 155);
          _pos = 0;
          PACC_TRACE fprintf(stderr, "binding of t: pos %ld holds col %ld\n", _pos, _pacc_p->evlis[_pos].col);
          PACC_TRACE fprintf(stderr, "bind t to r174 @ c%ld\n", _pacc_p->evlis[_pos].col);
          cur = _pacc_result(_pacc, _pacc_p->evlis[_pos].col, 174);
          if ((cur->rule & 3) != evaluated) {
            _pacc_Push(_x); _pacc_Push(_cont);
            _cont = 149;
            _pacc_ev_i = 0; goto eval_loop;
            case 149:
            _pacc_Pop(_cont); _pacc_Pop(_x);
          }
          t = cur->value.u0;
          PACC_TRACE fprintf(stderr, "bound t to r174 @ c%ld ==> " TYPE_PRINTF "\n", _pacc_p->evlis[_pos].col, cur->value.u0);
          cur = _pacc_p;
          cur->value.u0
#line 5 "icalc.pacc"
                   =(t);
#line 1008 "icalc.c"
          PACC_TRACE fprintf(stderr, "stash " TYPE_PRINTF " to (%ld, 155)\n", cur->value.u0, _x);
          goto _pacc_expr_done;
        }
        case 152:
        _pacc_Pop(_cont);
        PACC_TRACE fprintf(stderr, "seq 152 @ col %ld => %s\n", _x_rule, _status!=no_parse?"yes":"no");
        PACC_TRACE fprintf(stderr, "col is %ld\n", _x);
        if (_status == no_parse) {
          PACC_TRACE fprintf(stderr, "restore column registers\n");
          _pacc_Pop(cur->ev_valid); _pacc_Pop(_x);
        }
        else {
          PACC_TRACE fprintf(stderr, "accept column registers\n");
          _pacc_Discard(cur->ev_valid); _pacc_Discard(_x);
        }
        case 153:
        _pacc_Pop(_cont);
        PACC_TRACE fprintf(stderr, "alt 153 @ col %ld => %s\n", _x, _status!=no_parse?"yes":"no");
        PACC_TRACE fprintf(stderr, "col is %ld\n", _x);
        cur->rule = (cur->rule & ~3) | _status;
        cur->remainder = _x;
        if (_pacc->err_col == _x_rule) {
          _pacc->err_valid = 0;
          _pacc_error(_pacc, ".Product", _x_rule);
        }
      }
      goto contin;
      case 174: /* Term */
      PACC_TRACE fprintf(stderr, "rule 174 (Term) col %ld\n", _x);
      _x_rule = _x;
      cur = _pacc_result(_pacc, _x, 174);
      if ((cur->rule & 3) == uncomputed) {
        PACC_TRACE fprintf(stderr, "alt 172 @ col %ld?\n", _x);
        _pacc_Push(_cont);
        _cont = 172;
        PACC_TRACE fprintf(stderr, "save column registers\n");
        _pacc_Push(_x); _pacc_Push(cur->ev_valid);
        PACC_TRACE fprintf(stderr, "seq 161 @ col %ld?\n", _x);
        _pacc_Push(_cont);
        _cont = 161;
        _status = parsed;
        /* bind: d */
        PACC_TRACE fprintf(stderr, "bind 157 @ col %ld?\n", _x);
        PACC_TRACE fprintf(stderr, "will bind d @ rule 184, col %ld\n", _x);
        _pacc_save_core(184, _x);
        _pacc_Push(_x_rule);
        _pacc_Push(_cont);
        _cont = 156;
        _st = 184;
        /* call Decimal */
        goto top;
        case 156: /* return from Decimal */
        _pacc_Pop(_cont);
        _status = cur->rule & 3;
        _x = cur->remainder;
        _pacc_Pop(_x_rule);
        cur = _pacc_result(_pacc, _x_rule, 174);
        /* end bind: d */
        if (_status == no_parse) {
          goto contin;
        }
        PACC_TRACE fprintf(stderr, "expr 160 @ col %ld?\n", _x);
        assert(cur->expr_id == 0);
        cur->expr_id = 160;
        case 160:
        if (_evaling) {
          struct pacc_mid *_pacc_p;
          /* i is 0, type is int */
          int d;
          PACC_TRACE fprintf(stderr, "160: evaluating\n");
          _pacc_p = cur = _pacc_result(_pacc, _x, 174);
          _pos = 0;
          PACC_TRACE fprintf(stderr, "binding of d: pos %ld holds col %ld\n", _pos, _pacc_p->evlis[_pos].col);
          PACC_TRACE fprintf(stderr, "bind d to r184 @ c%ld\n", _pacc_p->evlis[_pos].col);
          cur = _pacc_result(_pacc, _pacc_p->evlis[_pos].col, 184);
          if ((cur->rule & 3) != evaluated) {
            _pacc_Push(_x); _pacc_Push(_cont);
            _cont = 158;
            _pacc_ev_i = 0; goto eval_loop;
            case 158:
            _pacc_Pop(_cont); _pacc_Pop(_x);
          }
          d = cur->value.u0;
          PACC_TRACE fprintf(stderr, "bound d to r184 @ c%ld ==> " TYPE_PRINTF "\n", _pacc_p->evlis[_pos].col, cur->value.u0);
          cur = _pacc_p;
          cur->value.u0
#line 6 "icalc.pacc"
                   =(d);
#line 1097 "icalc.c"
          PACC_TRACE fprintf(stderr, "stash " TYPE_PRINTF " to (%ld, 174)\n", cur->value.u0, _x);
          goto _pacc_expr_done;
        }
        case 161:
        _pacc_Pop(_cont);
        PACC_TRACE fprintf(stderr, "seq 161 @ col %ld => %s\n", _x_rule, _status!=no_parse?"yes":"no");
        PACC_TRACE fprintf(stderr, "col is %ld\n", _x);
        PACC_TRACE fprintf(stderr, "alt 172 @ col %ld => %s\n", _x, _status!=no_parse?"yes":"no");
        if (_status != no_parse) {
          PACC_TRACE fprintf(stderr, "accept column registers\n");
          _pacc_Discard(cur->ev_valid); _pacc_Discard(_x);
          goto contin;
        }
        PACC_TRACE fprintf(stderr, "restore column registers\n");
        _pacc_Pop(cur->ev_valid); _pacc_Pop(_x);
        PACC_TRACE fprintf(stderr, "save column registers\n");
        _pacc_Push(_x); _pacc_Push(cur->ev_valid);
        PACC_TRACE fprintf(stderr, "col restored to %ld\n", _x);
        PACC_TRACE fprintf(stderr, "alt 172 @ col %ld? (next alternative)\n", _x);
        PACC_TRACE fprintf(stderr, "seq 171 @ col %ld?\n", _x);
        _pacc_Push(_cont);
        _cont = 171;
        _status = parsed;
        _pacc_save_core(204, _x);
        _pacc_Push(_x_rule);
        _pacc_Push(_cont);
        _cont = 162;
        _st = 204;
        /* call lBrace */
        goto top;
        case 162: /* return from lBrace */
        _pacc_Pop(_cont);
        _status = cur->rule & 3;
        _x = cur->remainder;
        _pacc_Pop(_x_rule);
        cur = _pacc_result(_pacc, _x_rule, 174);
        if (_status == no_parse) {
          goto contin;
        }
        if (_status == no_parse) {
          goto contin;
        }
        /* bind: s */
        PACC_TRACE fprintf(stderr, "bind 164 @ col %ld?\n", _x);
        PACC_TRACE fprintf(stderr, "will bind s @ rule 134, col %ld\n", _x);
        _pacc_save_core(134, _x);
        _pacc_Push(_x_rule);
        _pacc_Push(_cont);
        _cont = 163;
        _st = 134;
        /* call Sum */
        goto top;
        case 163: /* return from Sum */
        _pacc_Pop(_cont);
        _status = cur->rule & 3;
        _x = cur->remainder;
        _pacc_Pop(_x_rule);
        cur = _pacc_result(_pacc, _x_rule, 174);
        /* end bind: s */
        if (_status == no_parse) {
          goto contin;
        }
        if (_status == no_parse) {
          goto contin;
        }
        _pacc_save_core(209, _x);
        _pacc_Push(_x_rule);
        _pacc_Push(_cont);
        _cont = 165;
        _st = 209;
        /* call rBrace */
        goto top;
        case 165: /* return from rBrace */
        _pacc_Pop(_cont);
        _status = cur->rule & 3;
        _x = cur->remainder;
        _pacc_Pop(_x_rule);
        cur = _pacc_result(_pacc, _x_rule, 174);
        if (_status == no_parse) {
          goto contin;
        }
        PACC_TRACE fprintf(stderr, "expr 170 @ col %ld?\n", _x);
        assert(cur->expr_id == 0);
        cur->expr_id = 170;
        case 170:
        if (_evaling) {
          struct pacc_mid *_pacc_p;
          /* i is 1, type is int */
          int s;
          PACC_TRACE fprintf(stderr, "170: evaluating\n");
          _pacc_p = cur = _pacc_result(_pacc, _x, 174);
          _pos = 1;
          PACC_TRACE fprintf(stderr, "binding of s: pos %ld holds col %ld\n", _pos, _pacc_p->evlis[_pos].col);
          PACC_TRACE fprintf(stderr, "bind s to r134 @ c%ld\n", _pacc_p->evlis[_pos].col);
          cur = _pacc_result(_pacc, _pacc_p->evlis[_pos].col, 134);
          if ((cur->rule & 3) != evaluated) {
            _pacc_Push(_x); _pacc_Push(_cont);
            _cont = 168;
            _pacc_ev_i = 0; goto eval_loop;
            case 168:
            _pacc_Pop(_cont); _pacc_Pop(_x);
          }
          s = cur->value.u0;
          PACC_TRACE fprintf(stderr, "bound s to r134 @ c%ld ==> " TYPE_PRINTF "\n", _pacc_p->evlis[_pos].col, cur->value.u0);
          cur = _pacc_p;
          cur->value.u0
#line 6 "icalc.pacc"
                             =(s);
#line 1206 "icalc.c"
          PACC_TRACE fprintf(stderr, "stash " TYPE_PRINTF " to (%ld, 174)\n", cur->value.u0, _x);
          goto _pacc_expr_done;
        }
        case 171:
        _pacc_Pop(_cont);
        PACC_TRACE fprintf(stderr, "seq 171 @ col %ld => %s\n", _x_rule, _status!=no_parse?"yes":"no");
        PACC_TRACE fprintf(stderr, "col is %ld\n", _x);
        if (_status == no_parse) {
          PACC_TRACE fprintf(stderr, "restore column registers\n");
          _pacc_Pop(cur->ev_valid); _pacc_Pop(_x);
        }
        else {
          PACC_TRACE fprintf(stderr, "accept column registers\n");
          _pacc_Discard(cur->ev_valid); _pacc_Discard(_x);
        }
        case 172:
        _pacc_Pop(_cont);
        PACC_TRACE fprintf(stderr, "alt 172 @ col %ld => %s\n", _x, _status!=no_parse?"yes":"no");
        PACC_TRACE fprintf(stderr, "col is %ld\n", _x);
        cur->rule = (cur->rule & ~3) | _status;
        cur->remainder = _x;
        if (_pacc->err_col == _x_rule) {
          _pacc->err_valid = 0;
          _pacc_error(_pacc, ".Term", _x_rule);
        }
      }
      goto contin;
      case 184: /* Decimal */
      PACC_TRACE fprintf(stderr, "rule 184 (Decimal) col %ld\n", _x);
      _x_rule = _x;
      cur = _pacc_result(_pacc, _x, 184);
      if ((cur->rule & 3) == uncomputed) {
        PACC_TRACE fprintf(stderr, "seq 182 @ col %ld?\n", _x);
        _pacc_Push(_cont);
        _cont = 182;
        _status = parsed;
        _pacc_save_core(228, _x);
        _pacc_Push(_x_rule);
        _pacc_Push(_cont);
        _cont = 176;
        _st = 228;
        /* call Decimal:*:0 */
        goto top;
        case 176: /* return from Decimal:*:0 */
        _pacc_Pop(_cont);
        _status = cur->rule & 3;
        _x = cur->remainder;
        _pacc_Pop(_x_rule);
        cur = _pacc_result(_pacc, _x_rule, 184);
        if (_status == no_parse) {
          goto contin;
        }
        _pacc_save_core(217, _x);
        _pacc_Push(_x_rule);
        _pacc_Push(_cont);
        _cont = 177;
        _st = 217;
        /* call _ */
        goto top;
        case 177: /* return from _ */
        _pacc_Pop(_cont);
        _status = cur->rule & 3;
        _x = cur->remainder;
        _pacc_Pop(_x_rule);
        cur = _pacc_result(_pacc, _x_rule, 184);
        if (_status == no_parse) {
          goto contin;
        }
        PACC_TRACE fprintf(stderr, "expr 181 @ col %ld?\n", _x);
        assert(cur->expr_id == 0);
        cur->expr_id = 181;
        case 181:
        if (_evaling) {
          struct pacc_mid *_pacc_p;
          PACC_TRACE fprintf(stderr, "181: evaluating\n");
          _pacc_p = cur = _pacc_result(_pacc, _x, 184);
          cur = _pacc_p;
          cur->value.u0
#line 8 "icalc.pacc"
                      =( atoi(ref_str()) );
#line 1287 "icalc.c"
          PACC_TRACE fprintf(stderr, "stash " TYPE_PRINTF " to (%ld, 184)\n", cur->value.u0, _x);
          goto _pacc_expr_done;
        }
        case 182:
        _pacc_Pop(_cont);
        PACC_TRACE fprintf(stderr, "seq 182 @ col %ld => %s\n", _x_rule, _status!=no_parse?"yes":"no");
        PACC_TRACE fprintf(stderr, "col is %ld\n", _x);
        cur->rule = (cur->rule & ~3) | _status;
        cur->remainder = _x;
        if (_pacc->err_col == _x_rule) {
          _pacc->err_valid = 0;
          _pacc_error(_pacc, ".Decimal", _x_rule);
        }
      }
      goto contin;
      case 189: /* Plus */
      PACC_TRACE fprintf(stderr, "rule 189 (Plus) col %ld\n", _x);
      _x_rule = _x;
      cur = _pacc_result(_pacc, _x, 189);
      if ((cur->rule & 3) == uncomputed) {
        PACC_TRACE fprintf(stderr, "seq 187 @ col %ld?\n", _x);
        _pacc_Push(_cont);
        _cont = 187;
        _status = parsed;
        PACC_TRACE fprintf(stderr, "lit 185 @ col %ld?\n", _x);
        PACC_TRACE fprintf(stderr, "lit 185 @ col %ld => ", _x);
        if (_x+1 <= _pacc->input_length && memcmp("+", _pacc->string + _x, 1) == 0) {
          _status = parsed;
          _x += 1;
        }
        else {
          _pacc_error(_pacc, ".\"+\"", _x);
          _status = no_parse;
        }
        PACC_TRACE fprintf(stderr, "lit 185 %ld => %s\n", _x, _status != no_parse ? "yes" : "no");
        if (_status == no_parse) {
          goto contin;
        }
        _pacc_save_core(217, _x);
        _pacc_Push(_x_rule);
        _pacc_Push(_cont);
        _cont = 186;
        _st = 217;
        /* call _ */
        goto top;
        case 186: /* return from _ */
        _pacc_Pop(_cont);
        _status = cur->rule & 3;
        _x = cur->remainder;
        _pacc_Pop(_x_rule);
        cur = _pacc_result(_pacc, _x_rule, 189);
        case 187:
        _pacc_Pop(_cont);
        PACC_TRACE fprintf(stderr, "seq 187 @ col %ld => %s\n", _x_rule, _status!=no_parse?"yes":"no");
        PACC_TRACE fprintf(stderr, "col is %ld\n", _x);
        cur->rule = (cur->rule & ~3) | _status;
        cur->remainder = _x;
        if (_pacc->err_col == _x_rule) {
          _pacc->err_valid = 0;
          _pacc_error(_pacc, ".Plus", _x_rule);
        }
      }
      goto contin;
      case 194: /* Star */
      PACC_TRACE fprintf(stderr, "rule 194 (Star) col %ld\n", _x);
      _x_rule = _x;
      cur = _pacc_result(_pacc, _x, 194);
      if ((cur->rule & 3) == uncomputed) {
        PACC_TRACE fprintf(stderr, "seq 192 @ col %ld?\n", _x);
        _pacc_Push(_cont);
        _cont = 192;
        _status = parsed;
        PACC_TRACE fprintf(stderr, "lit 190 @ col %ld?\n", _x);
        PACC_TRACE fprintf(stderr, "lit 190 @ col %ld => ", _x);
        if (_x+1 <= _pacc->input_length && memcmp("*", _pacc->string + _x, 1) == 0) {
          _status = parsed;
          _x += 1;
        }
        else {
          _pacc_error(_pacc, ".\"*\"", _x);
          _status = no_parse;
        }
        PACC_TRACE fprintf(stderr, "lit 190 %ld => %s\n", _x, _status != no_parse ? "yes" : "no");
        if (_status == no_parse) {
          goto contin;
        }
        _pacc_save_core(217, _x);
        _pacc_Push(_x_rule);
        _pacc_Push(_cont);
        _cont = 191;
        _st = 217;
        /* call _ */
        goto top;
        case 191: /* return from _ */
        _pacc_Pop(_cont);
        _status = cur->rule & 3;
        _x = cur->remainder;
        _pacc_Pop(_x_rule);
        cur = _pacc_result(_pacc, _x_rule, 194);
        case 192:
        _pacc_Pop(_cont);
        PACC_TRACE fprintf(stderr, "seq 192 @ col %ld => %s\n", _x_rule, _status!=no_parse?"yes":"no");
        PACC_TRACE fprintf(stderr, "col is %ld\n", _x);
        cur->rule = (cur->rule & ~3) | _status;
        cur->remainder = _x;
        if (_pacc->err_col == _x_rule) {
          _pacc->err_valid = 0;
          _pacc_error(_pacc, ".Star", _x_rule);
        }
      }
      goto contin;
      case 199: /* Equals */
      PACC_TRACE fprintf(stderr, "rule 199 (Equals) col %ld\n", _x);
      _x_rule = _x;
      cur = _pacc_result(_pacc, _x, 199);
      if ((cur->rule & 3) == uncomputed) {
        PACC_TRACE fprintf(stderr, "seq 197 @ col %ld?\n", _x);
        _pacc_Push(_cont);
        _cont = 197;
        _status = parsed;
        PACC_TRACE fprintf(stderr, "lit 195 @ col %ld?\n", _x);
        PACC_TRACE fprintf(stderr, "lit 195 @ col %ld => ", _x);
        if (_x+1 <= _pacc->input_length && memcmp("=", _pacc->string + _x, 1) == 0) {
          _status = parsed;
          _x += 1;
        }
        else {
          _pacc_error(_pacc, ".\"=\"", _x);
          _status = no_parse;
        }
        PACC_TRACE fprintf(stderr, "lit 195 %ld => %s\n", _x, _status != no_parse ? "yes" : "no");
        if (_status == no_parse) {
          goto contin;
        }
        _pacc_save_core(217, _x);
        _pacc_Push(_x_rule);
        _pacc_Push(_cont);
        _cont = 196;
        _st = 217;
        /* call _ */
        goto top;
        case 196: /* return from _ */
        _pacc_Pop(_cont);
        _status = cur->rule & 3;
        _x = cur->remainder;
        _pacc_Pop(_x_rule);
        cur = _pacc_result(_pacc, _x_rule, 199);
        case 197:
        _pacc_Pop(_cont);
        PACC_TRACE fprintf(stderr, "seq 197 @ col %ld => %s\n", _x_rule, _status!=no_parse?"yes":"no");
        PACC_TRACE fprintf(stderr, "col is %ld\n", _x);
        cur->rule = (cur->rule & ~3) | _status;
        cur->remainder = _x;
        if (_pacc->err_col == _x_rule) {
          _pacc->err_valid = 0;
          _pacc_error(_pacc, ".Equals", _x_rule);
        }
      }
      goto contin;
      case 204: /* lBrace */
      PACC_TRACE fprintf(stderr, "rule 204 (lBrace) col %ld\n", _x);
      _x_rule = _x;
      cur = _pacc_result(_pacc, _x, 204);
      if ((cur->rule & 3) == uncomputed) {
        PACC_TRACE fprintf(stderr, "seq 202 @ col %ld?\n", _x);
        _pacc_Push(_cont);
        _cont = 202;
        _status = parsed;
        PACC_TRACE fprintf(stderr, "lit 200 @ col %ld?\n", _x);
        PACC_TRACE fprintf(stderr, "lit 200 @ col %ld => ", _x);
        if (_x+1 <= _pacc->input_length && memcmp("(", _pacc->string + _x, 1) == 0) {
          _status = parsed;
          _x += 1;
        }
        else {
          _pacc_error(_pacc, ".\"(\"", _x);
          _status = no_parse;
        }
        PACC_TRACE fprintf(stderr, "lit 200 %ld => %s\n", _x, _status != no_parse ? "yes" : "no");
        if (_status == no_parse) {
          goto contin;
        }
        _pacc_save_core(217, _x);
        _pacc_Push(_x_rule);
        _pacc_Push(_cont);
        _cont = 201;
        _st = 217;
        /* call _ */
        goto top;
        case 201: /* return from _ */
        _pacc_Pop(_cont);
        _status = cur->rule & 3;
        _x = cur->remainder;
        _pacc_Pop(_x_rule);
        cur = _pacc_result(_pacc, _x_rule, 204);
        case 202:
        _pacc_Pop(_cont);
        PACC_TRACE fprintf(stderr, "seq 202 @ col %ld => %s\n", _x_rule, _status!=no_parse?"yes":"no");
        PACC_TRACE fprintf(stderr, "col is %ld\n", _x);
        cur->rule = (cur->rule & ~3) | _status;
        cur->remainder = _x;
        if (_pacc->err_col == _x_rule) {
          _pacc->err_valid = 0;
          _pacc_error(_pacc, ".lBrace", _x_rule);
        }
      }
      goto contin;
      case 209: /* rBrace */
      PACC_TRACE fprintf(stderr, "rule 209 (rBrace) col %ld\n", _x);
      _x_rule = _x;
      cur = _pacc_result(_pacc, _x, 209);
      if ((cur->rule & 3) == uncomputed) {
        PACC_TRACE fprintf(stderr, "seq 207 @ col %ld?\n", _x);
        _pacc_Push(_cont);
        _cont = 207;
        _status = parsed;
        PACC_TRACE fprintf(stderr, "lit 205 @ col %ld?\n", _x);
        PACC_TRACE fprintf(stderr, "lit 205 @ col %ld => ", _x);
        if (_x+1 <= _pacc->input_length && memcmp(")", _pacc->string + _x, 1) == 0) {
          _status = parsed;
          _x += 1;
        }
        else {
          _pacc_error(_pacc, ".\")\"", _x);
          _status = no_parse;
        }
        PACC_TRACE fprintf(stderr, "lit 205 %ld => %s\n", _x, _status != no_parse ? "yes" : "no");
        if (_status == no_parse) {
          goto contin;
        }
        _pacc_save_core(217, _x);
        _pacc_Push(_x_rule);
        _pacc_Push(_cont);
        _cont = 206;
        _st = 217;
        /* call _ */
        goto top;
        case 206: /* return from _ */
        _pacc_Pop(_cont);
        _status = cur->rule & 3;
        _x = cur->remainder;
        _pacc_Pop(_x_rule);
        cur = _pacc_result(_pacc, _x_rule, 209);
        case 207:
        _pacc_Pop(_cont);
        PACC_TRACE fprintf(stderr, "seq 207 @ col %ld => %s\n", _x_rule, _status!=no_parse?"yes":"no");
        PACC_TRACE fprintf(stderr, "col is %ld\n", _x);
        cur->rule = (cur->rule & ~3) | _status;
        cur->remainder = _x;
        if (_pacc->err_col == _x_rule) {
          _pacc->err_valid = 0;
          _pacc_error(_pacc, ".rBrace", _x_rule);
        }
      }
      goto contin;
      case 217: /* _ */
      PACC_TRACE fprintf(stderr, "rule 217 (_) col %ld\n", _x);
      _x_rule = _x;
      cur = _pacc_result(_pacc, _x, 217);
      if ((cur->rule & 3) == uncomputed) {
        PACC_TRACE fprintf(stderr, "seq 215 @ col %ld?\n", _x);
        _pacc_Push(_cont);
        _cont = 215;
        _status = parsed;
        _pacc_save_core(237, _x);
        _pacc_Push(_x_rule);
        _pacc_Push(_cont);
        _cont = 214;
        _st = 237;
        /* call _:*:0 */
        goto top;
        case 214: /* return from _:*:0 */
        _pacc_Pop(_cont);
        _status = cur->rule & 3;
        _x = cur->remainder;
        _pacc_Pop(_x_rule);
        cur = _pacc_result(_pacc, _x_rule, 217);
        case 215:
        _pacc_Pop(_cont);
        PACC_TRACE fprintf(stderr, "seq 215 @ col %ld => %s\n", _x_rule, _status!=no_parse?"yes":"no");
        PACC_TRACE fprintf(stderr, "col is %ld\n", _x);
        cur->rule = (cur->rule & ~3) | _status;
        cur->remainder = _x;
        if (_pacc->err_col == _x_rule) {
          _pacc->err_valid = 0;
          _pacc_error(_pacc, "._", _x_rule);
        }
      }
      goto contin;
      case 237: /* _:*:0 */
      PACC_TRACE fprintf(stderr, "rule 237 (_:*:0) col %ld\n", _x);
      _x_rule = _x;
      cur = _pacc_result(_pacc, _x, 237);
      if ((cur->rule & 3) == uncomputed) {
        PACC_TRACE fprintf(stderr, "alt 235 @ col %ld?\n", _x);
        _pacc_Push(_cont);
        _cont = 235;
        PACC_TRACE fprintf(stderr, "save column registers\n");
        _pacc_Push(_x); _pacc_Push(cur->ev_valid);
        PACC_TRACE fprintf(stderr, "seq 234 @ col %ld?\n", _x);
        _pacc_Push(_cont);
        _cont = 234;
        _status = parsed;
        _pacc_save_core(230, _x);
        _pacc_Push(_x_rule);
        _pacc_Push(_cont);
        _cont = 233;
        _st = 230;
        /* call _:1:0 */
        goto top;
        case 233: /* return from _:1:0 */
        _pacc_Pop(_cont);
        _status = cur->rule & 3;
        _x = cur->remainder;
        _pacc_Pop(_x_rule);
        cur = _pacc_result(_pacc, _x_rule, 237);
        if (_status == no_parse) {
          goto contin;
        }
        _pacc_save_core(237, _x);
        _pacc_Push(_x_rule);
        _pacc_Push(_cont);
        _cont = 232;
        _st = 237;
        /* call _:*:0 */
        goto top;
        case 232: /* return from _:*:0 */
        _pacc_Pop(_cont);
        _status = cur->rule & 3;
        _x = cur->remainder;
        _pacc_Pop(_x_rule);
        cur = _pacc_result(_pacc, _x_rule, 237);
        case 234:
        _pacc_Pop(_cont);
        PACC_TRACE fprintf(stderr, "seq 234 @ col %ld => %s\n", _x_rule, _status!=no_parse?"yes":"no");
        PACC_TRACE fprintf(stderr, "col is %ld\n", _x);
        PACC_TRACE fprintf(stderr, "alt 235 @ col %ld => %s\n", _x, _status!=no_parse?"yes":"no");
        if (_status != no_parse) {
          PACC_TRACE fprintf(stderr, "accept column registers\n");
          _pacc_Discard(cur->ev_valid); _pacc_Discard(_x);
          goto contin;
        }
        PACC_TRACE fprintf(stderr, "restore column registers\n");
        _pacc_Pop(cur->ev_valid); _pacc_Pop(_x);
        PACC_TRACE fprintf(stderr, "save column registers\n");
        _pacc_Push(_x); _pacc_Push(cur->ev_valid);
        PACC_TRACE fprintf(stderr, "col restored to %ld\n", _x);
        PACC_TRACE fprintf(stderr, "alt 235 @ col %ld? (next alternative)\n", _x);
        PACC_TRACE fprintf(stderr, "seq 231 @ col %ld?\n", _x);
        _pacc_Push(_cont);
        _cont = 231;
        _status = parsed;
        case 231:
        _pacc_Pop(_cont);
        PACC_TRACE fprintf(stderr, "seq 231 @ col %ld => %s\n", _x_rule, _status!=no_parse?"yes":"no");
        PACC_TRACE fprintf(stderr, "col is %ld\n", _x);
        if (_status == no_parse) {
          PACC_TRACE fprintf(stderr, "restore column registers\n");
          _pacc_Pop(cur->ev_valid); _pacc_Pop(_x);
        }
        else {
          PACC_TRACE fprintf(stderr, "accept column registers\n");
          _pacc_Discard(cur->ev_valid); _pacc_Discard(_x);
        }
        case 235:
        _pacc_Pop(_cont);
        PACC_TRACE fprintf(stderr, "alt 235 @ col %ld => %s\n", _x, _status!=no_parse?"yes":"no");
        PACC_TRACE fprintf(stderr, "col is %ld\n", _x);
        cur->rule = (cur->rule & ~3) | _status;
        cur->remainder = _x;
        if (_pacc->err_col == _x_rule) {
          _pacc->err_valid = 0;
          _pacc_error(_pacc, "._:*:0", _x_rule);
        }
      }
      goto contin;
      case 230: /* _:1:0 */
      PACC_TRACE fprintf(stderr, "rule 230 (_:1:0) col %ld\n", _x);
      _x_rule = _x;
      cur = _pacc_result(_pacc, _x, 230);
      if ((cur->rule & 3) == uncomputed) {
        PACC_TRACE fprintf(stderr, "cclass 213 @ col %ld?\n", _x);
        if (_x < _pacc->input_length) {
          _pacc_any_i = _pacc_utf8_char(_pacc->string+_x, _pacc->input_length - _x, &_pacc_utf_cp);
          if (!_pacc_any_i) pacc_panic("invalid UTF-8 input");
          if (_pacc_utf_cp==32 || _pacc_utf_cp==9 || _pacc_utf_cp==10) {
            _status = parsed;
            _x += _pacc_any_i;
          }
          else {
            _pacc_error(_pacc, ".[ \t\n]", _x);
            _status = no_parse;
          }
        }
        else {
          _pacc_error(_pacc, ".[ \t\n]", _x);
          _status = no_parse;
        }
        PACC_TRACE fprintf(stderr, "cclass 213 %ld => %s\n", _x, _status != no_parse ? "yes" : "no");
        cur->rule = (cur->rule & ~3) | _status;
        cur->remainder = _x;
        if (_pacc->err_col == _x_rule) {
          _pacc->err_valid = 0;
          _pacc_error(_pacc, "._:1:0", _x_rule);
        }
      }
      goto contin;
      case 228: /* Decimal:*:0 */
      PACC_TRACE fprintf(stderr, "rule 228 (Decimal:*:0) col %ld\n", _x);
      _x_rule = _x;
      cur = _pacc_result(_pacc, _x, 228);
      if ((cur->rule & 3) == uncomputed) {
        PACC_TRACE fprintf(stderr, "alt 226 @ col %ld?\n", _x);
        _pacc_Push(_cont);
        _cont = 226;
        PACC_TRACE fprintf(stderr, "save column registers\n");
        _pacc_Push(_x); _pacc_Push(cur->ev_valid);
        PACC_TRACE fprintf(stderr, "seq 225 @ col %ld?\n", _x);
        _pacc_Push(_cont);
        _cont = 225;
        _status = parsed;
        _pacc_save_core(220, _x);
        _pacc_Push(_x_rule);
        _pacc_Push(_cont);
        _cont = 224;
        _st = 220;
        /* call Decimal:1:0 */
        goto top;
        case 224: /* return from Decimal:1:0 */
        _pacc_Pop(_cont);
        _status = cur->rule & 3;
        _x = cur->remainder;
        _pacc_Pop(_x_rule);
        cur = _pacc_result(_pacc, _x_rule, 228);
        if (_status == no_parse) {
          goto contin;
        }
        _pacc_save_core(228, _x);
        _pacc_Push(_x_rule);
        _pacc_Push(_cont);
        _cont = 223;
        _st = 228;
        /* call Decimal:*:0 */
        goto top;
        case 223: /* return from Decimal:*:0 */
        _pacc_Pop(_cont);
        _status = cur->rule & 3;
        _x = cur->remainder;
        _pacc_Pop(_x_rule);
        cur = _pacc_result(_pacc, _x_rule, 228);
        case 225:
        _pacc_Pop(_cont);
        PACC_TRACE fprintf(stderr, "seq 225 @ col %ld => %s\n", _x_rule, _status!=no_parse?"yes":"no");
        PACC_TRACE fprintf(stderr, "col is %ld\n", _x);
        PACC_TRACE fprintf(stderr, "alt 226 @ col %ld => %s\n", _x, _status!=no_parse?"yes":"no");
        if (_status != no_parse) {
          PACC_TRACE fprintf(stderr, "accept column registers\n");
          _pacc_Discard(cur->ev_valid); _pacc_Discard(_x);
          goto contin;
        }
        PACC_TRACE fprintf(stderr, "restore column registers\n");
        _pacc_Pop(cur->ev_valid); _pacc_Pop(_x);
        PACC_TRACE fprintf(stderr, "save column registers\n");
        _pacc_Push(_x); _pacc_Push(cur->ev_valid);
        PACC_TRACE fprintf(stderr, "col restored to %ld\n", _x);
        PACC_TRACE fprintf(stderr, "alt 226 @ col %ld? (next alternative)\n", _x);
        PACC_TRACE fprintf(stderr, "seq 222 @ col %ld?\n", _x);
        _pacc_Push(_cont);
        _cont = 222;
        _status = parsed;
        _pacc_save_core(220, _x);
        _pacc_Push(_x_rule);
        _pacc_Push(_cont);
        _cont = 221;
        _st = 220;
        /* call Decimal:1:0 */
        goto top;
        case 221: /* return from Decimal:1:0 */
        _pacc_Pop(_cont);
        _status = cur->rule & 3;
        _x = cur->remainder;
        _pacc_Pop(_x_rule);
        cur = _pacc_result(_pacc, _x_rule, 228);
        case 222:
        _pacc_Pop(_cont);
        PACC_TRACE fprintf(stderr, "seq 222 @ col %ld => %s\n", _x_rule, _status!=no_parse?"yes":"no");
        PACC_TRACE fprintf(stderr, "col is %ld\n", _x);
        if (_status == no_parse) {
          PACC_TRACE fprintf(stderr, "restore column registers\n");
          _pacc_Pop(cur->ev_valid); _pacc_Pop(_x);
        }
        else {
          PACC_TRACE fprintf(stderr, "accept column registers\n");
          _pacc_Discard(cur->ev_valid); _pacc_Discard(_x);
        }
        case 226:
        _pacc_Pop(_cont);
        PACC_TRACE fprintf(stderr, "alt 226 @ col %ld => %s\n", _x, _status!=no_parse?"yes":"no");
        PACC_TRACE fprintf(stderr, "col is %ld\n", _x);
        cur->rule = (cur->rule & ~3) | _status;
        cur->remainder = _x;
        if (_pacc->err_col == _x_rule) {
          _pacc->err_valid = 0;
          _pacc_error(_pacc, ".Decimal:*:0", _x_rule);
        }
      }
      goto contin;
      case 220: /* Decimal:1:0 */
      PACC_TRACE fprintf(stderr, "rule 220 (Decimal:1:0) col %ld\n", _x);
      _x_rule = _x;
      cur = _pacc_result(_pacc, _x, 220);
      if ((cur->rule & 3) == uncomputed) {
        PACC_TRACE fprintf(stderr, "cclass 175 @ col %ld?\n", _x);
        if (_x < _pacc->input_length) {
          _pacc_any_i = _pacc_utf8_char(_pacc->string+_x, _pacc->input_length - _x, &_pacc_utf_cp);
          if (!_pacc_any_i) pacc_panic("invalid UTF-8 input");
          if ((_pacc_utf_cp>=48&&_pacc_utf_cp<=57)) {
            _status = parsed;
            _x += _pacc_any_i;
          }
          else {
            _pacc_error(_pacc, ".[0-9]", _x);
            _status = no_parse;
          }
        }
        else {
          _pacc_error(_pacc, ".[0-9]", _x);
          _status = no_parse;
        }
        PACC_TRACE fprintf(stderr, "cclass 175 %ld => %s\n", _x, _status != no_parse ? "yes" : "no");
        cur->rule = (cur->rule & ~3) | _status;
        cur->remainder = _x;
        if (_pacc->err_col == _x_rule) {
          _pacc->err_valid = 0;
          _pacc_error(_pacc, ".Decimal:1:0", _x_rule);
        }
      }
      goto contin;
      case -1: break;
    }

    _x = 0;
    cur = _pacc_result(_pacc, _x, start_rule_id);
    /* pacc grammars are anchored on the right */
    if ((cur->rule & 3) == parsed && cur->remainder != _pacc->input_length) {
	_pacc_error(_pacc, ".end-of-input", cur->remainder);
	cur->rule = (cur->rule & ~3) | no_parse;
    }
    if (!_evaling && (cur->rule & 3) == parsed) {
	unsigned char *stack_save;

	PACC_TRACE fprintf(stderr, "PARSED! Time to start eval...\n");
	_evaling = 1;
	_pacc_ev_i = 0;
    eval_loop:
	PACC_TRACE fprintf(stderr, "eval loop with _pacc_ev_i == %ld\n", _pacc_ev_i);
	stack_save = _pacc->sp;
    eval1:
	while (_pacc_ev_i < cur->ev_valid) {
	    int col, rule;
	    rule = cur->evlis[_pacc_ev_i].call_id;
	    col = cur->evlis[_pacc_ev_i].col;
	    ++_pacc_ev_i;
	    PACC_TRACE fprintf(stderr, "eval loop: r%d @ c%d\n", rule, col);
	    _pacc_Push(cur);
	    _pacc_Push(_pacc_ev_i);
	    cur = _pacc_result(_pacc, col, rule);
	    _pacc_ev_i = 0;
	}
	if ((cur->rule & 3) != evaluated && cur->expr_id) {
	    _x = cur->col;
	    _st = cur->expr_id;
	    goto top;
	}
    _pacc_expr_done:
	cur->rule = (cur->rule & ~3) | evaluated;
	if (_pacc->sp != stack_save) {
	    _pacc_Pop(_pacc_ev_i);
	    _pacc_Pop(cur);
	    goto eval1;
	}
	PACC_TRACE fprintf(stderr, "eval finished\n");
	goto contin;
    }

    PACC_TRACE {
	unsigned char min;
	unsigned int i;
	unsigned long a, v;

	a = v = 0;
	min = _pacc->m_chain_max;
	for (i = 0; i < _pacc->m_bkt_cnt; ++i) {
	    unsigned char valid = _pacc->m_valid[i * 2];
	    unsigned char alloc = _pacc->m_valid[i * 2 + 1];
	    fprintf(stderr, "%d/%d ", valid, alloc);
	    v += valid; a += alloc;
	    if (alloc < min)
		min = alloc;
	}
	fprintf(stderr, "\n");
	fprintf(stderr, "used %u buckets\n", _pacc->m_bkt_cnt);
	fprintf(stderr, "chain length from %d to %d\n", min, _pacc->m_chain_max);
	fprintf(stderr, "total used/allocated: %ld/%ld\n", v, a);
    }
    if ((cur->rule & 3) == evaluated) {
	PACC_TRACE fprintf(stderr, "parsed with value " TYPE_PRINTF "\n", cur->value.u0); /* XXX u0 */
	_pacc->value = cur->value.u0;
    } else if ((cur->rule & 3) == parsed) {
	PACC_TRACE fprintf(stderr, "parsed with void value\n");
    } else PACC_TRACE fprintf(stderr, "not parsed\n");
    return (cur->rule & 3) == evaluated;
}

PACC_TYPE PACC_SYM(result)(struct pacc_parser *p) {
    return p->value;
}

int PACC_SYM(wrap)(const char *name, char *addr, size_t l, PACC_TYPE *result) {
    int parsed;
    struct pacc_parser *p;

    p = PACC_SYM(new)();

    PACC_SYM(input)(p, addr, l);
    parsed = PACC_SYM(parse)(p);
    if (parsed) *result = PACC_SYM(result)(p);
    else {
	char *e = PACC_SYM(error)(p);
	fprintf(stderr, "%s:%s\n", name, e);
	free(e);
    }

    PACC_SYM(destroy)(p);

    return parsed;
}
