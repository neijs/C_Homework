#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

struct config_pair {
    char letter_old, letter_new;
};

struct config_pair *read_config(char *, int *);
int process(struct config_pair, char *, int);
int is_splitter(char);
int read_word(int);

int main(int argc, char **argv) {
    pid_t pid;
    int i, kids = 0, log;
    struct config_pair *config_collection = NULL;

    if (argc > 3) {
        printf("Too many arguments in argv. You should enter <subject_file> name & <config_file> name next time\n");
        return 1;
    } else if (argc < 3) {
        printf("Too few arguments in argv. You should enter <subject_file> name & <config_file> name next time\n");
        return 1;
    }
    if (!(config_collection = read_config(argv[2], &kids)))
        return 1; /* Some troubles with config file */
    for (i = kids - 1; i > 0; i--) {
        pid = fork();
        if (pid == -1) /* Error */
            printf("Warning: Kid #%d wasn't created! %s\n", i, strerror(errno));
        if (pid == 0) /* Kid */
            break;
    }
    log = process(config_collection[i], argv[1], i); /* Change the file according to the rule */
    free(config_collection);
    config_collection = NULL;
    if (i == 0) { /* Dad */
        if (log)
            printf("Warning: The father may not have changed something.\n");
        while (wait(NULL) != -1);
        printf("All children have done working\n");
    } else {/* Kiddos */
        if (log)
            printf("Warning: Kid #%d may not have changed something.\n", i);
        printf("Child process #%d is finished\n", i);
    }
    return 0;
}

struct config_pair *read_config(char *config_name, int *kids) {
    struct config_pair *config_collection = NULL;
    FILE *config = fopen(config_name, "r");
    int c, count = 0, j;

    if (!config) { /* Couldn't open the config file */
        printf("An error has occured while opennig the config file: %s\n", strerror(errno));
        return NULL;
    }
    if (fscanf(config, "%d", kids) != 1) {
        printf("Error in the config_file: Process counter is not found.\n");
        fclose(config);
        return NULL;
    }
    if (*kids < 1) {
        printf("Error in the config_file: Value of process counter is too low.\n");
        fclose(config);
        return NULL;
    }
    while ((c = fgetc(config)) == ' ' || c == '\t'); /* skip white spaces */
    if (c != '\n') {
        printf("Error in the config_file: Extra input in the first line.\n");
        fclose(config);
        return NULL;
    }
    config_collection = (struct config_pair *) malloc(sizeof(struct config_pair) * (*kids));
    if (config_collection == NULL) {
        printf("Error: Couldn't allocate memory for config collection.\n");
        fclose(config);
        return NULL;
    }
    while (1) { 
        if (count == *kids) {
           while ((c = fgetc(config)) == ' ' || c == '\t' || c == '\n'); /* skip white spaces */
           if (c != EOF) {
               printf("Error in the config_file: Extra lines are found.\n");
               free(config_collection);
               config_collection = NULL;
               fclose(config);
               return NULL;
           } 
           break;
        }
        while ((c = fgetc(config)) == ' ' || c == '\t'); /* skip white spaces */
        if (c == '\n' || c == EOF) {
            printf("Error in the config_file: The line is empty.\n");
            free(config_collection);
            config_collection = NULL;
            fclose(config);
            return NULL;
        }
        config_collection[count].letter_old = c; /* Successfully read the letter_old */
        if (config_collection[count].letter_old == '*') {
            printf("Error in the config_file: Letter_old == '*' is not allowed.\n");
            free(config_collection);
            config_collection = NULL;
            fclose(config);
            return NULL;
        }
        while ((c = fgetc(config)) == ' ' || c == '\t'); /* skip white spaces */
        if (c == '\n' || c == EOF) {
            printf("Error in the config_file: Half-hearted rules are not allowed.\n");
            free(config_collection);
            config_collection = NULL;
            fclose(config);
            return NULL;
        }
        config_collection[count].letter_new = c; /* Successfully read the letter_new */
        while ((c = fgetc(config)) == ' ' || c == '\t'); /* skip white spaces */
        if (c != EOF && c != '\n') {
            printf("Error in the config_file: Extra information in the line.\n");
            free(config_collection);
            config_collection = NULL;
            fclose(config);
            return NULL;
        }
        for (j = 0; j < count; j++) {
            if (config_collection[j].letter_old == config_collection[count].letter_old) {
                printf("Error in the config_file: Different rules for identical letters are not allowed.\n");
                free(config_collection);
                config_collection = NULL;
                fclose(config);
                return NULL;
            }
        }
        count++;
    }
    fclose(config);
    return config_collection;
}

int process(struct config_pair rule, char *file, int badge_number) {
    char c;
    int length = 0, subject;
    char badge[8] = "";
    off_t start;
    ssize_t log;

    if ((subject = open(file, O_RDWR)) == -1) {
        printf("An error has occured while opennig the subject file: %s\n", strerror(errno));
        return 1;
    }
    if (badge_number)
        sprintf(badge, "SON%d", badge_number);
    else
        sprintf(badge, "FATHER"); 
    while ((log = read(subject, &c, sizeof(char))) > 0) {
        if (!is_splitter(c)) {
            lseek(subject, -1, SEEK_CUR);
            start = lseek(subject, 0, SEEK_CUR);
            if ((length = read_word(subject)) >= 10 && c == rule.letter_old) {
                if (write(subject, "*", sizeof(char)) == -1) {
                    printf("An error has occured while changing the subject file: %s\n", strerror(errno));
                    close(subject);
                    return 1;
                }
                if (write(subject, &(rule.letter_new), sizeof(char)) == -1) {
                    printf("An error has occured while changing the subject file: %s\n", strerror(errno));
                    close(subject);
                    return 1;
                }
                if (lseek(subject, start + length - strlen(badge), SEEK_SET) == -1) {
                    printf("An error has occured while changing the subject file: %s\n", strerror(errno));
                    close(subject);
                    return 1;
                }
                if (write(subject, badge, sizeof(char) * strlen(badge)) == -1) {
                    printf("An error has occured while changing the subject file: %s\n", strerror(errno));
                    close(subject);
                    return 1;
                }
            } else {
                if (lseek(subject, start + length, SEEK_SET) == -1) {
                    printf("An error has occured while changing the subject file: %s\n", strerror(errno));
                    close(subject);
                    return 1;
                }
            }
        }
    }
    if (log == -1) {
        printf("An error has occured while changing the subject file: %s\n", strerror(errno));
        close(subject);
        return 1;
    }
    close(subject);
    return 0;
}

int is_splitter(char c) {
    return (c == ' ' || c == '\t' || c == '.' || c == ',' || c == '\n');
}

int read_word(int fd) {
    char c = 0;
    off_t start = lseek(fd, 0, SEEK_CUR);
    int length = 0;

    while (read(fd, &c, sizeof(char)) > 0) {
        if (is_splitter(c))
            break;
        length++;
    }
    lseek(fd, start, SEEK_SET);
    return length;
}