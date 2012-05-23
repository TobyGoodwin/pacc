

#include <ctype.h>

#include "syntax.h"


#include <sys/types.h>
struct pacc_parser;
extern void pacc_input(struct pacc_parser, char, char, off_t l);
extern void pacc_destroy(struct pacc_parser *);
extern int pacc_parse(struct pacc_parser *);
extern struct s_node * pacc_result(struct pacc_parser *);
extern void pacc_error(struct pacc_parser *);
extern int pacc_wrap(const char *, char *, off_t, struct s_node * *result);
