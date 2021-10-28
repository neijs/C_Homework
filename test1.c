#include "normal_input.h"

int main(int argc, char **argv) {
    int i;
    char *word;

    for (i = 1; i < argc; i++) {
        word = argv[i];
        while (*word)
            putchar(*word++);
    }
}

