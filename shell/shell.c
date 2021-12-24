#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>

#define LINE_SIZE 30

int EOF_flag = 0;
volatile sig_atomic_t flagC = 0, flagZ = 0;

char *get_line(int, int*);
char ***sep_cmd(char *, int, int*);
int word_count(char *);
int check_for_redir(char **, int*, int*);
int check_for_synt(char *);
void delete_excess(char ***, int, int);
void hndlr1(int);
void hndlr2(int);
void exec_pwd(void);
void exec_cd(char *);

int main() {
    pid_t *pids;
    int block_count, i, j, k, input;
    int stdinf = 0, stdoutf = 0;
    int *counts = NULL, *redir_fds = NULL;
    char *string = NULL;
    char ***commands = NULL;
    int fd[2];

    signal(SIGINT, hndlr1);
    signal(SIGTSTP, hndlr2);
    while (1) {
        printf("$ ");
        string = get_line(LINE_SIZE, &block_count);
        if (string == NULL) {
            if (EOF_flag == 1) {
                printf("EOF is reached.\n");
                break;
            }
            continue;
        }
        counts = (int *) malloc(block_count * sizeof(int));
        if (counts == NULL) {
            fprintf(stderr, "Error in main: couldn't allocate the memory for counts.\n");
            free(string);
            string = NULL;
            continue;
        }
        commands = sep_cmd(string, block_count, counts);
        if (commands == NULL) {
            string = NULL;
            continue;
        }
        pids = (pid_t *) malloc(block_count * sizeof(pid_t));
        if (pids == NULL) {
            fprintf(stderr, "Error in main: couldn't allocate the memory for pids.\n");
            free(string);
            string = NULL;
            free(counts);
            counts = NULL;
            for (i = 0; i < block_count; i++) {
                free(commands[i]);
                commands[i] = NULL;
            }
            free(commands);
            commands = NULL;
            continue;
        }
        redir_fds = (int *) malloc (3 * sizeof(int));
        if (redir_fds == NULL) {
            fprintf(stderr, "Error in main: couldn't allocate the memory for redir_fds.\n");
            free(pids);
            pids = NULL;
            free(string);
            string = NULL;
            free(counts);
            counts = NULL;
            for (i = 0; i < block_count; i++) {
                free(commands[i]);
                commands[i] = NULL;
            }
            free(commands);
            commands = NULL;
            continue;
        }
        for (j = 0; j < block_count; j++) {
            if (flagC || flagZ) {
                flagC = 0;
                flagZ = 0;
            }
            /* Обработка exit и cd ----------------------------------- */
            if (strcmp(commands[j][0], "exit") == 0) {
                free(pids);
                pids = NULL;
                free(redir_fds);
                redir_fds = NULL;
                free(string);
                string = NULL;
                free(counts);
                counts = NULL;
                for (i = 0; i < block_count; i++) {
                    free(commands[i]);
                    commands[i] = NULL;
                }
                free(commands);
                commands = NULL;
                printf("\nAborted due to exit.\n");
                exit(0);
            } 
            if (strcmp(commands[j][0], "cd") == 0) {
                exec_cd(commands[j][1]);
                continue;
            }
            /* ----------------------------------------------------- */

            if (pipe(fd) == -1) {
                fprintf(stderr, "Error in main: couldn't open the pipe.\n");
                break;
            }
            pids[j] = fork(); /* stdin - 0, stdout - 1, stderr - 2 */
            if (pids[j] == 0) {
                signal(SIGINT, SIG_DFL);
                signal(SIGTSTP, SIG_DFL);
                if (j != block_count - 1) /* Все кроме последнего отправляют свой вывод в пайп */
                    dup2(fd[1], 1);
                if (j > 0) /* Все кроме первого берут свой ввод из пайпа */
                    dup2(input, 0);
                if (check_for_redir(commands[j], &counts[j], redir_fds)) {
                    close(fd[0]);
                    close(fd[1]);
                    free(redir_fds);
                    redir_fds = NULL;
                    free(counts);
                    counts = NULL;
                    for (i = 0; i < block_count; i++) {
                        free(commands[i]);
                        commands[i] = NULL; 
                    }
                    free(commands);
                    commands = NULL;
                    free(string);
                    string = NULL;
                    free(pids);
                    pids = NULL;
                    exit(0);
                }
                for (i = 0; i < 3; i++) {
                    if (redir_fds[i] != -1) {
                        dup2(redir_fds[i], i);
                        close(redir_fds[i]);
                    }
                }
                free(redir_fds);
                redir_fds = NULL;
                free(pids);
                pids = NULL;
                free(counts);
                counts = NULL;
                close(fd[0]);
                close(fd[1]); 
                if (strcmp(commands[j][0], "pwd") == 0)
                    exec_pwd(); 
                else {
                    execvp(commands[j][0], commands[j]);
                    fprintf(stderr, "Command \'%s\' not found.\n", commands[j][0]);
                }
                free(string);
                string = NULL;
                for (i = 0; i < block_count; i++) {
                    free(commands[i]);
                    commands[i] = NULL;
                }
                free(commands);
                commands = NULL;
                exit(1);
            } 
            /* Father */
            waitpid(pids[j], NULL, WUNTRACED);
            if (flagC) {
                flagC = 0;
                printf("\nProcess PID:%d aborted. ", pids[j]);
                for (i = 0; i < counts[j]; i++)
                    printf("%s ", commands[j][i]);
                printf("\n");
            }
            if (flagZ) {
                flagZ = 0;
                printf("\nProcess PID:%d stopped. ", pids[j]);
                for (i = 0; i < counts[j]; i++)
                    printf("%s ", commands[j][i]);
                printf("\n");
            }
            input = fd[0];
            close(fd[1]);
        } /* for */

        /* Все отработало гладко, осталось все освободить */
        free(string);
        string = NULL;
        free(counts);
        counts = NULL;
        free(pids);
        pids = NULL;
        free(redir_fds);
        redir_fds = NULL;
        for (i = 0; i < block_count; i++) {
            free(commands[i]);
            commands[i] = NULL;
        }
        free(commands);
        commands = NULL;
        /* ---------------------------------------------- */
        if (EOF_flag == 1) {
            printf("EOF is reached.\n");
            break;
        }
    } /* while */
    return 0;
}

/* Get the conveyor input and count the amount of blocks */
char *get_line(int line_size, int *block_count) {
    int c, count = 0, seen_character = 0, sep_count = 0, prev = 0;
    char *the_string = (char *) malloc(line_size * sizeof(char));
    int CONV_flag = 0;

    if (!EOF_flag) {
        if (the_string == NULL) {
            fprintf(stderr, "Conveyor error: Couldn't allocate the memory for string.\n");
            free(the_string);
            the_string = NULL;
            return the_string;
        }
        *block_count = 0;
        while ((c = getchar()) != '\n' && c != EOF) {
            if (CONV_flag == 0) {
                if (c != ' ' && c != '\t' && c != '|') {
                    if (c == '>') {
                        if (the_string[count - 1] == '2') {
                            if (the_string[count - 2] != ' ')
                                the_string[count++] = ' ';
                        } else if (the_string[count - 1] == '>') {
                            if (the_string[count - 2] == '>')
                                the_string[count++] = ' ';
                        } else
                            the_string[count++] = ' ';
                    } else if (the_string[count - 1] == '>') {
                        if (c != '>')
                            the_string[count++] = ' ';
                    }
                    if (c == '<') {
                        if (the_string[count - 1] != ' ')
                            the_string[count++] = ' ';
                    } else if (the_string[count - 1] == '<')
                        the_string[count++] = ' ';
                    if (seen_character == 0)
                        seen_character = 1;
                    sep_count = 0;
                } else if (c == '|') {
                    sep_count++;
                    if (the_string[count - 1] == '>' || (the_string[count - 1] == ' ' && the_string[count - 2] == '>')) {
                        fprintf(stderr, "Conveyor error: unexpected token '|'.\n");
                        CONV_flag = 1;
                    }
                    if (seen_character) {
                        (*block_count)++;
                        seen_character = 0;
                    } else {
                        if (sep_count > 1)
                            fprintf(stderr, "Conveyor error: || is not allowed.\n");
                        else 
                            fprintf(stderr, "Conveyor error: | at the start is not allowed.\n");
                        CONV_flag = 1;
                    }
                }
                if (count == line_size) {
                    line_size *= 2;
                    the_string = (char *) realloc(the_string, line_size * sizeof(char));
                    if (the_string == NULL) {
                        fprintf(stderr, "Conveyor error: Couldn't reallocate the string.\n");
                        CONV_flag = 1;
                    }
                }
                if (c == '\t')
                    c = ' ';
                if (c != ' ' || prev == 0)
                    the_string[count++] = c;
                if (c == ' ')
                    prev = 1;
                else
                    prev = 0;
            }
        } /* while */
        if (c == EOF)
            EOF_flag = 1;
        if (the_string[count - 1] == '>' || the_string[count - 2] == '>') {
            fprintf(stderr, "Conveyor error: > at the end is not allowed.\n");
            CONV_flag = 1;
        }
        if (the_string[count - 1] == '<' || the_string[count - 2] == '<') {
            fprintf(stderr, "Conveyor error: < at the end is not allowed.\n");
            CONV_flag = 1;
        }
        if (CONV_flag) {
            free(the_string);
            the_string = NULL;
            return the_string;
        }
        if (seen_character)
            (*block_count)++;
        else if (seen_character == 0) {
            if (sep_count != 0)
                fprintf(stderr, "Conveyor error: | at the end is not allowed.\n");
            free(the_string);
            the_string = NULL;
            return the_string;
        }
        the_string = (char *) realloc(the_string, (count + 1) * sizeof(char));
        if (the_string == NULL) {
            fprintf(stderr, "Conveyor error: Couldn't reallocate the string.\n");
            free(the_string);
            the_string = NULL;
            return the_string;
        }
        the_string[count] = '\0';
        return the_string;
    }
    free(the_string);
    the_string = NULL;
    fprintf(stderr, "Conveyor error: No input available. The EOF is reached.\n");
    return the_string;
}

char ***sep_cmd(char *string, int block_count, int *counts) {
    char **solid_blocks = NULL;
    char **separated_block = NULL;
    char ***separated_blocks = NULL;
    int i, j, number, count;

    /* Разделяем начальную строку на целые блоки */
    solid_blocks = (char **) malloc(block_count * sizeof(char *));
    if (solid_blocks == NULL) {
        fprintf(stderr, "sep_cmd error: couldn't allocate the memory for solid_blocks.\n");
        free(string);
        return NULL;
    }
    for (number = 0; number < block_count; number++) {
        if (number == 0)
            solid_blocks[number] = strtok(string, "|");
        else 
            solid_blocks[number] = strtok(NULL, "|");
    }
    /* ----------------------------------------- */

    /* Разделяем целый блок на отдельные команды и параметры */
    separated_blocks = (char ***) malloc(block_count * sizeof(char **));
    if (separated_blocks == NULL) {
        fprintf(stderr, "sep_cmd error: could't allocate the memory for separated blocks.\n");
        free(solid_blocks);
        solid_blocks = NULL;
        free(string);
        return NULL;
    }
    for (number = 0; number < block_count; number++) {
        count = word_count(solid_blocks[number]);
        counts[number] = count;
        separated_block = (char **) malloc((count + 1) * sizeof(char *));
        if (separated_block == NULL) {
            fprintf(stderr, "sep_cmd error: could't allocate the memory for separated blocks.\n");
            for (j = 0; j < number; j++) {
                free(separated_blocks[j]);
                separated_blocks[j] = NULL;
            }
            free(separated_blocks);
            separated_blocks = NULL;
            free(solid_blocks);
            solid_blocks = NULL;
            free(string);
            return NULL;
        }
        for (i = 0; i < count; i++) {
            if (i == 0)
                separated_block[i] = strtok(solid_blocks[number], " ");
            else
                separated_block[i] = strtok(NULL, " ");
        }
        separated_block[i] = NULL;
        separated_blocks[number] = separated_block;
    }
    /* ----------------------------------------------------- */
    free(solid_blocks);
    solid_blocks = NULL;
    return separated_blocks;
}

int word_count(char *string) {
    int c, prev = 1, words = 0;

    while (c = *string++) {
        if (c != ' ' && prev == 1) {
            words++;
            prev = 0;
        }
        if (c == ' ')
            prev = 1;
    }
    return words;
}

int check_for_redir(char **cmds, int *cmd_amount, int *redir) {
    int j, i, temp;

    for (i = 0; i < 3; i++) {
        redir[i] = -1;
    }
    for (j = 0; j < *cmd_amount; j++) {
        if (strcmp(cmds[j], ">") == 0 && redir[1] == -1) {
            delete_excess(&cmds, *cmd_amount, j);
            (*cmd_amount)--;
            temp = open(cmds[j], O_RDWR | O_TRUNC);
            if (temp == -1) {
                if (!check_for_synt(cmds[j])) {
                    temp = creat(cmds[j], 0666);
                    if (temp == -1) {
                        fprintf(stderr, "Redir error: couldn't open or create \'%s\' directory.\n", cmds[j]);
                        return 1; 
                    }
                } else {
                    fprintf(stderr, "Redir error: couldn't open or create \'%s\' directory.\n", cmds[j]);
                    return 1;
                }
            }
            delete_excess(&cmds, *cmd_amount, j);
            (*cmd_amount)--;
            j--;
            redir[1] = temp;
        } else if (strcmp(cmds[j], ">>") == 0 && redir[1] == -1) {
            delete_excess(&cmds, *cmd_amount, j);
            (*cmd_amount)--;
            temp = open(cmds[j], O_RDWR | O_APPEND);
            if (temp == -1) {
                if (!check_for_synt(cmds[j])) {
                    temp = creat(cmds[j], 0666);
                    if (temp == -1) {
                        fprintf(stderr, "Redir error: couldn't open or create \'%s\' directory.\n", cmds[j]);
                        return 1; 
                    }
                } else {
                    fprintf(stderr, "Redir error: couldn't open or create \'%s\' directory.\n", cmds[j]);
                    return 1;
                }
            }
            delete_excess(&cmds, *cmd_amount, j);
            (*cmd_amount)--;
            j--;
            redir[1] = temp;
        } else if (strcmp(cmds[j], "<") == 0 && redir[0] == -1) {
            delete_excess(&cmds, *cmd_amount, j);
            (*cmd_amount)--;
            temp = open(cmds[j], O_RDWR);
            if (temp == -1) {
                fprintf(stderr, "Redir error: couldn't open \'%s\' directory.\n", cmds[j]);
                return 1;
            }
            delete_excess(&cmds, *cmd_amount, j);
            (*cmd_amount)--;
            j--;
            redir[0] = temp;
        } else if (strcmp(cmds[j], "2>") == 0 && redir[2] == -1) {
            delete_excess(&cmds, *cmd_amount, j);
            (*cmd_amount)--;
            temp = open(cmds[j], O_RDWR | O_TRUNC);
            if (temp == -1) {
                if (!check_for_synt(cmds[j])) {
                    temp = creat(cmds[j], 0666);
                    if (temp == -1) {
                        fprintf(stderr, "Redir error: couldn't open or create \'%s\' directory.\n", cmds[j]);
                        return 1; 
                    }
                } else {
                    fprintf(stderr, "Redir error: couldn't open or create \'%s\' directory.\n", cmds[j]);
                    return 1;
                }
            }
            delete_excess(&cmds, *cmd_amount, j);
            (*cmd_amount)--;
            j--;
            redir[2] = temp;
        }  
    } /* For commands in block */
    return 0;
}

int check_for_synt(char *cmd) {
    return (strcmp(cmd, ">") == 0) || (strcmp(cmd, ">>") == 0) || (strcmp(cmd, "<") == 0) || (strcmp(cmd, "2>") == 0);
}

void delete_excess(char ***cmds, int length, int count){
    int j;

    for (j = count; j < length; j++){
        (*cmds)[j] = (*cmds)[j + 1];
    }
}

void hndlr1(int n) {
    flagC = 1;
    signal(SIGINT, hndlr1);
}

void hndlr2(int n) {
    flagZ = 1;
    signal(SIGTSTP, hndlr2);
}

void exec_pwd(void) {
    char cwd[1024];

    getcwd(cwd, sizeof(cwd));
    printf("PWD: %s\n", cwd);
    return;
}

void exec_cd(char *dir) {
    if (chdir(dir) != 0)
        fprintf(stderr, "Couldn't change the directory.\n");
    return;
}