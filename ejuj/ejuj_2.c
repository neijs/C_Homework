#include <stdio.h>

int main() {
    int sum = 0;
    int c;

    while ((c = getchar()) != EOF) {
        if (c >= '0' && c <= '9')
            sum = sum + c - '0';
        else if (c >= 'a' && c <= 'f')
            sum = sum + c - 'a' + 10;
        else if (c >= 'A' && c <= 'F')
            sum = sum + c - 'A' + 10;
    }
    printf("%d\n", sum);
    return 0;
}