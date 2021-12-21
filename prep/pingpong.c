#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int flag1 = 0, flag2 = 0;

void hndlr(int n) {
    if (n == SIGUSR1) {
        flag1 = 1;
        signal(SIGUSR1, hndlr); 
    } else if (n == SIGUSR2) {
        flag2 = 1;
        signal(SIGUSR2, hndlr); 
    }      
}

int main(int argc, char **argv)
{
    int N, ball;
    int fd[2];
    pid_t father, son, grandson;
    
    fflush(stdout);
    signal(SIGUSR1, hndlr);
    signal(SIGUSR2, hndlr);
    if (pipe(fd) == -1) {
        return 1;
    }
    if (scanf("%d", &N) != 1) {
        return 1;
    }
    son = fork();
    if (son == 0) { /* Son */
        father = getppid();
        grandson = fork();
        if (grandson == 0) { /* Grandson */
            ball = 0;
            son = getppid();
            printf("grandson %d\n", ball);
            fflush(stdout);
            write(fd[1], &ball, sizeof(int));
            kill(son, SIGUSR1); /* Son, catch! */
            while (1) {
                while (flag1 == 0 && flag2 == 0);
                if (flag2)
                    break;
                flag1 = 0;
                read(fd[0], &ball, sizeof(int));
                ball++;
                if (ball > N) {
                    kill(son, SIGUSR2);
                    kill(father, SIGUSR2);
                    break;  
                }
                printf("grandson %d\n", ball);
                fflush(stdout);
                write(fd[1], &ball, sizeof(int));
                kill(son, SIGUSR1); /* Son, catch! */
            }
            close(fd[0]);
            close(fd[1]);
            return 0;
        } else { /* Son */
            while (1) {
                while (flag1 == 0 && flag2 == 0);
                if (flag2)
                    break;
                flag1 = 0;
                read(fd[0], &ball, sizeof(int));
                ball++;
                if (ball > N) {
                    kill(father, SIGUSR2);
                    kill(grandson, SIGUSR2);
                    break;  
                }
                printf("son %d\n", ball);
                fflush(stdout);
                write(fd[1], &ball, sizeof(int));
                kill(getppid(), SIGUSR1); /* Dad, catch! */
                while (flag1 == 0);
                flag1 = 0;
                read(fd[0], &ball, sizeof(int));
                ball++;
                if (ball > N) {
                    kill(father, SIGUSR2);
                    kill(grandson, SIGUSR2);
                    break;  
                }
                printf("son %d\n", ball);
                fflush(stdout);
                write(fd[1], &ball, sizeof(int));
                kill(grandson, SIGUSR1);
            }
            kill(grandson, SIGUSR2);
            close(fd[0]);
            close(fd[1]);
            return 0;
        }  
    }
    else { /* Dad */
        while (1) {
            while (flag1 == 0 && flag2 == 0);
            if (flag2)
                break;
            flag1 = 0;
            read(fd[0], &ball, sizeof(int));
            ball++;
            if (ball > N) {
                kill(son, SIGUSR2);
                break;  
            }
            printf("father %d\n", ball);
            fflush(stdout);
            write(fd[1], &ball, sizeof(int));
            kill(son, SIGUSR1); /* Son, catch! */
        }
        close(fd[0]);
        close(fd[1]);
        return 0;
    }
}

