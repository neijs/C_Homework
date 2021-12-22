#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define LINE_SIZE 30

int EOF_flag = 0;

char *get_line(int, int*);
char ***sep_cmd(char *, int, int*);
int word_count(char *);

int main() {
    int block_count, i, j, k;
    int *counts = NULL;
    char *string = NULL;
    char ***commands = NULL;

    while (1) {
        printf("$");
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
            printf("Error in main: couldn't allocate the memory for counts.\n");
            free(string);
            string = NULL;
            free(counts);
            counts = NULL;
            continue;
        }
        commands = sep_cmd(string, block_count, counts);
        if (commands == NULL) {
            string = NULL;
            continue;
        }
        for (i = 0; i < block_count; i++) {
            puts("-------------");
            for (j = 0; j < counts[i]; j++) {
                puts(commands[i][j]);
            }
            puts("-------------");
        }

        /* Все отработало гладко, осталось все освободить */
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
        /* ---------------------------------------------- */

        if (EOF_flag == 1) {
            printf("EOF is reached.\n");
            break;
        }
    }
    return 0;
}

/* Get the conveyor input and count the amount of blocks */
char *get_line(int line_size, int *block_count) {
    int c, count = 0, seen_character = 0, sep_count = 0, prev = 0;
    char *the_string = (char *) malloc(line_size * sizeof(char));
    int CONV_flag = 0;

    if (!EOF_flag) {
        if (the_string == NULL) {
            printf("Conveyor error: Couldn't allocate the memory for string.\n");
            free(the_string);
            the_string = NULL;
            return the_string;
        }
        *block_count = 0;
        while ((c = getchar()) != '\n' && c != EOF) {
            if (CONV_flag == 0) {
                if (c != ' ' && c != '\t' && c != '|') {
                    if (c == '>') {
                        if (the_string[count - 1] != ' ' && the_string[count - 1] != '>') {
                            the_string[count++] = ' ';
                        }
                    } else if (the_string[count - 1] == '>')
                        the_string[count++] = ' ';
                    if (seen_character == 0)
                        seen_character = 1;
                    sep_count = 0;
                } else if (c == '|') {
                    sep_count++;
                    if (seen_character) {
                        (*block_count)++;
                        seen_character = 0;
                    } else {
                        if (sep_count > 1)
                            printf("Conveyor error: || is not allowed.\n");
                        else 
                            printf("Conveyor error: | at the start is not allowed.\n");
                        CONV_flag = 1;
                    }
                }
                if (count == line_size) {
                    line_size *= 2;
                    the_string = (char *) realloc(the_string, line_size * sizeof(char));
                    if (the_string == NULL) {
                        printf("Conveyor error: Couldn't reallocate the string.\n");
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
        if (CONV_flag) {
            free(the_string);
            the_string = NULL;
            return the_string;
        }
        if (seen_character)
            (*block_count)++;
        else if (seen_character == 0) {
            if (sep_count != 0)
                printf("Conveyor error: | at the end is not allowed.\n");
            free(the_string);
            the_string = NULL;
            return the_string;
        }
        the_string = (char *) realloc(the_string, (count + 1) * sizeof(char));
        if (the_string == NULL) {
            printf("Conveyor error: Couldn't reallocate the string.\n");
            free(the_string);
            the_string = NULL;
            return the_string;
        }
        the_string[count] = '\0';
        return the_string;
    }
    free(the_string);
    the_string = NULL;
    puts("Conveyor error: No input available. The EOF is reached.\n");
    return the_string;
}


/* Akkuratno blyat vse razdelyaem */
char ***sep_cmd(char *string, int block_count, int *counts) {
    char **solid_blocks = NULL;
    char **separated_block = NULL;
    char ***separated_blocks = NULL;
    int i, j, number, count;

    /* Разделяем начальную строку на целые блоки */
    solid_blocks = (char **) malloc(block_count * sizeof(char *));
    if (solid_blocks == NULL) {
        printf("sep_cmd error: couldn't allocate the memory for solid_blocks.\n");
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
        printf("sep_cmd error: could't allocate the memory for separated blocks.\n");
        free(solid_blocks);
        free(string);
        solid_blocks = NULL;
        return NULL;
    }
    for (number = 0; number < block_count; number++) {
        count = word_count(solid_blocks[number]);
        counts[number] = count;
        separated_block = (char **) malloc(count * sizeof(char *));
        if (separated_block == NULL) {
            printf("sep_cmd error: could't allocate the memory for separated blocks.\n");
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