#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    FILE *fptr1, *fptr2;
    char curr;
    int line_number = 0;

    fptr1 = fopen(argv[1],"r");
    fptr2 = fopen(argv[2], "w");
    while (1) {
        curr = getc(fptr1);
        if (line_number % 2 == 0)
            putc(curr, fptr2);
        if (curr == '\n')
            line_number++;
        if (curr == EOF)
            break;
    }
    fclose(fptr1);
    fclose(fptr2);
}