#include <stdio.h>

struct rec {
    int x,y,z;
};

int main() {
    int i, j;
    FILE *f;
    struct rec r;

    f = fopen("junk", "w");
    if (!f)
        return 1;
    for (i = 1; i <= 10; i++) {
        fwrite(&i, sizeof(int), 1, f);
    }
    fclose(f);
    return 0;
}