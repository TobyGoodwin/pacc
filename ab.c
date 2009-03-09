/* A packrat parser for the following grammar

   S <- A B / Aa B
   A <- 'a'
   B <- 'b'
   Aa <- "aa"

   The point here is that when parsing the string "aab", the A
   production successfully matches, but then this match is not part of
   the overall parse. So what happens to any semantic action associated
   with A?
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int n_rules = 4;

char *string;

enum status {
    uncomputed, parsed, no_parse
};

struct intermed {
    enum status status;
    int value; /* semantic value */
    int remainder; /* unparsed string */
};

struct intermed *matrix;

struct intermed *S(int col);
struct intermed *A(int col);
struct intermed *B(int col);
struct intermed *Aa(int col);

/* semantic actions */
void sa_A0(void) { printf("production A\n"); }
void sa_Aa0(void) { printf("production Aa\n"); }
void sa_B0(void) { printf("production B\n"); }

void (*sastack[3])(void);
int sap, sap2;

struct intermed *S(int col) {
    int elem = col * n_rules + 0;
    if (matrix[elem].status == uncomputed) {
	struct intermed *a;
	matrix[elem].status = no_parse;
	sap2 = sap;
	a = A(col);
	if (a->status == parsed) {
	    struct intermed *b;
	    b = B(a->remainder);
	    if (b->status == parsed) {
		matrix[elem].remainder = b->remainder;
		matrix[elem].value = 17;
		matrix[elem].status = parsed;
		return matrix + elem;
	    }
	} else sap = sap2;
	sap2 = sap;
	a = Aa(col);
	if (a->status == parsed) {
	    struct intermed *b;
	    b = B(a->remainder);
	    if (b->status == parsed) {
		matrix[elem].remainder = b->remainder;
		matrix[elem].value = 9;
		matrix[elem].status = parsed;
		return matrix + elem;
	    }
	} else sap = sap2;
    }
    return matrix + elem;
}

struct intermed *A(int col) {
    int elem = col * n_rules + 1;
    if (matrix[elem].status == uncomputed) {
	matrix[elem].status = no_parse;
	if (string[col] == 'a') {
	    matrix[elem].remainder = col + 1;
	    matrix[elem].value = 11;
	    matrix[elem].status = parsed;
	    sastack[sap++] = &sa_A0;
	    return matrix + elem;
	}
    }
    return matrix + elem;
}

struct intermed *B(int col) {
    int elem = col * n_rules + 2;
    if (matrix[elem].status == uncomputed) {
	matrix[elem].status = no_parse;
	if (string[col] == 'b') {
	    matrix[elem].remainder = col + 1;
	    matrix[elem].value = 93;
	    matrix[elem].status = parsed;
	    sastack[sap++] = &sa_B0;
	    return matrix + elem;
	}
    }
    return matrix + elem;
}

struct intermed *Aa(int col) {
    int elem = col * n_rules + 3;
    if (matrix[elem].status == uncomputed) {
	matrix[elem].status = no_parse;
	if (string[col] == 'a' && string[col + 1] == 'a') {
	    matrix[elem].remainder = col + 2;
	    matrix[elem].value = 23;
	    matrix[elem].status = parsed;
	    sastack[sap++] = &sa_Aa0;
	    return matrix + elem;
	}
    }
    return matrix + elem;
}

void matrix_dump(int n) {
    int r, s;

    for (s = 0; s < n + 1; ++s) printf("   %c   ", string[s]);
    printf("\n");
    for (r = 0; r < n_rules; ++r) {
	for (s = 0; s < n + 1; ++s) {
	    int elem = s * n_rules + r;
	    switch (matrix[elem].status) {
	    case uncomputed:
		printf("   _   "); break;
	    case no_parse:
		printf("   X   "); break;
	    case parsed:
		printf("%3d,%2d ", matrix[elem].value, matrix[elem].remainder);
		break;
	    }
	}
	printf("\n");
    }
}

int main(int argc, char **argv) {
    int i, n, matrix_size;
    struct intermed *r;

    string = argv[1];
    printf("%s\n", string);
    n = strlen(string);
    matrix_size = n_rules * (n + 1);
    matrix = malloc(sizeof(struct intermed) * matrix_size);
    for (i = 0; i < matrix_size; ++i)
	matrix[i].status = uncomputed;
    sap = 0;

    r = S(0);
    if (r->status == parsed) {
	for (i = 0; i < sap; ++i)
	    sastack[i]();
	printf("==> %d\n", r->value);
	if (string[r->remainder] != '\0')
	    printf("input not completely consumed\n");
    } else {
	printf("parse error\n");
    }
    matrix_dump(n);
    return 0;
}
