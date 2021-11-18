#include <stdio.h>
#include <stdlib.h>

int main (int argc, char **argv) {
    int size = argc - 1, min_pos, i, printed = 0;
    int *buf = (int *) malloc(sizeof(int) * size);
    int *closed = (int *) malloc(sizeof(int) * size);
    FILE **fa = (FILE **) malloc(sizeof(FILE *) * size);

    if (!buf || !closed || !fa) {
        free(buf);
        free(closed);
        free(fa);
        return 1;
    }
    for (i = 0; i < size; i++) {
        fa[i] = fopen(argv[i + 1], "r");
        if (!fa[i])
            closed[i] = 1;
        else
            closed[i] = (fscanf(fa[i], "%d", &buf[i]) == EOF) ? 1 : 0;
    }
    while (1) {
        for (i = 0; closed[i] && i < size; i++);
        if (i == size) {
            if (printed)
                printf("\n");
            break;
        }
        min_pos = i;
        for (; i < size; i++)
            if (!closed[i] && buf[i] < buf[min_pos])
                min_pos = i;
        printf("%d ", buf[min_pos]);
        if (!printed)
            printed = 1;
        closed[min_pos] = (fscanf(fa[min_pos], "%d", &buf[min_pos]) == EOF) ? 1 : 0;
    }
    for (i = 0; i < size; i++)
        if (fa[i])
            fclose(fa[i]);
    free(fa);
    free(buf);
    free(closed);
    return 0;
}