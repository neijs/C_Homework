#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char **argv) {
    int id1, id2;

    id1 = fork();
    id2 = fork();
    while (wait(NULL) != -1 || errno != ECHILD)
        printf("waited for what?\n");
    if (!id2)
        if (id1)
            printf("z\n");
        else
            printf("y\n");
    else
        if (id1)
            printf("daddy\n");
        else
            printf("x\n");
        
}