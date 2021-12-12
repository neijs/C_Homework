#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    int i;
    for (i = 1; i < argc; i++) {
        int status;
        if (fork() > 0) {
            wait(&status);
            printf("process-father\n");
            continue;
        }
        execlp(argv[i], argv[i], (char *)0);
        exit(0);
    }
}