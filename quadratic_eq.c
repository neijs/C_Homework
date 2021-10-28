#include "normal_input.h"
#include <math.h>


int solve(double, double, double);
int response(void);

int main() {
    double *params;

    do {
        printf("\nPlease enter the parameters of your quadratic equation. A, B and C: ");
        if (!(params = get_double(3)))
            return 1;
        solve(params[0], params[1], params[2]);
    } while (response()); /* While the user wants to continue solving */
    return 0;
}

/* Solve quadratic equation using entered parametres */
int solve(double A, double B, double C) { 
    double discriminant = 0, root1 = 0, root2 = 0;
    
    if (!A) {
        if (!B){
            if (!C)
                puts("\nThis quadratic equation has infinitely many roots!");
            else 
                puts("\nThis quadratic equation has no roots!");
        } else {
            if (!C)
                puts("\nThis quadratic equation has only 1 root: x = 0");
            else 
                printf("\nThis quadratic equation has only 1 root: %lf\n", -C/B);
        }
    } else {
        discriminant = B*B - 4*A*C;
        root1 = (-B + sqrt(discriminant))/(2*A);
        root2 = (-B - sqrt(discriminant))/(2*A);
        if (discriminant < 0) 
            puts("\nThe discriminant is less than zero, so there are no roots!");
        else if (discriminant == 0) 
            printf("\nThe discriminant is zero, so there is only one root: %lf\n", root1);
        else if (discriminant > 0)
            printf("\nThe discriminant is positive, so there are two roots: x1 = %lf, x2 = %lf\n", root1, root2);
    }
    return 0;
}

/* Gets the information from the user whether s/he wants to continue solving */
int response(void){
    char *response; /* Char value for storing the user's response */

    do {
        printf("Would you like to continue? [y/n]: ");
        if (!(response = get_char(1)))
            return 0;
        if (*response != 'y' && *response != 'n')
            puts("Incorrect input! Try again!");
    } while (*response != 'y' && *response != 'n'); /* Reapeat until the user enters the correct date, AND skip the check if the flag is raised */
    return (*response == 'y') ? 1 : 0;
}