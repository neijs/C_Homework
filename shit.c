#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#define IN 1
#define OUT 0

int main (int argc, char** argv) {
    int fd = open("file.txt", O_RDONLY);
    char c;
    int le_count = 0;

    while (read(fd, &c, sizeof(char)) > 0) {
        if ()
        printf("%c ", c);
    }
    printf("\n");
    return 0;
}

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

int is_splitter(char);
void text_handling(char, char, int);

int main() {
    char a = 'd', b = ',';
    printf("%d %d", is_splitter(a), is_splitter(b));
    // int file = open("file.txt", O_WRONLY);
    // write(file, "abcde", 5 * sizeof(char));
    return 0;
}

int is_splitter(char c) {
    return (c == ' ' || c == '\t' || c == '.' || c == ',');
}

void text_handling(char letter_old, char letter_new, int pos_num) {
    /* Assuming the file is open and ready for work */
    char c;

    while ()