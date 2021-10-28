#include <stdio.h>
#include <ctype.h>

#define MAXLEN 500

static int EOF_flag = 1;

int get_line (char *, FILE*, int);
double atof(char *, int *);
char atoc(char *, int *);
int capable_double_input(double*, int);
int capable_char_input(char*, int);
short atosh(char *, int *);

/* Extract a line from the temporary file. The lines are separated by a space */
int get_line(char *str, FILE *file, int lim) {
    int c, i;

    while ((c = fgetc(file)) == ' '); /* skip white spaces */
    if (c != EOF) { /*If EOF wasn't reached yet, return the file position indicator to the previous character */
        fseek(file, ftell(file) - 1, SEEK_SET); 
        while (--lim > 0 && (c = fgetc(file)) != EOF && c != '\n' && c != ' ') /* Copy the line from file to a string */
            *str++ = c;
        return 0;
    } else 
        return EOF;
}

/* Get <amount> double values, using a temporary file */
int capable_double_input(double *values, int amount) {
    FILE* file = tmpfile(); /* Create the temporary file */
    int c, error_flag = 0, i; /* c for storing a current character, error_flag for recognizing input errors */
    char line[MAXLEN] = {0};

    if (file == NULL) { /* Exit the function, if temporary file for some reason wasn't created */
        puts("\nERROR (capable_double_input): Couldn't create temporary file!");
        return 1;
    }
    while ((c = getchar()) != '\n' && c != EOF) /* Store the input data in the temporary file */
        fputc(c, file);
    if (c == EOF) /* Check whether the user entered the EOF */
        EOF_flag = EOF;
    rewind(file); /* Return the file position indicator to the beginning of the temporary file */
    while (get_line(line, file, MAXLEN) != EOF){ /* As long as there is something to read from the temporary file */
        if (amount == 0) { /* Extra input */
            puts("\nERROR (capable_double_input): Extra input. Try to FINALLY enter the correct amount of values!");
            return EOF_flag;
        }
        *values++ = atof(line, &error_flag); /* Save the received value */
        if (error_flag) { /* Atof registered input error */
            puts("\nERROR (capable_double_input): Invalid input. Try to FINALLY enter the correct values!");
            return EOF_flag;
        }
        amount--;
        for (i = 0; i < MAXLEN; i++) /* Clean the line, for the future input */
            line[i] = '\0';
    }
    if (amount != 0) { /* Not enough input */
        puts("\nERROR (capable_double_input): Not enough input. Try to FINALLY enter the correct amount of values!");
        return EOF_flag;
    }
    fclose(file);
    EOF_flag = 1; /* Renew EOF_flag */
    return 0;
}

/* Get <amount> char values, using a temporary file */
int capable_char_input(char *symbols, int amount) {
    FILE* file = tmpfile(); /* Create the temporary file */
    int c, error_flag = 0, i; /* c for storing a current character, error_flag for recognizing input errors */
    char line[MAXLEN] = {0};

    if (file == NULL) { /* Exit the function, if temporary file for some reason wasn't created */
        puts("\nERROR (capable_char_input): Couldn't create temporary file!");
        return 1;
    }
    while ((c = getchar()) != '\n' && c != EOF) /* Store the input data in the temporary file */
        fputc(c, file);
    if (c == EOF) /* Check whether the user entered the EOF */
        EOF_flag = EOF;
    rewind(file); /* Return the file position indicator to the beginning of the temporary file */
    while (get_line(line, file, MAXLEN) != EOF){ /* As long as there is something to read from the temporary file */
        if (amount == 0){ /* Extra input */
            puts("\nERROR (capable_char_input): Extra input. Try to FINALLY enter the correct amount of values!");
            return EOF_flag;
        }
        *symbols++ = atoc(line, &error_flag); /* Save the received value */
        if (error_flag) { /* Atoc registered input error */
            puts("\nERROR (capable_char_input): Invalid input. Try to FINALLY enter the correct values!");
            return EOF_flag;
        }
        amount--;
        for (i = 0; i < MAXLEN; i++) /* Clean the line, for the future input */
            line[i] = '\0';
    }
    if (amount != 0) { /* Not enough input */
        puts("\nERROR (capable_char_input): Not enough input. Try to FINALLY enter the correct amount of values!");
        return EOF_flag;
    }
    fclose(file);
    EOF_flag = 1; /* Renew EOF_flag */
    return 0;
}

/* Get <amount> short values, using a temporary file */
int capable_short_input(short *values, int amount) {
    FILE* file = tmpfile(); /* Create the temporary file */
    int c, error_flag = 0, i; /* c for storing a current character, error_flag for recognizing input errors */
    char line[MAXLEN] = {0};

    if (file == NULL) { /* Exit the function, if temporary file for some reason wasn't created */
        puts("\nERROR (capable_short_input): Couldn't create temporary file!");
        return 1;
    }
    while ((c = getchar()) != '\n' && c != EOF) /* Store the input data in the temporary file */
        fputc(c, file);
    if (c == EOF) /* Check whether the user entered the EOF */
        EOF_flag = EOF;
    rewind(file); /* Return the file position indicator to the beginning of the temporary file */
    while (get_line(line, file, MAXLEN) != EOF){ /* As long as there is something to read from the temporary file */
        if (amount == 0){ /* Extra input */
            puts("\nERROR (capable_short_input): Extra input. Try to FINALLY enter the correct amount of values!");
            return EOF_flag;
        }
        *values++ = atosh(line, &error_flag); /* Save the received value */
        if (error_flag) { /* Atosh registered input error */
            puts("\nERROR (capable_short_input): Invalid input. Try to FINALLY enter the correct values!");
            return EOF_flag;
        }
        amount--;
        for (i = 0; i < MAXLEN; i++) /* Clean the line, for the future input */
            line[i] = '\0';
    }
    if (amount != 0) { /* Not enough input */
        puts("\nERROR (capable_short_input): Not enough input. Try to FINALLY enter the correct amount of values!");
        return EOF_flag;
    }
    fclose(file);
    EOF_flag = 1; /* Renew EOF_flag */
    return 0;
}

/* Extract the double value from the string */
double atof(char *str, int *error_flag) {
    double val, power;
    int sign;

    sign = (*str == '-') ? -1 : 1; /* If there's a sign, save it */
    if (*str == '+' || *str == '-')
        str++;
    for (val = 0.0; isdigit(*str); str++) /* Count the integer part of the number */
        val = 10.0 * val + (*str - '0');
    if (*str == '.')
        str++;
    else if (*str) { /* '\0'-- We reached the end of string, no errors are found */
        *error_flag = 1;
        return 1.0;
    }
    for (power = 1.0; isdigit(*str); str++) { /* Count the fractional part of the number */
        val = 10.0 * val + (*str - '0');
        power *= 10;
    }
    if (*str || !val){ /* '\0'-- We reached the end of string, no errors are found */
        *error_flag = 1;
        return 1.0;
    }
    return sign * val / power;
}

/* Extract the char value from the string */
char atoc(char *str, int *error_flag) {
    int c;

    c = *str;
    str++;
    if (*str) { /* '\0'-- We reached the end of string, no errors are found */
        *error_flag = 1;
        return 1;
    }
    return c;
}

/* Extract the short value from the string */
short atosh(char *str, int *error_flag) {
    short val, sign;

    sign = (*str == '-') ? -1 : 1; /* If there's a sign, save it */
    if (*str == '+' || *str == '-')
        str++;
    for (val = 0; isdigit(*str); str++) /* Count the integer part of the number */
        val = 10 * val + (*str - '0');
    if (*str){ /* '\0'-- We reached the end of string, no errors are found */
        *error_flag = 1;
        return 1;
    }
    return sign * val;
}