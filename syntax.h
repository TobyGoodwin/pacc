#ifndef PACC_SYNTAX_H
#define PACC_SYNTAX_H 1
enum s_type {
    grammar, rule,	/* scaffolding */
    type, alt, seq,	/* fundamentals */
    and, not,		/* syntactic predicates */
    expr, bind,		/* expressions */
    guard, ident,	/* semantic predicate */
    call, lit, any,	/* matchers */
    rep, 		/* sugar */
    preamble,
    s_type_max
};

struct s_node {
    enum s_type type;
    long id;
    struct s_node *first, *last; /* children */
    struct s_node *next; /* sibling */
    char *text;
};

/* building trees */
extern struct s_node *create(void);

extern struct s_node *s_new(enum s_type);
extern struct s_node *s_text(enum s_type, char *);
extern struct s_node *s_kid(enum s_type, struct s_node *);
extern struct s_node *s_both(enum s_type, char *, struct s_node *);
extern struct s_node *s_retype(enum s_type, struct s_node *);
extern struct s_node *cons(struct s_node *, struct s_node *);
extern struct s_node *s_set_cons(struct s_node *, struct s_node *);
#define new_node(t) s_new(t)

extern struct s_node *s_alt(struct s_node *, struct s_node *);
extern char *s_stash_type(char *);
extern char *s_stashed_type(void);

/* manipulating trees */
extern void desugar(struct s_node *);
extern void resolve(struct s_node *);

/* examining trees */
extern void s_dump(struct s_node *);
extern int s_has_children(enum s_type);
extern int s_has_text(enum s_type);

extern char *prefix;
#endif
