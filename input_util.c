static int EOF_reached = 0;

double *get_double(int amount) {
    int recieved = 0, seen_digit, invalid_input = 0, sign, c;
    double value, power;
    double *values = (double *) malloc(amount * sizeof(double));

    if (!EOF_reached)
        if (values == NULL) {
            printf("\nerror: Couldn't allocate the memory.");
            free(values);
            return NULL;
        }
        while (1) {
            seen_digit = 0, value = 0.0, power = 1.0;
            while ((c = getch()) == ' '); /* skip white spaces */

            sign = (c == '-') ? -1 : 1;
            if (c != '+' && c != '-')
                ungetch(c);
            while (isdigit(c = getch())) {
                seen_digit = 1;
                value = 10.0 * value + (c - '0');
            }
            if (c == '.')
                while (isdigit(c = getch())) {
                    seen_digit = 1;
                    value = 10.0 * value + (c - '0');
                    power *= 10;
                }
            if (c != EOF && c != '\n' && c != ' ' && c != '\t') {
                invalid_input = 1;
            } 
            if (seen_digit && !invalid_input) {
                if (recieved < amount)
                    values[recieved] = sign * value / power;
                recieved++;
            }
            if (c == EOF || c == '\n') {
                if (recieved == amount && !invalid_input) {
                    if (c == EOF)
                        EOF_reached = 1;
                    return values;
                }
                if (invalid_input)
                    printf("\nerror: invalid input. ");
                else if (recieved < amount)
                    printf("\nerror: not enough input. ");
                else if (recieved > amount)
                    printf("\nerror: extra input. ");
                if (c == EOF) {
                    puts("The EOF is reached");
                    free(values);
                    return NULL;
                } else {
                    printf("Try again: ");
                    recieved = 0;
                    invalid_input = 0;
                    continue;
                }
            }
        }
    free(values);
    puts("\nerror: No input available. The EOF is reached.");
    return NULL;
}

short *get_short(int amount) {
    int recieved = 0, seen_digit, invalid_input = 0, sign, c;
    short value;
    short *values = (short *) malloc(amount * sizeof(short));

    if (!EOF_reached)
        if (values == NULL) {
            printf("\nerror: Couldn't allocate the memory.");
            free(values);
            return NULL;
        }
        while (1) {
            seen_digit = 0, value = 0;
            while ((c = getch()) == ' '); /* skip white spaces */

            sign = (c == '-') ? -1 : 1;
            if (c != '+' && c != '-')
                ungetch(c);
            while (isdigit(c = getch())) {
                seen_digit = 1;
                value = 10 * value + (c - '0');
            }
            if (c != EOF && c != '\n' && c != ' ' && c != '\t') {
                invalid_input = 1;
            } 
            if (seen_digit && !invalid_input) {
                if (recieved < amount)
                    values[recieved] = sign * value;
                recieved++;
            }
            if (c == EOF || c == '\n') {
                if (recieved == amount && !invalid_input) {
                    if (c == EOF)
                        EOF_reached = 1;
                    return values;
                }
                if (invalid_input)
                    printf("\nerror: invalid input. ");
                else if (recieved < amount)
                    printf("\nerror: not enough input. ");
                else if (recieved > amount)
                    printf("\nerror: extra input. ");
                if (c == EOF) {
                    puts("The EOF is reached");
                    free(values);
                    return NULL;
                } else {
                    printf("Try again: ");
                    recieved = 0;
                    invalid_input = 0;
                    continue;
                }
            }
        }
    free(values);
    puts("\nerror: No input available. The EOF is reached.");
    return NULL;
}

char *get_char(int amount) {
    int recieved = 0, seen_character, invalid_input = 0, c;
    int value;
    char *values = (char *) malloc(amount * sizeof(char));

    if (!EOF_reached) {
        if (values == NULL) {
            printf("\nerror: Couldn't allocate the memory.");
            free(values);
            return NULL;
        }
        printf("(If you would like to input a space, type ' '): ");
        while (1) {
            seen_character = 0, value = -1;
            while ((c = getch()) == ' '); /* skip white spaces */
            ungetch(c);
            while ((c = getch()) != EOF && c != '\n' && c != ' ' && c != '\t') {
                if (c == '\'') /* Process the space character */
                    if ((c = getch()) != ' ') {
                        ungetch(c);
                        c = '\'';
                    } else
                        if ((c = getch()) != '\'') {
                            ungetch(c);
                            ungetch(' ');
                            c = '\'';
                        } else 
                            value = ' ';
                if (seen_character)
                    invalid_input = 1;
                if (value == -1)
                    value = c;
                seen_character = 1;
            } /* while */
            if (seen_character && !invalid_input) {
                if (recieved < amount)
                    values[recieved] = value;
                recieved++;
            }
            if (c == EOF || c == '\n') {
                if (recieved == amount && !invalid_input) {
                    if (c == EOF)
                        EOF_reached = 1;
                    return values; /* If everything was fine, we would left here */
                }
                if (invalid_input) /* Tell the user where are they wrong */
                    printf("\nerror: invalid input. ");
                else if (recieved < amount)
                    printf("\nerror: not enough input. ");
                else if (recieved > amount)
                    printf("\nerror: extra input. ");
                if (c == EOF) {
                    puts("The EOF is reached");
                    free(values);
                    return NULL;
                } else {
                    printf("Try again: ");
                    recieved = 0;
                    invalid_input = 0;
                    continue;
                }
            }
        }
    }
    free(values);
    puts("\nerror: No input available. The EOF is reached.");
    return NULL;
}

char *get_line(int chunk_size) {
    int count = 0, c, seen_character = 0;
    char *the_string = (char *) malloc(chunk_size * sizeof(char));

    if (!EOF_reached) {
        if (the_string == NULL) {
            printf("\nerror: Couldn't allocate the memory.");
            free(the_string);
            return NULL;
        }
        while ((c = getchar()) != '\n' && c != EOF) {
            seen_character = 1;
            if (count == chunk_size) {
                chunk_size *= 2;
                the_string = (char *) realloc(the_string, chunk_size * sizeof(char));
                if (the_string == NULL) {
                    printf("\nerror: Couldn't allocate the memory.");
                    free(the_string);
                    return NULL;
                }
            }
            the_string[count++] = c;
        }
        if (c == EOF)
            EOF_reached = 1;
        if (!seen_character) {
            free(the_string);
            return NULL;
        }
        the_string = (char *) realloc(the_string, (count + 1) * sizeof(char));
        if (the_string == NULL) {
            printf("\nerror: Couldn't allocate the memory.");
            free(the_string);
            return NULL;
        }
        the_string[count] = '\0';
        return the_string;
    }
    free(the_string);
    puts("\nerror: No input available. The EOF is reached.");
    return NULL;
}

int *get_int(int amount) {
    int recieved = 0, seen_digit, invalid_input = 0, sign, c;
    int value;
    int *values = (int *) malloc(amount * sizeof(int));
    
    if (!EOF_reached)
        if (values == NULL) {
            printf("\nerror: Couldn't allocate the memory.");
            free(values);
            return NULL;
        }
        while (1) {
            seen_digit = 0, value = 0;
            while ((c = getch()) == ' '); /* skip white spaces */

            sign = (c == '-') ? -1 : 1;
            if (c != '+' && c != '-')
                ungetch(c);
            while (isdigit(c = getch())) {
                seen_digit = 1;
                value = 10 * value + (c - '0');
            }
            if (c != EOF && c != '\n' && c != ' ' && c != '\t') {
                invalid_input = 1;
            } 
            if (seen_digit && !invalid_input) {
                if (recieved < amount)
                    values[recieved] = sign * value;
                recieved++;
            }
            if (c == EOF || c == '\n') {
                if (recieved == amount && !invalid_input) {
                    if (c == EOF)
                        EOF_reached = 1;
                    return values;
                }
                if (invalid_input)
                    printf("\nerror: invalid input. ");
                else if (recieved < amount)
                    printf("\nerror: not enough input. ");
                else if (recieved > amount)
                    printf("\nerror: extra input. ");
                if (c == EOF) {
                    puts("The EOF is reached");
                    free(values);
                    return NULL;
                } else {
                    printf("Try again: ");
                    recieved = 0;
                    invalid_input = 0;
                    continue;
                }
            }
        }
    free(values);
    puts("\nerror: No input available. The EOF is reached.");
    return NULL;
}