#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main (int argc, char **argv) {
    int bfd, c, i;

    if (argc != 2)
        return 1;
    else {
        bfd = open(argv[1], O_RDWR | O_CREAT);
    } 

    for (i = 0; i < 10; i++) {
        scanf("%d", &c);
        write(bfd, &c, sizeof(int));
    }
    close(bfd);

    return 0;
}