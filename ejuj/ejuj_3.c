#include <stdio.h>
#include <math.h>

int main() {
    int a, b, n;
    int i, j, k;
    scanf("%d%d%d", &a, &b, &n);
    for (i = a; i < b; i++) {
        if (i == a)
            i = 1;
        for (j = a; j < b; j++) {
            if (j == a)
                j = 1;
            printf("%*d", n, i*j);
        }
        puts("");
    }
    return 0;
}