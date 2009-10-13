#include "syntax.h"

int main(int argc, char **argv) {
    struct s_node *p;

    p = create();
    s_dump(p);
    desugar(p);
    resolve(p);
    s_dump(p);
    emit(p);

    return 0;
}
