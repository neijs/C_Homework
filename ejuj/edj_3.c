#include <stdio.h>

int main() {
	int a, b, i, j, n;

	scanf("%d %d %d", &a, &b, &n);
	printf("%*c ", n, ' ');
	for (i = a; i < b; i++) {
		printf("%*d", n, i);
		if (i != b - 1)
			printf(" ");
	}
	printf("\n");
	for (i = a; i < b; i++) {
		printf("%*d ", n, i);
		for (j = a; j < b; j++) {
			printf("%*lld", n, (long long) i * j);
			if (j != b - 1)
			printf(" ");		
		}
		printf("\n");
	}
	return 0;
}