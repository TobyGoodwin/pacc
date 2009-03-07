#include <stdio.h> 
#define SIZE 251

#define HASHSTART 5381

static unsigned int hashadd(unsigned int h, unsigned int n) {
	h += h << 5;
	h ^= n;
	return h;
}

int main(void) {
	int h, i, j, k;
	long tally[SIZE];
	unsigned char *p;

	for (i = 0; i < SIZE; ++i)
		tally[i] = 0;

	for (j = 0; j < 100000; ++j) {
		for (k = 0; k < 100; ++k) {
			h = HASHSTART;
			h = hashadd(h, k);
			h = hashadd(h, j);
			++tally[h % SIZE];
		}
	}

	for (i = 0; i < SIZE; ++i) {
		printf("%3d %ld\n", i, tally[i]);
	}
}
