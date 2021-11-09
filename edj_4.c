#include <string.h>
#include <stdlib.h>
struct Node {
    struct Node *prev, *next;
    char *elem;
};

struct List {
    struct Node *first, *last;
};

void process(struct List *pl, const char *str) {
    struct Node *tmp = pl -> first, *step = NULL, *tmp_last = pl -> last;
    int check = 1;

    if (!(pl -> first) || !(pl -> last))
        return;
    while (check) {
        if (tmp == pl -> last)
            check = 0;
        step = tmp -> next;
        if (strcmp(tmp -> elem, str) == 0) {
            if (tmp -> prev)
                tmp -> prev -> next = tmp -> next;
            if (tmp -> next)
                tmp -> next -> prev = tmp -> prev;
            if (tmp == tmp_last)
                tmp_last = tmp -> prev;
            if (tmp == pl -> first)
                pl -> first = tmp -> next;
            free(tmp -> elem);
            free(tmp);
        } else if (strcmp(tmp -> elem, str) > 0) { // > 0
            if (tmp != tmp_last) {
                if (tmp -> prev)
                    tmp -> prev -> next = tmp -> next;
                if (tmp -> next)
                    tmp -> next -> prev = tmp -> prev;
                if (tmp == pl -> first)
                    pl -> first = tmp -> next;
                tmp -> next = tmp_last -> next;
                tmp_last -> next = tmp;
                tmp -> prev = tmp_last;
                tmp_last = tmp;
            }
        }
        tmp = step;
    }
    pl -> last = tmp_last;
    return;
}

int main() {
    struct List *pl;
    struct Node *tmp;
    struct Node *tmp1;
    pl = (struct List*) malloc(sizeof(struct List));
    tmp = (struct Node *) malloc(sizeof(struct Node));
    tmp -> elem = (char*) malloc (5*sizeof(char));
    strcpy(tmp -> elem, "A");
    tmp -> prev = NULL;
    tmp1 = (struct Node *) malloc (sizeof(struct Node));
    tmp1-> elem = (char*) malloc (5*sizeof(char));
    tmp -> next = tmp1;
    tmp1 -> prev = tmp;
    strcpy(tmp1 -> elem, "ABCD");
    pl -> first = tmp;
    tmp = (struct Node *) malloc(sizeof(struct Node));
    tmp -> elem = (char*) malloc (5*sizeof(char));
    tmp1 -> next = tmp;
    strcpy(tmp -> elem, "C");
    tmp -> prev = tmp1;
    tmp -> next = NULL;
    pl ->last = tmp;
    process(pl, "ABCD");
    tmp = pl->first;
    while (tmp != NULL){
        printf("%s\n", tmp -> elem);
        tmp = tmp -> next;
    }
    return 0;
}