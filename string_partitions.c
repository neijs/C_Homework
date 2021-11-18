#include "normal_input.h"

#define CHUNK_SIZE 5
#define WORD_SIZE 1
#define IN 1
#define OUT 0

static char *original_string;
static int amount;

static char argv_splitter;
static char argv_option;
static int argv_N_threshold;
static int argv_use = 1;

int choice(char *);
int we_dont_like_long_words_here(int, char **);
void sort_words(char **);
int compare_words(char *, char *);
int measure_word(char *);
char **get_words(char *, char);
void reverse(char **);
void print_words(char **);

int main(int argc, char *argv[]) {
    char *splitter;
    int c,i;

    if (argc > 2) {
        if (sscanf (argv[1], "%c", &argv_splitter) != 1) {
            printf ("Invalid argv input.\n");
            return 0;
        }
        if (sscanf (argv[2], "%c", &argv_option) != 1 || argv_option < '1' || argv_option > '3') {
            printf ("Invalid argv input.\n");
            return 0;
        }
        if (argv_option == '3' && argc != 4) {
            printf ("Invalid argv input.\n");
            return 0;
        }
        if (argc >= 4)
            if (argv_option != '3' || sscanf (argv[3], "%d", &argv_N_threshold) != 1) {
                printf ("Invalid argv input.\n");
                return 0;
            }
    } else
        argv_use = 0;
    printf("\nEnter the string: ");
    if (!(original_string = get_line(CHUNK_SIZE))) { 
        puts("\nThe string is empty.");
        return 1;
    }
    do {
        if (argv_use)
            splitter = &argv_splitter;
        else {
            printf("\nEnter the splitter ('q' -- to quit): ");
            if (!(splitter = get_char(1)))
                return 1;
        }
    } while (choice(splitter));
    free(splitter);
    free(original_string);
    return 0;
}

int choice(char *splitter) {
    char **a_bag_of_words = NULL;
    char *option = NULL;
    int *N = NULL;
    int i;

    if (*splitter == 'q')
        return 0;
    amount = 0;
    if (!(a_bag_of_words = get_words(original_string, *splitter))) {
        if (argv_use)
            argv_use = 0;
        printf("\nThere're no words.");
        return 1;
    }
    print_words(a_bag_of_words);
    do {
        if (argv_use)
            option = &argv_option;
        else {
            if (option)
                free(option);
            printf("\nSelect one of the options: \n"
                "[1] -- sort the array of words in lexico-graphical ascending order.\n"
                "[2] -- sort the array of words in lexico-graphical descending order.\n"
                "[3] -- delete words from array, the length of which is more than N characters.\n");
            if (!(option = get_char(1)))
                return 0;
            if (*option != '1' && *option != '2' && *option != '3')
                printf("\n[%c] option does not exist. Choose an existing one: ", *option);
        }
    } while (*option != '1' && *option != '2' && *option != '3');
    switch (*option) {
        case '1':
            sort_words(a_bag_of_words);
            break;
        case '2':
            sort_words(a_bag_of_words);
            reverse(a_bag_of_words);
            break;
        case '3':
            if (argv_use) {
                if (we_dont_like_long_words_here(argv_N_threshold, a_bag_of_words))
                    return 0;
            } else {
                printf("\nEnter the N threshold: ");
                if (!(N = get_int(1)))
                    return 0;
                if (we_dont_like_long_words_here(*N, a_bag_of_words))
                    return 0;
                free(N);
            }
            break;
    }
    if (argv_use)
        argv_use = 0;
    else { 
        free(option);
        free(splitter);
    }
    print_words(a_bag_of_words);
    for (i = 0; i < amount; i++)
        free(a_bag_of_words[i]);
    if (amount)
        free(a_bag_of_words);
    return 1;
}

int we_dont_like_long_words_here(int limit, char **collection) {
    int i, j;

    for (i = 0; i < amount; i++)
        if (measure_word(collection[i]) > limit) {
            free(collection[i]);
            collection[i] = NULL;
            amount--;
            for (j = i--; j < amount; j++)
                collection[j] = collection[j + 1];
        }
    collection = (char **) realloc(collection, amount * sizeof(char*));
    if (collection == NULL && amount != 0) {
        printf("\nerror: Couldn't allocate the memory.");
        free(collection);
        return 1;
    }
    return 0;
}

void sort_words(char **collection) {
    int i, j;
    char *tmp_word;

    for (i = 0; i < amount - 1; i++)
        for (j = 0; j < amount - i - 1; j++)
            if (compare_words(collection[j], collection[j + 1]) > 0) {
                tmp_word = collection[j];
                collection[j] = collection[j + 1];
                collection[j + 1] = tmp_word;
            }
}

int compare_words(char *first, char *second) {
    for (; *first == *second; first++, second++)
        if (*first == '\0')
            return 0;
    return *first - *second;
}

int measure_word(char *word) {
    int length;

    for (length = 0; *word++; length++);
    return length;
}

char **get_words(char *the_string, char splitter) {
    int c, state = OUT, length = 0; 
    int word_size = WORD_SIZE, chunk_size = CHUNK_SIZE;
    char *word = NULL;
    char **collection = (char **) malloc(CHUNK_SIZE * sizeof(char *));

    if (collection == NULL) {
        printf("\nerror: Couldn't allocate the memory.");
        free(collection);
        return NULL;
    }
    while (c = *the_string++) {
        if (c == splitter && word) {
            state = OUT;
            word = (char *) realloc(word, (length + 1) * sizeof(char));
            if (word == NULL) {
                printf("\nerror: Couldn't allocate the memory.");
                free(word);
                return NULL;
            }
            word[length] = '\0';
            if (amount == chunk_size) { /* Not enough space in the collection for the next word */
                chunk_size *= 2;
                collection = (char **) realloc(collection, chunk_size * sizeof(char *));
                if (collection == NULL) {
                    printf("\nerror: Couldn't allocate the memory.");
                    free(collection);
                    return NULL;
                }
            }
            collection[amount++] = word; /* Add the word in the collection */
            length = 0;
            word = NULL;
        } else if (c != splitter) {
            if (state == OUT) { /* Create a new word */
                word = (char *) malloc(word_size * sizeof(char));
                if (word == NULL) {
                    printf("\nerror: Couldn't allocate the memory.");
                    free(word);
                    return NULL;
                }
                state = IN;
            }
            if (length == word_size) { /* Not enough space in the word for the next character */
                word_size *= 2;
                word = (char *) realloc(word, word_size * sizeof(char));
                if (word == NULL) {
                    printf("\nerror: Couldn't allocate the memory.");
                    free(word);
                    return NULL;
                }
            }
            word[length++] = c; /* Add the character in the word */
        }
    }
    if (state == IN) { /* Process the last word in the string */
        word = (char *) realloc(word, (length + 1) * sizeof(char));
        if (word == NULL) {
            printf("\nerror: Couldn't allocate the memory.");
            free(word);
            return NULL;
        }
        word[length] = '\0';
            if (amount == chunk_size) {
                chunk_size *= 2;
                collection = (char **) realloc(collection, chunk_size * sizeof(char *));
                if (collection == NULL) {
                    printf("\nerror: Couldn't allocate the memory.");
                    free(collection);
                    return NULL;
                }
            }
        collection[amount++] = word;
    }
    if (!amount) { /* no words found */
        free(collection);
        return NULL;
    } 
    collection = (char **) realloc(collection, amount * sizeof(char *)); 
    if (collection == NULL) {
        printf("\nerror: Couldn't allocate the memory.");
        free(collection);
        return NULL;
    }
    return collection;
}

void reverse(char **collection) {
    int start = 0, end = amount - 1;
    char *tmp_word;

    while (start < end) {
        tmp_word = collection[start];
        collection[start++] = collection[end];
        collection[end--] = tmp_word;
    }
}

void print_words(char **collection) {
    int i; 
    char *word;

    printf("\n[");
    for (i = 0; i < amount; i++) {
        word = collection[i];
        printf("\"");
        while (*word)
            putchar(*word++);
        if (i == amount - 1)
            printf("\"");
        else
            printf("\", ");
    }
    printf("].\n");
}