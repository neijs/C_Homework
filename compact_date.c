#include "normal_input.h"

static unsigned short compact; /* 2 bytes for storing the date */

void date_to_compact(short, short, short);
void compact_to_date(short*, short*, short*);

void print_date();                /* 0 */
void print_day();                 /* 1 */
void print_month();               /* 2 */
void print_year();                /* 3 */
int change_day();                 /* 4 */
int change_month();               /* 5 */
int change_year();                /* 6 */
void print_compact();             /* 7 */

int choice(void);
int data_check(short, short, short);

int main() {
    short *date; /* Array for storing day, month and yeat */

    do {
        printf("Please enter your date in 'DD MM YY' format: ");
        if (!(date = get_short(3)))
            return 1;
    } while (data_check(date[0], date[1], date[2])); /* Reapeat until the user enters the correct date, AND skip data_check if the flag is raised */
    date_to_compact(date[0], date[1], date[2]); /* Store the date in a compact form */
    while (choice()); /* Work until the user wills to exit */
    free(date);
    return 0;
}

/* Store the date in 2 bytes: 5 bits for the day, 4 bits for the month and 7 bits for the year */
void date_to_compact(short day, short month, short year) {
    compact = (day << 11) | (month << 7) | year;
}

/* Unrap the compact date format */
void compact_to_date(short *day, short *month, short *year) {
    if (day)
        *day = compact >> 11;
    if (month)
        *month = (compact >> 7) & 0xf;
    if (year)
        *year = compact & 0x7f;
}

/* Print the date, using the compact form */
void print_date() {
    short day, month, year;

    compact_to_date(&day, &month, &year);
    printf("\nThe current date is: %02hi %02hi %02hi\n", day, month, year);
}

/* Print the day, using the compact form */
void print_day() {
    short day; 

    compact_to_date(&day, NULL, NULL);
    printf("\nThe current day is: %02hi\n", day);
}

/* Print the month, using the compact form */
void print_month() {
    short month;

    compact_to_date(NULL, &month, NULL);
    printf("\nThe current month is: %02hi\n", month);
}

/* Print the year, using the compact form */
void print_year() {
    short year;

    compact_to_date(NULL, NULL, &year);
    printf("\nThe current year is: %hi%02hi\n", (year <= 49) ? 20 : 19, year);
}

/* Change the value of the day */
int change_day() {
    short *day = NULL;
    short month, year;

    compact_to_date(NULL, &month, &year); /* Get stored values of month and year */
    do {
        if (day)
            free(day);
        printf("\nPlease enter the value of new day: ");
        if (!(day = get_short(1)))
            return 1;
    } while (data_check(*day, month, year)); /* Reapeat until the user enters the correct date, AND skip data_check if the flag is raised */
    date_to_compact(*day, month, year); /* Save the new date in the compact form */
    puts("\nThe day has been successfully changed!");
    free(day);
    return 0;
}

/* Change the value of the month */
int change_month() {
    short *month = NULL;
    short day, year;

    compact_to_date(&day, NULL, &year); /* Get stored values of day and year */
     do {
        if (month)
            free(month);
        printf("\nPlease enter the value of new month: ");
        if (!(month = get_short(1)))
            return 1;
    } while (data_check(day, *month, year)); /* Reapeat until the user enters the correct date, AND skip data_check if the flag is raised */
    date_to_compact(day, *month, year); /* Save the new date in the compact form */
    puts("\nThe month has been successfully changed!");
    free(month);
    return 0;
}

/* Change the value of the year */
int change_year() {
    short *year = NULL;
    short day, month;

    compact_to_date(&day, &month, NULL); /* Get stored values of day and month */
    do {
        if (year)
            free(year);
        printf("\nPlease enter the value of new year: ");
        if (!(year = get_short(1)))
            return 1;
    } while (data_check(day, month, *year)); /* Reapeat until the user enters the correct date, AND skip data_check if the flag is raised */
    date_to_compact(day, month, *year); /* Save the new date in the compact form */
    puts("\nThe year has been successfully changed!");
    free(year);
    return 0;
}

/* Print a binary representation of the compact date form */
void print_compact() {
    unsigned short temp = compact, mask = 0x8000; /* 1000 0000 0000 0000 -- mask for printing the first bit of 2 bytes */
    int i;

    puts("");
    for (i = 15; i >= 0; i--, mask >>= 1)
        printf("%hi", (temp & mask) >> i);
    puts("");
}

/* Get the user's response and run the corresponding function */
int choice(void) {
    char *option = NULL; /* Char value for storing the user's response */
    printf("\nPlease select one of the options: \n"
        "[0] -- print the date in the format DD MM YY\n"
        "[1] -- print only the day\n"
        "[2] -- print only the month\n"
        "[3] -- print only the year\n"
        "[4] -- change the day\n"
        "[5] -- change the month\n"
        "[6] -- change the year\n"
        "[7] -- output a binary representation of a compact format\n");
    if (!(option = get_char(1)))
        return 0;
    switch (*option) {
        case '0':
            print_date();
            break;
        case '1':
            print_day();
            break;
        case '2':
            print_month();
            break;
        case '3':
            print_year();
            break;
        case '4':
            if (change_day()) /* EOF was typed by the user. Exit the program */
                return 0;
            break;
        case '5':
            if (change_month()) /* EOF was typed by the user. Exit the program */
                return 0;
            break;
        case '6':
            if (change_year()) /* EOF was typed by the user. Exit the program */          
                return 0;
            break;
        case '7':
            print_compact();
            break;
        default:
            free(option);
            return 0; /* Stop */
    }
    free(option);
    return 1; /* Stay in the while loop, because the user is willing to continue */
}

/* Check if date values are correct */
int data_check(short day, short month, short year) {
    enum months { JAN = 1, FEB, MAR, APR, MAY, JUN, JUL, AUG, SEP, OCT, NOV, DEC };
    short ext_year = ((year <= 49) ? 2000 : 1900) + year; /* Extend the value of the year: 89 --> 1989, 23 --> 2023, etc. */

    if (day > 31 || day < 1 || month > 12 || month < 1 || year > 99 || year < 0){ /* Check for simple incorrect input */
        puts("\nThe date values are out of bounds");
        return 1;
    }
    else if ((month == APR || month == JUN || month == SEP || month == NOV) && day == 31){ /* Check for those 4 months with just 30 days in them */ 
        printf("\nIn %s there're only 30 days\n", (month == APR) ? "April" : (month == JUN) ? "June" : (month == SEP) ? "September" : (month == NOV) ? "November" : 0);
        return 1;
    }
    else if (month == FEB && ((ext_year % 4 == 0 && ext_year % 100 != 0) || ext_year % 400 == 0)) { /* Check for February in leap years */
        if (day > 29){
            printf("\nThe year %hi is a leap year, so there're only 29 days in February\n", ext_year);
            return 1;
        }
    }
    else if (month == FEB) /* Check for February in normal years */
        if (day > 28){
            printf("\nThe year %hi is not a leap year, so there're only 28 days in February\n", ext_year);
            return 1;
        }
    return 0;
}