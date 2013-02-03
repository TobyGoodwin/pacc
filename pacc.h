struct pacc_parser;
#include <sys/types.h>
#include "syntax.h"
extern struct pacc_parser *pacc_new(void);
extern void pacc_input(struct pacc_parser *, const char *, char *, size_t);
extern void pacc_destroy(struct pacc_parser *);
extern int pacc_parse(struct pacc_parser *);
extern struct s_node * pacc_result(struct pacc_parser *);
extern const char *pacc_error(struct pacc_parser *);
extern const char *pacc_pos(struct pacc_parser *, const char *);
extern int pacc_wrap(const char *, char *, size_t, struct s_node * *);
