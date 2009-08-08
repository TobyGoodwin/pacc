enum s_type {
    grammar, rule,	/* scaffolding */
    type, alt, seq,	/* fundamentals */
    and, not,		/* syntactic predicates */
    expr, bind,		/* expressions */
    guard, ident,	/* semantic predicate */
    call, lit, any,	/* matchers */
    opt, rep0, rep1,	/* sugar */
    s_type_max
};

struct s_node {
    enum s_type type;
    int id;
    struct s_node *first, *last; /* children */
    struct s_node *next; /* sibling */
    char *text;
};

/* building trees */
extern struct s_node *s_new(enum s_type);
#define new_node(t) s_new(t)
extern void s_resolve(struct s_node *, struct s_node *);
#define resolve(g, n) s_resolve(g, n)

extern struct s_node *mkalt(struct s_node *);
extern struct s_node *mkcall(char *);
extern struct s_node *mkrule(char *, struct s_node *);
extern struct s_node *mkseq(struct s_node *);
extern struct s_node *cons(struct s_node *, struct s_node *);

extern void s_dump(struct s_node *);
extern int s_has_children(enum s_type);
extern int s_has_text(enum s_type);

extern char *prefix;
