#define BUFSIZE 100

char buf[BUFSIZE];
int bufp = 0;

int getch(void) {
    return (bufp > 0) ? buf[--bufp] : getchar();
}

/* Push character back on input */
void ungetch(int c) {
    if (bufp >= BUFSIZE)
        puts("Ungetch: too many characters");
    else
        buf[bufp++] = c;
}