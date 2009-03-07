#include <stdio.h>

#define SIZE 251

#define HASHSTART 5381

static unsigned int hashadd(unsigned int h, unsigned char c) {
	h += h << 5;
	h ^= c;
	return h;
}

int main(void) {
	int i, j, k;
	int tally[SIZE];
	unsigned char *p;

	for (j = 0; j < 100000; ++j) {
		for (k = 0; k < 100; ++k) {
			h = hashadd(HASHSTART, k);
			for (p = (unsigned char *)&j; 
