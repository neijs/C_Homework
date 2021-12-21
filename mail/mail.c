#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MSG_LEN 1024

struct package {
    int target;
    char msg[MSG_LEN + 1];
};

int N; /* The amount of processes */
int flag1 = 0, flag2 = 0, stop = 0; /* Flags for signals */

struct package *pickup(int);

void postman(pid_t *, int *);
void companion(struct package *, int *);

void hndlr(int);

int main (int argc, char **argv) {
    pid_t *pids;
    int i, j;
    int fd[2];
    struct package *letter;

    /* Extracting amount of processes from argv */
    if (argc > 2) {
        printf("Error: too many arguments in argv. You should enter <number of processes>.\n");
        return 1;
    } else if (argc != 2) {
        printf("Error: too few arguments in argv. You should enter <number of processes>.\n");
        return 1;
    } else if (sscanf(argv[1], "%d", &N) != 1) {
        printf("Error: incorrect amount of processes in argv.\n");
        return 1;
    }
    /* ---------------------------------------- */
    signal(SIGUSR1, hndlr);
    signal(SIGUSR2, hndlr);
    signal(SIGCHLD, hndlr);
    if (pipe(fd) == -1) {
        printf("Error: problems with pipe.\n");
        return 1;
    }
    pids = (pid_t *) malloc(sizeof(pid_t) * (N - 1));
    if (pids == NULL) {
        free(pids);
        close(fd[0]);
        close(fd[1]);
        return 1;
    }
    for (i = 0; i < N - 1; i++) {
        if ((pids[i] = fork()) == -1)
            printf("Warning: failed to create child #%d.\n", i);
        else if (pids[i] == 0) {
            letter = pickup(i + 1);
            companion(letter, fd);
            free(letter);
            free(pids);
            pids = NULL;
            letter = NULL;
            close(fd[0]);
            close(fd[1]);
            return 0;
        }
    }
    postman(pids, fd);
    for (j = 0; j < N - 1; j++)
        kill(pids[j], SIGCHLD);
    free(pids);
    pids = NULL;
    while (wait(NULL) != -1);
    close(fd[0]);
    close(fd[1]);
    return 0;
}

/* Child process opens his config file and takes out the target and message */
struct package *pickup(int badge) {
    char file_name[15];
    char *msg;
    int target = 0, c, cnt = 0, seen_character = 0, newline_c = 0;
    FILE *file;
    struct package *package = NULL;

    sprintf(file_name, "config%d.txt", badge);
    if ((file = fopen(file_name, "r")) == NULL) {
        printf("Error: child #%d couldn't open his config file\n", badge);
        return package;
    }
    package = (struct package *) malloc(sizeof(struct package));
    while ((c = fgetc(file)) == ' ' || c == '\t'); /* skip white spaces */
    fseek(file, -1, SEEK_CUR);
    if (fscanf(file, "%d", &target) != 1) {
        printf("Error: child #%d encountered incorrect data while reading his target.\n", badge);
        free(package);
        package = NULL;
        fclose(file);
        return package;        
    }
    if (target <= 0 || target >= N) {
        printf("Error: child #%d encountered invalid target.\n", badge);
        free(package);
        package = NULL;
        fclose(file);
        return package;
    }
    package -> target = target;
    while ((c = fgetc(file)) == ' ' || c == '\t'); /* skip white spaces */
    if (c != '\n' && c != EOF) {
        printf("Error: child #%d encountered extra data in the first line of package.\n", badge);
        free(package);
        package = NULL;
        fclose(file);
        return package;
    } else if (c == EOF) {
        printf("Error: child #%d: there was no message in the package.\n", badge);
        free(package);
        package = NULL;
        fclose(file);
        return package;
    }
    while ((c = fgetc(file)) != EOF) {
        if (cnt == MSG_LEN) {
            printf("Error: child #%d: the message size is too big.\n", badge);
            free(package);
            package = NULL;
            fclose(file);
            return package;
        }
        if (newline_c) {
            printf("Error: child #%d encountered extra input in the message.\n", badge);
            free(package);
            package = NULL;
            fclose(file);
            return package;
        }
        if (c != ' ' && c != '\t' && c != '\n') {
            if (!seen_character) 
                seen_character = 1;
        } else if (c == '\n')
            newline_c++;
        if (c != '\n')
            package -> msg[cnt++] = c;
    }
    if (!seen_character) {
        printf("Error: child #%d: the message is empty.\n", badge);
        free(package);
        package = NULL;
        fclose(file);
        return package;
    }
    package -> msg[cnt] = '\0';
    fclose(file);
    return package;
}

/* SIGUSR1 -- flag1 (speak); SIGUSR2 -- flag2(silent) */
void postman(pid_t *pids, int *fd) {
    int son;
    struct package letter;

    for (son = 0; son < N - 1; son++) {
        kill(pids[son], SIGUSR1); /* Speak, son */
        while (flag1 == 0 && flag2 == 0);
        if (flag2) {
            flag2 = 0;
            continue;
        }
        flag1 = 0;
        read(fd[0], &letter, sizeof(struct package)); /* Receive the letter */
        write(fd[1], &son, sizeof(int)); /* Send the number of the speaker */
        write(fd[1], &letter, sizeof(struct package)); /* Send the letter */
        kill(pids[letter.target - 1], SIGUSR2); /* Receive, son */
        while (flag1 == 0); /* Wait for son to receive and print */
        flag1 = 0;
    }    
}

/* SIGUSR1 -- flag1 (send); SIGUSR2 -- flag2 (receive)  */
void companion(struct package *letter, int *fd) {
    struct package received_letter;
    int from;

    while (1) {
        while (flag1 == 0 && flag2 == 0 && stop == 0); /* wait for father */
        if (flag1) {
            flag1 = 0;
            if (letter == NULL) {
                kill(getppid(), SIGUSR2);  /* silent */
                continue;
            } else {
                write(fd[1], letter, sizeof(struct package));
                kill(getppid(), SIGUSR1);  /* speak  */
            }
        } else if (flag2) {
            flag2 = 0;
            read(fd[0], &from, sizeof(int));
            read(fd[0], &received_letter, sizeof(struct package));
            printf("Son #%d received message from son #%d:\n", received_letter.target, from + 1);
            fputs(received_letter.msg, stdout);
            printf("\n");
            kill(getppid(), SIGUSR1);
        } else if (stop)
            return;
    }
}

void hndlr(int n) {
    if (n == SIGUSR1) {
        flag1 = 1;
        signal(SIGUSR1, hndlr);
    } else if (n == SIGUSR2) {
        flag2 = 1;
        signal(SIGUSR2, hndlr);
    } else if (n == SIGCHLD) {
        stop = 1;
    }
}
