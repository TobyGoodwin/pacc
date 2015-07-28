#ifndef PACC_SYNTAX_H
#define PACC_SYNTAX_H 1

enum s_type {
    grammar, rule,		/* scaffolding */
    type, alt, seq,		/* fundamentals */
    cafe,			/* feed point */
    and, not,			/* syntactic predicates */
    expr, bind, coords,		/* expressions */
    guard, ident,		/* semantic predicate */
    call, lit, any,		/* matchers */
    cclass, cceq, ccge, ccle,	/* character classes */
    rep, 			/* sugar */
    preamble,
    s_type_max
};

struct s_node {
    enum s_type type;
    long id;
    struct s_node *first, *last; /* children */
    struct s_node *next; /* sibling */
    union {
	char *text;
	int pair[2];
	int number;
    };
    unsigned reached : 1;
};

/* building trees */
extern struct s_node *create(void);

extern struct s_node *s_new(enum s_type);
extern struct s_node *s_num(enum s_type, int);
extern struct s_node *s_text(enum s_type, char *);
extern struct s_node *s_coords(unsigned int line, unsigned int col);
extern struct s_node *s_child(struct s_node *, struct s_node *);
extern struct s_node *s_kid(enum s_type, struct s_node *);
extern struct s_node *s_both(enum s_type, char *, struct s_node *);
extern struct s_node *s_child_cons(struct s_node *r, struct s_node *s);
extern struct s_node *s_retype(enum s_type, struct s_node *);
extern struct s_node *cons(struct s_node *, struct s_node *);
extern struct s_node *append(struct s_node *, struct s_node *);
extern struct s_node *s_set_cons(struct s_node *, struct s_node *);
extern struct s_node *s_ccnode(const unsigned char *);
extern struct s_node *s_ccrange(struct s_node *, struct s_node *);
#define new_node(t) s_new(t)

extern struct s_node *s_alt(struct s_node *, struct s_node *);
extern char *s_stash_type(struct s_node *);
extern char *s_stashed_type(void);

/* manipulating trees */
extern void desugar(struct s_node *);

/* examining trees */
extern int path_max(struct s_node *, enum s_type);
extern int path_min(struct s_node *, enum s_type);
extern void s_dump(struct s_node *);
extern char *decode_type(enum s_type);
extern int s_has_children(enum s_type);
extern int s_is_number(enum s_type);
extern int s_is_pair(enum s_type);
extern int s_is_text(enum s_type);

extern char *prefix;
#endif
