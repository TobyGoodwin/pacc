enum s_type {
    grammar,
    rule,
    alt, seq,
    and, not, /* syntactic predicates */
    binding,
    lit, any, /* matchers */
    opt, rep0, rep1 /* sugar */
};

struct s_node {
    enum s_type type;
    struct s_node *first, *last; /* children */
    struct s_node *next; /* sibling */
    char *text;
};

extern void s_dump(struct s_node *);
