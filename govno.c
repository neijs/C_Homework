#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>

int main (int argc, char **argv) {
    int fd = open("subject_file.txt", O_RDWR);
    int k;
    while (read (fd, &k, sizeof(char))) {
        printf("%c", k);
        write(fd, "a", sizeof(char));
    }
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    close(fd);
    return 0;
}
