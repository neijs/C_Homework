#include <stdio.h>
#include <stdlib.h>

int main(int argc, char ** argv) {
	int N;
	if (argc != 2) {
		printf("Incorrect amount of argv.\n");
		return 1;
	} else {
		sscanf(argv[1], &N, sizeof(int));	
	}

	N = N >> 2;
	printf("%d\n", N);
	return 0;
}
