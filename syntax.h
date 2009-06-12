enum s_type {
    grammar, rule,	/* scaffolding */
    alt, seq,		/* fundamentals */
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
    int e_type; /* expression type */
    struct s_node *first, *last; /* children */
    struct s_node *next; /* sibling */
    char *bind, *text;
};

extern void s_dump(struct s_node *);
extern int s_has_children(enum s_type);
extern int s_has_text(enum s_type);
