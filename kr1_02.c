#include <stdio.h>
#define BUFSIZE 100

int main() {
    int c, i, value = 0, weight = 1, counter = 0, number;
    char tmp[3] = {'z', 'z', 'z'};
    while ((c = getchar()) != '\n' && c != EOF) {
        if (c == ' ' && counter) {
            weight = 1;
            counter = 0;
            for (i = 2; i >= 0; i--) {
                if (tmp[i] == 'a') {
                    number = -1;
                    value = value + number * weight;
                } else if (tmp[i] != 'z') {
                    number = tmp[i] - '0';
                    value = value + number * weight;
                    weight *= 3;
                }
            }
            for (i = 0; i < 3; i++)
                tmp[i] = 'z';
            printf("%d\n", value);
            value = 0;
        } else {
            tmp[counter] = c;
            counter++;
        }
    }
            weight = 1;
            counter = 0;
            for (i = 2; i >= 0; i--) {
                if (tmp[i] == 'a') {
                    number = -1;
                    value = value + number * weight;
                } else if (tmp[i] != 'z') {
                    number = tmp[i] - '0';
                    value = value + number * weight;
                    weight *= 3;
                }
            }
            for (i = 0; i < 3; i++)
                tmp[i] = 'z';
            printf("%d\n", value);
            value = 0;
    return 0;
}
